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
// A source object for AMR audio files (as defined in RFC 3267, section 5)
// Implementation

#include "AMRAudioFileSource.hh"
#include "InputFile.hh"
#include "GroupsockHelper.hh"

////////// AMRAudioFileSource //////////

AMRAudioFileSource*
AMRAudioFileSource::createNew(UsageEnvironment& env, char const* fileName) {
#ifdef __LINUX__
  FILE* fid = NULL;
#else
  UFILE* fid = NULL;
#endif
  Boolean magicNumberOK = True;
  do {

    fid = OpenInputFile(env, fileName);
    if (fid == NULL) break;

    // Now, having opened the input file, read the first few bytes, to
    // check the required 'magic number':
    magicNumberOK = False; // until we learn otherwise
    Boolean isWideband = False; // by default
    unsigned numChannels = 1; // by default
    char buf[100];
    // Start with the first 6 bytes (the first 5 of which must be "#!AMR"):
#ifdef __LINUX__
    if (fread(buf, 1, 6, fid) < 6) break;
#else
    u32 read_size = 0;
    int ret = 0;
    ret = ufs_read(fid, buf, 6, &read_size);
    if ((read_size != 6) || (ret != FR_OK)) break;
#endif
    if (strncmp(buf, "#!AMR", 5) != 0) break; // bad magic #
    unsigned bytesRead = 6;

    // The next bytes must be "\n", "-WB\n", "_MC1.0\n", or "-WB_MC1.0\n"
    if (buf[5] == '-') {
      // The next bytes must be "WB\n" or "WB_MC1.0\n"
#ifdef __LINUX__
      if (fread(&buf[bytesRead], 1, 3, fid) < 3) break;
#else
      u32 read_size = 0;
      int ret = 0;
      ret = ufs_read(fid, &buf[bytesRead], 3, &read_size);
      if ((read_size != 3) || (ret != FR_OK)) break;
#endif
      if (strncmp(&buf[bytesRead], "WB", 2) != 0) break; // bad magic #
      isWideband = True;
      bytesRead += 3;
    }
    if (buf[bytesRead-1] == '_') {
      // The next bytes must be "MC1.0\n"
#ifdef __LINUX__
      if (fread(&buf[bytesRead], 1, 6, fid) < 6) break;
#else
      u32 read_size = 0;
      int ret = 0;
      ret = ufs_read(fid, &buf[bytesRead], 6, &read_size);
      if ((read_size != 6) || (ret != FR_OK)) break;
#endif
      if (strncmp(&buf[bytesRead], "MC1.0\n", 6) != 0) break; // bad magic #
      bytesRead += 6;

      // The next 4 bytes contain the number of channels:
      char channelDesc[4];
#ifdef __LINUX__
      if (fread(channelDesc, 1, 4, fid) < 4) break;
#else
      ret = ufs_read(fid, channelDesc, 4, &read_size);
      if ((read_size != 4) || (ret != FR_OK)) break;
#endif
      numChannels = channelDesc[3]&0xF;
    } else if (buf[bytesRead-1] != '\n') {
      break; // bad magic #
    }

    // If we get here, the magic number was OK:
    magicNumberOK = True;

#ifdef DEBUG
    fprintf(stderr, "isWideband: %d, numChannels: %d\n",
	    isWideband, numChannels);
#endif
    return new AMRAudioFileSource(env, fid, isWideband, numChannels);
  } while (0);

  // An error occurred:
  CloseInputFile(fid);
  if (!magicNumberOK) {
    env.setResultMsg("Bad (or nonexistent) AMR file header");
  }
  return NULL;
}

#ifdef __LINUX__
AMRAudioFileSource
::AMRAudioFileSource(UsageEnvironment& env, FILE* fid,
		     Boolean isWideband, unsigned numChannels)
  : AMRAudioSource(env, isWideband, numChannels),
    fFid(fid) {
}
#else
AMRAudioFileSource
::AMRAudioFileSource(UsageEnvironment& env, UFILE* fid,
		     Boolean isWideband, unsigned numChannels)
  : AMRAudioSource(env, isWideband, numChannels),
    fFid(fid) {
}
#endif

