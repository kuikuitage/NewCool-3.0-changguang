/**********
  This library is free software; you can redistribute it and/or modify it under
  the terms of the GNU Lesser General Public License as published by the
  Free Software Foundation; either version 2.1 of the License, or (at your
  option) any later version. (See <http://www.gnu.org/copyleft/lesser.html>.)

  This library is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
  FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for
  more details.

  You should have received a copy of the GNU Lesser General Public License
  along with this library; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 **********/
// "liveMedia"
// Copyright (c) 1996-2012 Live Networks, Inc.  All rights reserved.
// RTP source for a common kind of payload format: Those that pack multiple,
// complete codec frames (as many as possible) into each RTP packet.
// Implementation

#include "MultiFramedRTPSource.hh"
#include "GroupsockHelper.hh"
#include <string.h>
extern int gettimeofday_replace(struct timeval* tp, int* /*tz*/) ;
////////// ReorderingPacketBuffer definition //////////

class ReorderingPacketBuffer {
	public:
		ReorderingPacketBuffer(BufferedPacketFactory* packetFactory);
		virtual ~ReorderingPacketBuffer();
		void reset();

		BufferedPacket* getFreePacket(MultiFramedRTPSource* ourSource);
		Boolean storePacket(BufferedPacket* bPacket);
		BufferedPacket* getNextCompletedPacket(Boolean& packetLossPreceded);
		void releaseUsedPacket(BufferedPacket* packet);
		
		void freePacket(BufferedPacket* packet) {
			
			if (packet != fSavedPacket) {
				LIV555_DEBUG("[%s] -- delete packet \n",__func__);
				delete packet;
				
			} else {
				LIV555_LOG("[%s] -- fSavedPacketFree = True\n",__func__);
				fSavedPacketFree = True;
			}
		}
		
		Boolean isEmpty() const { return fHeadPacket == NULL; }

		void setThresholdTime(unsigned uSeconds) { fThresholdTime = uSeconds; }
		void resetHaveSeenFirstPacket() { fHaveSeenFirstPacket = False; }

	private:
		BufferedPacketFactory* fPacketFactory;
		unsigned fThresholdTime; // uSeconds
		Boolean fHaveSeenFirstPacket; // used to set initial "fNextExpectedSeqNo"
		unsigned short fNextExpectedSeqNo;
		BufferedPacket* fHeadPacket;
		BufferedPacket* fTailPacket;
		BufferedPacket* fSavedPacket;
		// to avoid calling new/free in the common case
		Boolean fSavedPacketFree;
};


////////// MultiFramedRTPSource implementation //////////

	MultiFramedRTPSource
::MultiFramedRTPSource(UsageEnvironment& env, Groupsock* RTPgs,
		unsigned char rtpPayloadFormat,
		unsigned rtpTimestampFrequency,
		BufferedPacketFactory* packetFactory)
: RTPSource(env, RTPgs, rtpPayloadFormat, rtpTimestampFrequency) {
	reset();
	fReorderingBuffer = new ReorderingPacketBuffer(packetFactory);

	// Try to use a big receive buffer for RTP:
	increaseReceiveBufferTo(env, RTPgs->socketNum(), 50*1024);
}

void MultiFramedRTPSource::reset() {
	fCurrentPacketBeginsFrame = True; // by default
	fCurrentPacketCompletesFrame = True; // by default
	fAreDoingNetworkReads = False;
	fPacketReadInProgress = NULL;
	fNeedDelivery = False;
	fPacketLossInFragmentedFrame = False;
}

MultiFramedRTPSource::~MultiFramedRTPSource() {
	fRTPInterface.stopNetworkReading();
	delete fReorderingBuffer;
}

Boolean MultiFramedRTPSource
::processSpecialHeader(BufferedPacket* /*packet*/,
		unsigned& resultSpecialHeaderSize) {
	// Default implementation: Assume no special header:
	resultSpecialHeaderSize = 0;
	return True;
}

Boolean MultiFramedRTPSource
::packetIsUsableInJitterCalculation(unsigned char* /*packet*/,
		unsigned /*packetSize*/) {
	// Default implementation:
	return True;
}

void MultiFramedRTPSource::doStopGettingFrames() {
	fRTPInterface.stopNetworkReading();
	fReorderingBuffer->reset();
	reset();
}

