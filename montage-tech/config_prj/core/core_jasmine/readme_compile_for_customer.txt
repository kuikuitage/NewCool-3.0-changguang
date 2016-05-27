compile README
-------------

1 modules
----------------
libPlaybackSeq.a
libMDemux.a
libDownloadFile.a
libneon.a
libopenssla.a
libopensslb.a


2 build
------------
   before you build modules, you should change the root directory name "core_jasmine" to "montage-tech".
   1) libPlaybackSeq.a
      go to the directory montage-tech\src\kware\file_playback, and implement make
      
   2) libMDemux.a
      go to the directory montage-tech\src\kware\demux_mp, and implement make
      
   3) libDownloadFile.a
      go to the directory montage-tech\src\kware\network\http_file, and implement make
      
   4) libneon.a
      go to the directory montage-tech\src\kware\network\HTTPC, and implement make
      
   5) libopenssla.a and libopensslb.a
      go to the directory montage-tech\src\kware\network\openssl-0.9.8, and implement make