AMRAudioFileSource::~AMRAudioFileSource() {
  CloseInputFile(fFid);
}

// The mapping from the "FT" field to frame size.
// Values of 65535 are invalid.
#define FT_INVALID 65535
static unsigned short const frameSize[16] = {
  12, 13, 15, 17,
  19, 20, 26, 31,
  5, FT_INVALID, FT_INVALID, FT_INVALID,
  FT_INVALID, FT_INVALID, FT_INVALID, 0
};
static unsigned short const frameSizeWideband[16] = {
  17, 23, 32, 36,
  40, 46, 50, 58,
  60, 5, FT_INVALID, FT_INVALID,
  FT_INVALID, FT_INVALID, 0, 0
};

// Note: We should change the following to use asynchronous file reading, #####
// as we now do with ByteStreamFileSource. #####
void AMRAudioFileSource::doGetNextFrame() {
#ifdef __LINUX__
  if (feof(fFid) || ferror(fFid))
#else
  if (1)    //change by jiakun, should change for feof and ferror func based on ufs file system
#endif
  {
    handleClosure(this);
    return;
  }

  // Begin by reading the 1-byte frame header (and checking it for validity)
  while (1) {
#ifdef __LINUX__
    if (fread(&fLastFrameHeader, 1, 1, fFid) < 1)
#else
    u32 read_size;
    int ret;
    ret = ufs_read(fFid, (void *)&fLastFrameHeader, 1, &read_size);
    if ((read_size != 1) || (ret != FR_OK))
#endif
    {
      handleClosure(this);
      return;
    }
    if ((fLastFrameHeader&0x83) != 0) {
#ifdef DEBUG
      fprintf(stderr, "Invalid frame header 0x%02x (padding bits (0x83) are not zero)\n", fLastFrameHeader);
#endif
    } else {
      unsigned char ft = (fLastFrameHeader&0x78)>>3;
      fFrameSize = fIsWideband ? frameSizeWideband[ft] : frameSize[ft];
      if (fFrameSize == FT_INVALID) {
#ifdef DEBUG
	fprintf(stderr, "Invalid FT field %d (from frame header 0x%02x)\n",
		ft, fLastFrameHeader);
#endif
      } else {
	// The frame header is OK
#ifdef DEBUG
	fprintf(stderr, "Valid frame header 0x%02x -> ft %d -> frame size %d\n", fLastFrameHeader, ft, fFrameSize);
#endif
	break;
      }
    }
  }

  // Next, read the frame-block into the buffer provided:
  fFrameSize *= fNumChannels; // because multiple channels make up a frame-block
  if (fFrameSize > fMaxSize) {
    fNumTruncatedBytes = fFrameSize - fMaxSize;
    fFrameSize = fMaxSize;
  }
#ifdef __LINUX__
  fFrameSize = fread(fTo, 1, fFrameSize, fFid);
#else
  u32 read_size;
  int ret;
  ret = ufs_read(fFid, (void *)fTo, fFrameSize, &read_size);
  fFrameSize = (unsigned)read_size;
#endif

  // Set the 'presentation time':
  if (fPresentationTime.tv_sec == 0 && fPresentationTime.tv_usec == 0) {
    // This is the first frame, so use the current time:
#ifdef __LINUX__    
   // gettimeofday(&fPresentationTime, NULL);
    //gettimeofday(&fPresentationTime, NULL);//peacer del
    gettimeofday_replace(&fPresentationTime, NULL);

#else
    gettimeofday_replace(&fPresentationTime, NULL);
#endif
  } else {
    // Increment by the play time of the previous frame (20 ms)
    unsigned uSeconds	= fPresentationTime.tv_usec + 20000;
    fPresentationTime.tv_sec += uSeconds/1000000;
    fPresentationTime.tv_usec = uSeconds%1000000;
  }

  fDurationInMicroseconds = 20000; // each frame is 20 ms

  // Switch to another task, and inform the reader that he has data:
  nextTask() = envir().taskScheduler().scheduleDelayedTask(0,
				(TaskFunc*)FramedSource::afterGetting, this);
 }