void MultiFramedRTPSource::doGetNextFrame() {
	if (!fAreDoingNetworkReads) {
		// Turn on background read handling of incoming packets:
		fAreDoingNetworkReads = True;
		TaskScheduler::BackgroundHandlerProc* handler
			= (TaskScheduler::BackgroundHandlerProc*)&networkReadHandler;

		OS_PRINTF("[%s] add networkReadHandler for multiFramedRTPSource\n",__func__);
		fRTPInterface.startNetworkReading(handler);
	}

	fSavedTo = fTo;
	fSavedMaxSize = fMaxSize;
	fFrameSize = 0; // for now
	fNeedDelivery = True;
	doGetNextFrame1();
}

void MultiFramedRTPSource::doGetNextFrame1() {
	LIV555_DEBUG("[%s]--start\n",__func__);
	while (fNeedDelivery) {
		
		// If we already have packet data available, then deliver it now.
		Boolean packetLossPrecededThis;
		
		BufferedPacket* nextPacket = fReorderingBuffer->getNextCompletedPacket(packetLossPrecededThis);
		
		if (nextPacket == NULL) 
		{
			LIV555_LOG("[%s]--1--not find compelete packet in recordingbuffer!!! break\n",__func__);
			break;
		}
		
		fNeedDelivery = False;

		if (nextPacket->useCount() == 0) {
			// Before using the packet, check whether it has a special header
			// that needs to be processed:
			unsigned specialHeaderSize;
			if (!processSpecialHeader(nextPacket, specialHeaderSize)) {
				// Something's wrong with the header; reject the packet:
				fReorderingBuffer->releaseUsedPacket(nextPacket);
				fNeedDelivery = True;
				OS_PRINTF("[%s] --2--Something's wrong with the header;!!! break\n",__func__);
				break;
			}
			
			nextPacket->skip(specialHeaderSize);
		}

		// Check whether we're part of a multi-packet frame, and whether
		// there was packet loss that would render this packet unusable:
		if (fCurrentPacketBeginsFrame) {
			
			if (packetLossPrecededThis || fPacketLossInFragmentedFrame) {
				// We didn't get all of the previous frame.
				// Forget any data that we used from it:
				fTo = fSavedTo; fMaxSize = fSavedMaxSize;
				fFrameSize = 0;
			}
			
			fPacketLossInFragmentedFrame = False;
			
		} else if (packetLossPrecededThis) {
		
			// We're in a multi-packet frame, with preceding packet loss
			fPacketLossInFragmentedFrame = True;
		}
		
		if (fPacketLossInFragmentedFrame) {
			// This packet is unusable; reject it:
			OS_PRINTF("[%s] [ERROR] --3--This packet is unusable; reject it!!, break\n",__func__);
			fReorderingBuffer->releaseUsedPacket(nextPacket);
			fNeedDelivery = True;
			break;
		}

		// The packet is usable. Deliver all or part of it to our caller:
		unsigned frameSize;
		
		nextPacket->use(fTo, fMaxSize, frameSize, fNumTruncatedBytes,
				fCurPacketRTPSeqNum, fCurPacketRTPTimestamp,
				fPresentationTime, fCurPacketHasBeenSynchronizedUsingRTCP,
				fCurPacketMarkerBit);
		
		fFrameSize += frameSize;

		if (!nextPacket->hasUsableData()) {
			// We're completely done with this packet now
			LIV555_DEBUG("[%s] --4--We're completely done with this packet now, fFrameSize: %d\n",__func__,fFrameSize);
			fReorderingBuffer->releaseUsedPacket(nextPacket);
		}


		if (fCurrentPacketCompletesFrame) {

			LIV555_DEBUG("[%s] --5--We have all the data that the client wants \n",__func__);
			// We have all the data that the client wants.
			if (fNumTruncatedBytes > 0) {

				OS_PRINTF("MultiFramedRTPSource::doGetNextFrame1(): The total received frame size exceeds the client's buffer size\n");
				envir() << "MultiFramedRTPSource::doGetNextFrame1(): The total received frame size exceeds the client's buffer size ("
										     << fSavedMaxSize << ").  "
													       << fNumTruncatedBytes << " bytes of trailing data will be dropped!\n";
			}
			
			// Call our own 'after getting' function, so that the downstream object can consume the data:
			if (fReorderingBuffer->isEmpty()) {

				LIV555_DEBUG("[%s] --6--fReorderingBuffer->isEmpty()\n",__func__);
				// Common case optimization: There are no more queued incoming packets, so this code will not get
				// executed again without having first returned to the event loop.  Call our 'after getting' function
				// directly, because there's no risk of a long chain of recursion (and thus stack overflow):
				afterGetting(this);
				
			} else {
				// Special case: Call our 'after getting' function via the event loop.
				LIV555_DEBUG("[%s] --7--Special case: Call our 'after getting' function via the event loop \n",__func__);
				nextTask() = envir().taskScheduler().scheduleDelayedTask(0,
						(TaskFunc*)FramedSource::afterGetting, this);
			}
			
			LIV555_DEBUG("[%s] --8--We have all the data that the client wants \n",__func__);
		} else {
		
			// This packet contained fragmented data, and does not complete
			// the data that the client wants.  Keep getting data:
			
			fTo += frameSize; fMaxSize -= frameSize;
			fNeedDelivery = True;
			LIV555_DEBUG("[%s]--9-- contained fragmented data, not complete, Keep getting data, frameSize:%d\n",__func__,frameSize);
		}
		LIV555_DEBUG("[%s] --10-- fNeedDelivery: %d, while loop\n",__func__, fNeedDelivery);
		
	}
	LIV555_DEBUG("[%s]--end\n",__func__);
}

