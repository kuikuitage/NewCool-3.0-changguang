all:

	mipsel-linux-ar x  libiqiyiTVCWrapper.a
	mipsel-linux-ar x  libPlaybackSeq.a	
	mipsel-linux-ar x  libMDemux.a
	
	mipsel-linux-ar crs libiqiyiTVCWrapper.a *.o
	mipsel-linux-ranlib  libiqiyiTVCWrapper.a
	rm -fr *.o
clean:
	rm -fr  *.o

	