void MultiFramedRTPSource
::setPacketReorderingThresholdTime(unsigned uSeconds) {
	fReorderingBuffer->setThresholdTime(uSeconds);
}

#define ADVANCE(n) do { bPacket->skip(n); } while (0)

void MultiFramedRTPSource::networkReadHandler(MultiFramedRTPSource* source, int /*mask*/) {
	source->networkReadHandler1();
}

unsigned int rtp_seq_no_pre_bak[2][2]={0,0,0,0};

void MultiFramedRTPSource::networkReadHandler1() {
	LIV555_DEBUG("[%s]  start \n",__func__);
	BufferedPacket* bPacket = fPacketReadInProgress;
	if (bPacket == NULL) {
		// Normal case: Get a free BufferedPacket descriptor to hold the new network packet:
		bPacket = fReorderingBuffer->getFreePacket(this);
	}

	// Read the network packet, and perform sanity checks on the RTP header:
	Boolean readSuccess = False;
	
	do {
		
		Boolean packetReadWasIncomplete = fPacketReadInProgress != NULL;
		
		if (!bPacket->fillInData(fRTPInterface, packetReadWasIncomplete))
			break;
		
		if (packetReadWasIncomplete) {
			// We need additional read(s) before we can process the incoming packet:
			fPacketReadInProgress = bPacket;
			OS_PRINTF("[%s]@@@@ packet incomplete!!!\n",__func__);
			return;
		} else {
			fPacketReadInProgress = NULL;
		}

		
#ifdef TEST_LOSS
		setPacketReorderingThresholdTime(0);
		// don't wait for 'lost' packets to arrive out-of-order later
		if ((our_random()%10) == 0) break; // simulate 10% packet loss
#endif

		// Check for the 12-byte RTP header:
		if (bPacket->dataSize() < 12) 
		{
			OS_PRINTF("[%s][ERROR] udp packet error size < 12 !!!!!\n",__func__);
			break;
		}
		
		unsigned rtpHdr = ntohl(*(u_int32_t*)(bPacket->data())); 
		ADVANCE(4);
		
		Boolean rtpMarkerBit = (rtpHdr&0x00800000) != 0;
		
		unsigned rtpTimestamp = ntohl(*(u_int32_t*)(bPacket->data()));
		ADVANCE(4);
		
		unsigned rtpSSRC = ntohl(*(u_int32_t*)(bPacket->data())); 
		ADVANCE(4);

		// Check the RTP version number (it should be 2):
		if ((rtpHdr&0xC0000000) != 0x80000000) 
		{
			OS_PRINTF("[%s][ERROR] fail to check RTP Version !!!!!\n",__func__);
			break;
		}

		// Skip over any CSRC identifiers in the header:
		unsigned cc = (rtpHdr>>24)&0xF;
		if (bPacket->dataSize() < cc) 
		{
			OS_PRINTF("[%s][ERROR] fail to check CSRC !!!!!\n",__func__);
			break;
		}
		
		ADVANCE(cc*4);

		// Check for (& ignore) any RTP header extension
		if (rtpHdr&0x10000000) {
			
			if (bPacket->dataSize() < 4) 
			{
				OS_PRINTF("[%s][ERROR] 1111 !!!!!\n",__func__);
				break;
			}
			
			unsigned extHdr = ntohl(*(u_int32_t*)(bPacket->data())); 
			ADVANCE(4);
			
			unsigned remExtSize = 4*(extHdr&0xFFFF);
			if (bPacket->dataSize() < remExtSize) 
			{
				OS_PRINTF("[%s][ERROR] 2222 !!!!!\n",__func__);
				break;
			}
			
			ADVANCE(remExtSize);
		}

		// Discard any padding bytes:
		if (rtpHdr&0x20000000) {
			if (bPacket->dataSize() == 0) 
			{
				OS_PRINTF("[%s][ERROR] 333 !!!!!\n",__func__);
				break;
			}
			
			unsigned numPaddingBytes
				= (unsigned)(bPacket->data())[bPacket->dataSize()-1];
			
			if (bPacket->dataSize() < numPaddingBytes) 
			{
				OS_PRINTF("[%s][ERROR] 4444 !!!!!\n",__func__);
				break;
			}
			
			bPacket->removePadding(numPaddingBytes);
		}
		
		// Check the Payload Type.
		if ((unsigned char)((rtpHdr&0x007F0000)>>16)
				!= rtpPayloadFormat()) {

			OS_PRINTF("[%s][ERROR] 5555!!!!!\n",__func__);
			break;
		}

		// The rest of the packet is the usable data.  Record and save it:
		if (rtpSSRC != fLastReceivedSSRC) {
			// The SSRC of incoming packets has changed.  Unfortunately we don't yet handle streams that contain multiple SSRCs,
			// but we can handle a single-SSRC stream where the SSRC changes occasionally:
			fLastReceivedSSRC = rtpSSRC;
			fReorderingBuffer->resetHaveSeenFirstPacket();
		}
		
		unsigned short rtpSeqNo = (unsigned short)(rtpHdr&0xFFFF);
		
		Boolean usableInJitterCalculation
			= packetIsUsableInJitterCalculation((bPacket->data()),
					bPacket->dataSize());
		
		struct timeval presentationTime; // computed by:
		Boolean hasBeenSyncedUsingRTCP; // computed by:
		
		receptionStatsDB().noteIncomingPacket(rtpSSRC, rtpSeqNo, rtpTimestamp,
					timestampFrequency(),
					usableInJitterCalculation, presentationTime,
					hasBeenSyncedUsingRTCP, bPacket->dataSize());


		// Fill in the rest of the packet descriptor, and store it:
		struct timeval timeNow;
#ifdef __LINUX__
		//  gettimeofday(&timeNow, NULL);
		gettimeofday_replace(&timeNow, NULL);
#else
		gettimeofday_replace(&timeNow, NULL);
#endif

		bPacket->assignMiscParams(rtpSeqNo, rtpTimestamp, presentationTime,
				hasBeenSyncedUsingRTCP, rtpMarkerBit,
				timeNow);
#if 0//debug lost packets

		if(rtp_seq_no_pre_bak[0][0]==0){
			rtp_seq_no_pre_bak[0][0] =  rtpSSRC;
			rtp_seq_no_pre_bak[0][1] =  rtpSeqNo;
			OS_PRINTF("[%s] debug init! SSRC: %d, seq no: %d\n",__func__,rtpSSRC, rtpSeqNo);
		}
		else{
			if(rtpSSRC == rtp_seq_no_pre_bak[0][0]){
				if(rtpSeqNo != (rtp_seq_no_pre_bak[0][1] + 1)){
					OS_PRINTF("[%s][lost]SSRC: %d, seq no: %d( bak:%d)\n",__func__,rtpSSRC, rtpSeqNo, rtp_seq_no_pre_bak[0][1]);
					rtp_seq_no_pre_bak[0][1] =  rtpSeqNo;
				}
				else{
					rtp_seq_no_pre_bak[0][1] =  rtpSeqNo;
					//OS_PRINTF("[%s] SSRC: %d, seq no: %d( bak:%d)\n",__func__,rtpSSRC, rtpSeqNo, rtp_seq_no_pre_bak[0][1]);
				}
			}
			else{
				if(rtp_seq_no_pre_bak[1][0]==0){
					rtp_seq_no_pre_bak[1][0] =  rtpSSRC;
					rtp_seq_no_pre_bak[1][1] =  rtpSeqNo;
					OS_PRINTF("[%s] debug init! SSRC: %d, seq no: %d\n",__func__,rtpSSRC, rtpSeqNo);
				}
				else{
					if(rtpSSRC == rtp_seq_no_pre_bak[1][0]){
						if(rtpSeqNo != (rtp_seq_no_pre_bak[1][1] + 1)){
							OS_PRINTF("[%s][lost] SSRC: %d, seq no: %d( bak:%d)\n",__func__,rtpSSRC, rtpSeqNo, rtp_seq_no_pre_bak[1][1]);
							rtp_seq_no_pre_bak[1][1] =  rtpSeqNo;
						}
						else{
							rtp_seq_no_pre_bak[1][1] =  rtpSeqNo;
							//OS_PRINTF("[%s] SSRC: %d, seq no: %d( bak:%d)\n",__func__,rtpSSRC, rtpSeqNo, rtp_seq_no_pre_bak[0][1]);
						}
					}

				}

			}
		}
#endif		
		//OS_PRINTF("[%s] ----rtpSSRC:%d, rtpSeqNo:%d, bak0[%d, %d], bak1[%d, %d] \n",__func__,rtpSSRC,rtpSeqNo,\
		//	rtp_seq_no_pre_bak[0][0], rtp_seq_no_pre_bak[0][1], rtp_seq_no_pre_bak[1][0], rtp_seq_no_pre_bak[1][1]);
		if (!fReorderingBuffer->storePacket(bPacket)) 
		{
			OS_PRINTF("[%s][ERROR] fail to store packet !!!!!\n",__func__);
			break;
		}

		readSuccess = True;
		
	} while (0);

	
	if (!readSuccess) 
	{
		fReorderingBuffer->freePacket(bPacket);
	}

	doGetNextFrame1();
	
	// If we didn't get proper data this time, we'll get another chance
	LIV555_DEBUG("[%s]  end \n",__func__);
	
}


////////// BufferedPacket and BufferedPacketFactory implementation /////

#define MAX_PACKET_SIZE 10000

BufferedPacket::BufferedPacket()
	: fPacketSize(MAX_PACKET_SIZE),
	fNextPacket(NULL) {
	//fBuf(new unsigned char[MAX_PACKET_SIZE]),
	//OS_PRINTF("[%s]@@BufferedPacket()!!!\n",__func__);
#if 0//add for debug memory use
{
	mem_user_dbg_info_t dbg_info;
	mtos_mem_user_debug(&dbg_info);
	OS_PRINTF("[%s]-1-- alloced[%d], alloced_peak[%d]\n",__func__,dbg_info.alloced, dbg_info.alloced_peak);
}
#endif	
	fBuf = new unsigned char[MAX_PACKET_SIZE];
	//OS_PRINTF("[%s] new buf : %d Bytes\n",__func__,MAX_PACKET_SIZE);
	}

BufferedPacket::~BufferedPacket() {
	delete fNextPacket;
	delete[] fBuf;
	//OS_PRINTF("[%s]@@~BufferedPacket!!!\n",__func__);
}

void BufferedPacket::reset() {
	fHead = fTail = 0;
	fUseCount = 0;
	fIsFirstPacket = False; // by default
}

// The following function has been deprecated:
unsigned BufferedPacket
::nextEnclosedFrameSize(unsigned char*& /*framePtr*/, unsigned dataSize) {
	// By default, use the entire buffered data, even though it may consist
	// of more than one frame, on the assumption that the client doesn't
	// care.  (This is more efficient than delivering a frame at a time)
	return dataSize;
}

void BufferedPacket
::getNextEnclosedFrameParameters(unsigned char*& framePtr, unsigned dataSize,
		unsigned& frameSize,
		unsigned& frameDurationInMicroseconds) {
	// By default, use the entire buffered data, even though it may consist
	// of more than one frame, on the assumption that the client doesn't
	// care.  (This is more efficient than delivering a frame at a time)

	// For backwards-compatibility with existing uses of (the now deprecated)
	// "nextEnclosedFrameSize()", call that function to implement this one:
	frameSize = nextEnclosedFrameSize(framePtr, dataSize);

	frameDurationInMicroseconds = 0; // by default.  Subclasses should correct this.
}

Boolean BufferedPacket::fillInData(RTPInterface& rtpInterface, Boolean& packetReadWasIncomplete) {
	if (!packetReadWasIncomplete)
		reset();

	unsigned numBytesRead;
	struct sockaddr_in fromAddress;
	if (!rtpInterface.handleRead(&fBuf[fTail], fPacketSize-fTail, numBytesRead, fromAddress, packetReadWasIncomplete)) {
		return False;
	}
	fTail += numBytesRead;
	return True;
}

void BufferedPacket
::assignMiscParams(unsigned short rtpSeqNo, unsigned rtpTimestamp,
		struct timeval presentationTime,
		Boolean hasBeenSyncedUsingRTCP, Boolean rtpMarkerBit,
		struct timeval timeReceived) {
		
	fRTPSeqNo = rtpSeqNo;
	fRTPTimestamp = rtpTimestamp;
	fPresentationTime = presentationTime;
	fHasBeenSyncedUsingRTCP = hasBeenSyncedUsingRTCP;
	fRTPMarkerBit = rtpMarkerBit;
	fTimeReceived = timeReceived;
	
}

void BufferedPacket::skip(unsigned numBytes) {
	fHead += numBytes;
	if (fHead > fTail) fHead = fTail;
}

void BufferedPacket::removePadding(unsigned numBytes) {
	if (numBytes > fTail-fHead) numBytes = fTail-fHead;
	fTail -= numBytes;
}

void BufferedPacket::appendData(unsigned char* newData, unsigned numBytes) {
	if (numBytes > fPacketSize-fTail) numBytes = fPacketSize - fTail;
	memmove(&fBuf[fTail], newData, numBytes);
	fTail += numBytes;
}

void BufferedPacket::use(unsigned char* to, unsigned toSize,
		unsigned& bytesUsed, unsigned& bytesTruncated,
		unsigned short& rtpSeqNo, unsigned& rtpTimestamp,
		struct timeval& presentationTime,
		Boolean& hasBeenSyncedUsingRTCP,
		Boolean& rtpMarkerBit) {
		
	unsigned char* origFramePtr = &fBuf[fHead];
	unsigned char* newFramePtr = origFramePtr; // may change in the call below
	
	unsigned frameSize, frameDurationInMicroseconds;
	getNextEnclosedFrameParameters(newFramePtr, fTail - fHead,frameSize, frameDurationInMicroseconds);
	if (frameSize > toSize) {
		bytesTruncated += frameSize - toSize;
		bytesUsed = toSize;
	} else {
		bytesTruncated = 0;
		bytesUsed = frameSize;
	}

	memmove(to, newFramePtr, bytesUsed);
	fHead += (newFramePtr - origFramePtr) + frameSize;
	++fUseCount;

	rtpSeqNo = fRTPSeqNo;
	rtpTimestamp = fRTPTimestamp;
	presentationTime = fPresentationTime;
	hasBeenSyncedUsingRTCP = fHasBeenSyncedUsingRTCP;
	rtpMarkerBit = fRTPMarkerBit;

	//OS_PRINTF("[%s]-- rtpSeqNo[%d] rtpTimestamp[%d] hasBeenSyncedUsingRTCP[%d] rtpMarkerBit[%d]\n",\
	//	__func__,rtpSeqNo,rtpTimestamp,hasBeenSyncedUsingRTCP,rtpMarkerBit);
	
	// Update "fPresentationTime" for the next enclosed frame (if any):
	fPresentationTime.tv_usec += frameDurationInMicroseconds;
	if (fPresentationTime.tv_usec >= 1000000) {
		fPresentationTime.tv_sec += fPresentationTime.tv_usec/1000000;
		fPresentationTime.tv_usec = fPresentationTime.tv_usec%1000000;
	}
}

BufferedPacketFactory::BufferedPacketFactory() {
}

BufferedPacketFactory::~BufferedPacketFactory() {
}

BufferedPacket* BufferedPacketFactory
::createNewPacket(MultiFramedRTPSource* /*ourSource*/) {
	return new BufferedPacket;
}


////////// ReorderingPacketBuffer implementation //////////

	ReorderingPacketBuffer
::ReorderingPacketBuffer(BufferedPacketFactory* packetFactory)
	: fThresholdTime(100000) /* default reordering threshold: 100 ms */,
	fHaveSeenFirstPacket(False), fHeadPacket(NULL), fTailPacket(NULL), fSavedPacket(NULL), fSavedPacketFree(True) {
		fPacketFactory = (packetFactory == NULL)
			? (new BufferedPacketFactory)
			: packetFactory;
	}

ReorderingPacketBuffer::~ReorderingPacketBuffer() {
	reset();
	delete fPacketFactory;
}

void ReorderingPacketBuffer::reset() {
	
	if (fSavedPacketFree) 
		delete fSavedPacket; // because fSavedPacket is not in the list
		
	delete fHeadPacket; // will also delete fSavedPacket if it's in the list
	
	resetHaveSeenFirstPacket();
	
	fHeadPacket = fTailPacket = fSavedPacket = NULL;
}

BufferedPacket* ReorderingPacketBuffer::getFreePacket(MultiFramedRTPSource* ourSource) {
	
	if (fSavedPacket == NULL) { // we're being called for the first time
		fSavedPacket = fPacketFactory->createNewPacket(ourSource);
		fSavedPacketFree = True;
	}

	if (fSavedPacketFree == True) {
		fSavedPacketFree = False;
		return fSavedPacket;
	} else {
		return fPacketFactory->createNewPacket(ourSource);
	}
}

Boolean ReorderingPacketBuffer::storePacket(BufferedPacket* bPacket) {
	
	unsigned short rtpSeqNo = bPacket->rtpSeqNo();
	LIV555_DEBUG("[%s] ----start , rtpSeqNo: %d\n",__func__,rtpSeqNo);
	if (!fHaveSeenFirstPacket) {
		fNextExpectedSeqNo = rtpSeqNo; // initialization
		bPacket->isFirstPacket() = True;
		fHaveSeenFirstPacket = True;
	}

	// Ignore this packet if its sequence number is less than the one
	// that we're looking for (in this case, it's been excessively delayed).
	if (seqNumLT(rtpSeqNo, fNextExpectedSeqNo))
	{
		OS_PRINTF("[%s][ERROR][ERROR] this packet is excessively delayed !!!\n",__func__);
		OS_PRINTF("rtpSeqNo[%ld] fNextExpectedSeqNo[%ld]\n",__func__,rtpSeqNo,fNextExpectedSeqNo);
		return False;
	}

	if (fTailPacket == NULL) {
		// Common case: There are no packets in the queue; this will be the first one:
		bPacket->nextPacket() = NULL;
		fHeadPacket = fTailPacket = bPacket;
		//OS_PRINTF("[%s]There are no packets in the queue; this will be the first one\n",__func__);
		return True;
	}

	if (seqNumLT(fTailPacket->rtpSeqNo(), rtpSeqNo)) {
		// The next-most common case: There are packets already in the queue; this packet arrived in order => put it at the tail:
		bPacket->nextPacket() = NULL;
		fTailPacket->nextPacket() = bPacket;
		fTailPacket = bPacket;
		//OS_PRINTF("[%s]this packet arrived in order => put it at the tail\n",__func__);
		return True;
	} 

	if (rtpSeqNo == fTailPacket->rtpSeqNo()) {
		// This is a duplicate packet - ignore it
		OS_PRINTF("[%s][ERROR] This is a duplicate packet!!\n",__func__);
		return False;
	}


	// Rare case: This packet is out-of-order.  Run through the list (from the head), to figure out where it belongs:
	BufferedPacket* beforePtr = NULL;
	BufferedPacket* afterPtr = fHeadPacket;
	
	while (afterPtr != NULL) {
		
		if (seqNumLT(rtpSeqNo, afterPtr->rtpSeqNo())) 
		{
			OS_PRINTF("[%s][ERROR]22222222!\n",__func__);
			break; // it comes here
		}
			
		if (rtpSeqNo == afterPtr->rtpSeqNo()) {
			// This is a duplicate packet - ignore it
			OS_PRINTF("[%s][ERROR]This is a duplicate packet - ignore it!\n",__func__);
			return False;
		}

		beforePtr = afterPtr;
		afterPtr = afterPtr->nextPacket();
	}

	// Link our new packet between "beforePtr" and "afterPtr":
	bPacket->nextPacket() = afterPtr;
	if (beforePtr == NULL) {
		fHeadPacket = bPacket;
	} else {
		beforePtr->nextPacket() = bPacket;
	}

	return True;
}

void ReorderingPacketBuffer::releaseUsedPacket(BufferedPacket* packet) {
	// ASSERT: packet == fHeadPacket
	// ASSERT: fNextExpectedSeqNo == packet->rtpSeqNo()
	++fNextExpectedSeqNo; // because we're finished with this packet now

	fHeadPacket = fHeadPacket->nextPacket();
	if (!fHeadPacket) { 
		fTailPacket = NULL;
	}
	packet->nextPacket() = NULL;
	LIV555_DEBUG("[%s] -- call freePacket, fNextExpectedSeqNo: %d,  fHeadPacket: 0x%x\n",__func__,fNextExpectedSeqNo, fHeadPacket);
	freePacket(packet);
}

BufferedPacket* ReorderingPacketBuffer
::getNextCompletedPacket(Boolean& packetLossPreceded) {

	if (fHeadPacket == NULL) 
		return NULL;

	// Check whether the next packet we want is already at the head
	// of the queue:
	// ASSERT: fHeadPacket->rtpSeqNo() >= fNextExpectedSeqNo
	//OS_PRINTF("\n[%s] == rtpSeqNo[ %d ] fNextExpectedSeqNo[%d]\n",\
	//		__func__, fHeadPacket->rtpSeqNo(), fNextExpectedSeqNo);
	if (fHeadPacket->rtpSeqNo() == fNextExpectedSeqNo) {
		packetLossPreceded = fHeadPacket->isFirstPacket();
		// (The very first packet is treated as if there was packet loss beforehand.)
		//OS_PRINTF("XXXX\n");
		return fHeadPacket;
	}
	else{
		OS_PRINTF("[%s] --seqno unexpected! rtpSeqNo[%d] fNextExpectedSeqNo[%d], fThresholdTime[%d]\n",\
			__func__, fHeadPacket->rtpSeqNo(), fNextExpectedSeqNo, fThresholdTime);
	}

	// We're still waiting for our desired packet to arrive.  However, if
	// our time threshold has been exceeded, then forget it, and return
	// the head packet instead:
	Boolean timeThresholdHasBeenExceeded;
	
	if (fThresholdTime == 0) {
		
		timeThresholdHasBeenExceeded = True; // optimization
		
	} else {

	
		struct timeval timeNow;
#ifdef __LINUX__
		// gettimeofday(&timeNow, NULL);
		gettimeofday_replace(&timeNow, NULL);
#else
		gettimeofday_replace(&timeNow, NULL);
#endif


		unsigned uSecondsSinceReceived
			= (timeNow.tv_sec - fHeadPacket->timeReceived().tv_sec)*1000000
			+ (timeNow.tv_usec - fHeadPacket->timeReceived().tv_usec);

		timeThresholdHasBeenExceeded = uSecondsSinceReceived > fThresholdTime;


		//peacer add only for debug
		//in future we should remove these code !!!!!!!!!!!!!!!!!!
		if(uSecondsSinceReceived > 100000)
		{
			OS_PRINTF("SinceRecv:%ld\n",uSecondsSinceReceived);
		}
		
	}

	
	if (timeThresholdHasBeenExceeded) {
		fNextExpectedSeqNo = fHeadPacket->rtpSeqNo();
		// we've given up on earlier packets now
		packetLossPreceded = True;
		OS_PRINTF("[%s] --we've given up on earlier packets now, set fNextExpectedSeqNo: %d\n",__func__,fNextExpectedSeqNo);
		return fHeadPacket;
	}

	// Otherwise, keep waiting for our desired packet to arrive:
	return NULL;
}
