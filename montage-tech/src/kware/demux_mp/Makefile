#TOPDIR = .$(shell pwd | sed 's/\(.*\)\/montage-tech\(.*\)/\2/g' \
	 #|sed 's/\/[0-9a-zA-Z]*/\/../g')
#TOPDIR = .$(shell pwd | sed 's/\(.*\)\/montage-tech\(.*\)/\2/g' | sed 's:/[^/]*:/..:g')

TOPDIR=../../..

-include $(TOPDIR)/common.mk



OBJS :=\
   ./demux_mp.o\
   ./demux_mp_misc.o\
\
\
\
./mplayer/bstr.o\
./mplayer/path.o\
./mplayer/av_helpers.o\
./mplayer/mpcommon.o\
./mplayer/mp_msg.o\
./mplayer/mp_strings.o\
./mplayer/mp_func_trans.o\
./mplayer/codec-cfg.o\
./mplayer/m_struct.o\
./mplayer/m_option.o\
./mplayer/av_opts.o\
\
\
\
./mplayer/libmpdemux/mp3_hdr.o\
./mplayer/libmpdemux/demuxer.o\
./mplayer/libmpdemux/parse_es.o\
./mplayer/libmpdemux/demux_lavf.o\
./mplayer/libmpdemux/demux_real.o\
./mplayer/libmpdemux/demux_demuxers.o\
./mplayer/libmpdemux/demux_ts.o\
./mplayer/libmpdemux/demux_audio.o\
./mplayer/libmpdemux/demux_avi.o\
./mplayer/libmpdemux/mp_taglists.o\
./mplayer/libmpdemux/aviprint.o\
./mplayer/libmpdemux/aviheader.o\
./mplayer/libmpdemux/extension.o\
./mplayer/libmpdemux/mpeg_hdr.o\
./mplayer/libmpdemux/demux_mpg.o\
./mplayer/libmpdemux/asfheader.o\
./mplayer/libmpdemux/demux_asf.o\
\
\
\
./mplayer/libass/ass_fontconfig.o\
./mplayer/libass/ass.o\
./mplayer/libass/ass_drawing.o\
./mplayer/libass/ass_shaper.o\
./mplayer/libass/ass_font.o\
./mplayer/libass/ass_utils.o\
./mplayer/libass/ass_render.o\
./mplayer/libass/ass_render_api.o\
./mplayer/libass/ass_library.o\
./mplayer/libass/ass_bitmap.o\
./mplayer/libass/ass_cache.o\
./mplayer/libass/ass_parse.o\
./mplayer/libass/ass_strtod.o\
\
\
\
./mplayer/ffmpeg/libavformat/options.o\
./mplayer/ffmpeg/libavformat/id3v1.o\
./mplayer/ffmpeg/libavformat/rmsipr.o\
./mplayer/ffmpeg/libavformat/matroskadec.o\
./mplayer/ffmpeg/libavformat/flvenc.o\
./mplayer/ffmpeg/libavformat/matroska.o\
./mplayer/ffmpeg/libavformat/id3v2.o\
./mplayer/ffmpeg/libavformat/metadata.o\
./mplayer/ffmpeg/libavformat/riff.o\
./mplayer/ffmpeg/libavformat/dv.o\
./mplayer/ffmpeg/libavformat/formatutils.o\
./mplayer/ffmpeg/libavformat/avidec.o\
./mplayer/ffmpeg/libavformat/mpegts.o\
./mplayer/ffmpeg/libavformat/flvdec.o\
./mplayer/ffmpeg/libavformat/isom.o\
./mplayer/ffmpeg/libavformat/rtmppkt.o\
./mplayer/ffmpeg/libavformat/rtmpproto.o\
./mplayer/ffmpeg/libavformat/ff_network.o\
./mplayer/ffmpeg/libavformat/os_support.o\
./mplayer/ffmpeg/libavformat/ff_tcp.o\
./mplayer/ffmpeg/libavformat/mov.o\
./mplayer/ffmpeg/libavformat/aviobuf.o\
./mplayer/ffmpeg/libavformat/avlanguage.o\
./mplayer/ffmpeg/libavformat/asfcrypt.o\
./mplayer/ffmpeg/libavformat/asf.o\
./mplayer/ffmpeg/libavformat/asfdec.o\
./mplayer/ffmpeg/libavformat/allformats.o\
./mplayer/ffmpeg/libavformat/mov_chan.o\
./mplayer/ffmpeg/libavformat/avio.o\
./mplayer/ffmpeg/libavformat/urldecode.o\
./mplayer/ffmpeg/libavformat/ff_hls.o\
./mplayer/ffmpeg/expat/lib/xmlparse.o\
./mplayer/ffmpeg/expat/lib/xmltok.o\
./mplayer/ffmpeg/expat/lib/xmlrole.o\
./mplayer/ffmpeg/libavformat/f4mmanifest.o\
./mplayer/ffmpeg/libavformat/amfmetadata.o\
./mplayer/ffmpeg/libavformat/f4fbox.o\
./mplayer/ffmpeg/libavformat/flvtag.o\
./mplayer/ffmpeg/libavformat/ff_hds.o\
./mplayer/ffmpeg/libavformat/ff_crypto.o\
./mplayer/ffmpeg/libavformat/ff_http.o\
./mplayer/ffmpeg/libavformat/ff_httpauth.o\
./mplayer/ffmpeg/libavformat/librtmp.o\
./mplayer/ffmpeg/libavformat/ff_ac3dec.o\
./mplayer/ffmpeg/libavformat/ff_rawdec.o\
./mplayer/ffmpeg/libavformat/mp3dec.o\
./mplayer/ffmpeg/libavformat/wav.o\
./mplayer/ffmpeg/libavformat/pcm.o\
\
\
\
./mplayer/ffmpeg/libavutil/dict.o\
./mplayer/ffmpeg/libavutil/ff_aes.o\
./mplayer/ffmpeg/libavutil/imgutils.o\
./mplayer/ffmpeg/libavutil/timecode.o\
./mplayer/ffmpeg/libavutil/rational.o\
./mplayer/ffmpeg/libavutil/pixdesc.o\
./mplayer/ffmpeg/libavutil/audioconvert.o\
./mplayer/ffmpeg/libavutil/utilutils.o\
./mplayer/ffmpeg/libavutil/parseutils.o\
./mplayer/ffmpeg/libavutil/bprint.o\
./mplayer/ffmpeg/libavutil/opt.o\
./mplayer/ffmpeg/libavutil/mathematics.o\
./mplayer/ffmpeg/libavutil/utiltime.o\
./mplayer/ffmpeg/libavutil/samplefmt.o\
./mplayer/ffmpeg/libavutil/utilmem.o\
./mplayer/ffmpeg/libavutil/base64.o\
./mplayer/ffmpeg/libavutil/crc.o\
./mplayer/ffmpeg/libavutil/avstring.o\
./mplayer/ffmpeg/libavutil/util_log.o\
./mplayer/ffmpeg/libavutil/eval.o\
./mplayer/ffmpeg/libavutil/rc4.o\
./mplayer/ffmpeg/libavutil/des.o\
./mplayer/ffmpeg/libavutil/sha.o\
./mplayer/ffmpeg/libavutil/lfg.o\
./mplayer/ffmpeg/libavutil/ff_md5.o\
./mplayer/ffmpeg/libavutil/md5_2.o\
./mplayer/ffmpeg/libavutil/random_seed.o\
\
\
./mplayer/ffmpeg/libavcodec/rawdec.o\
./mplayer/ffmpeg/libavcodec/imgconvert.o\
./mplayer/ffmpeg/libavcodec/codec_desc.o\
./mplayer/ffmpeg/libavcodec/allcodecs.o\
./mplayer/ffmpeg/libavcodec/h264.o\
./mplayer/ffmpeg/libavcodec/mpegvideo.o\
./mplayer/ffmpeg/libavcodec/h264_parser.o\
./mplayer/ffmpeg/libavcodec/golomb.o\
./mplayer/ffmpeg/libavcodec/avpacket.o\
./mplayer/ffmpeg/libavcodec/parser.o\
./mplayer/ffmpeg/libavcodec/bitstream_filter.o\
./mplayer/ffmpeg/libavcodec/codecutils.o\
./mplayer/ffmpeg/libavcodec/h264_mp4toannexb_bsf.o\
./mplayer/ffmpeg/libavcodec/mpeg4audio.o\
./mplayer/ffmpeg/libavcodec/codecraw.o\
./mplayer/ffmpeg/libavcodec/codecoptions.o\
./mplayer/ffmpeg/libavcodec/dv_profile.o\
./mplayer/ffmpeg/libavcodec/ac3_parser.o\
./mplayer/ffmpeg/libavcodec/ac3tab.o\
./mplayer/ffmpeg/libavcodec/mpegaudiodecheader.o\
./mplayer/ffmpeg/libavcodec/mpegaudiodata.o\
\
\
\
./mplayer/stream/stream.o\
./mplayer/stream/cache2.o\
./mplayer/stream/register_net_stream.o\
./mplayer/stream/http.o\
./mplayer/stream/stream_file.o\
./mplayer/stream/open.o\
./mplayer/stream/network.o\
./mplayer/stream/tcp_mp.o\
./mplayer/stream/cookies.o\
./mplayer/stream/url.o\
./mplayer/stream/rtp.o\
./mplayer/stream/stream_rtp.o\
./mplayer/stream/stream_rtsp.o\
./mplayer/stream/asf_mmst_streaming.o\
./mplayer/stream/asf_streaming.o\
./mplayer/stream/stream_ffmpeg.o\
./mplayer/stream/stream_live555.o\
./mplayer/stream/stream_fifo.o\
\
\
\
./mplayer/stream/freesdp/common.o\
./mplayer/stream/freesdp/errorlist.o\
./mplayer/stream/freesdp/freesdp_parser.o\
\
\
\
./mplayer/stream/librtsp/rtsp.o\
./mplayer/stream/librtsp/rtsp_rtp.o\
./mplayer/stream/librtsp/rtsp_session.o\
\
\
\
./mplayer/stream/realrtsp/asmrp.o\
./mplayer/stream/realrtsp/real.o\
./mplayer/stream/realrtsp/rmff.o\
./mplayer/stream/realrtsp/sdpplin.o\
./mplayer/stream/realrtsp/xbuffer.o\
\
\
./mplayer/sub/subreader.o\
./mplayer/sub/find_sub.o\
./mplayer/sub/subassconvert.o\
./mplayer/sub/eosd.o\
./mplayer/sub/ass_mp.o\


INCLUDES += -I./live555/liveMedia/include/\
		-I./live555/groupsock/include/\
		-I./live555/UsageEnvironment/include\
		-I./live555/BasicUsageEnvironment/include/\
	
INCLUDES += -I$(TOP_DIR)/src/drv/ethernet
	
CFLAGS += -D__STDC_CONSTANT_MACROS					  
					  
  ifeq ($(WITH_TCPIP_PROTOCOL), Y)
    ifeq ($(USE_PB_FIFO), Y)
    	LIB_NAME                := $(KWARE_LIBDIR)/libMDemuxFifo.a
    else
    	LIB_NAME                := $(KWARE_LIBDIR)/libMDemux.a
    endif
  else
    LIB_NAME                := $(KWARE_LIBDIR)/libMDemuxLite.a
  endif

ifeq ($(WITH_TCPIP_PROTOCOL), Y)
 OBJS +=	./mplayer/stream/stream_udp.o\
			./mplayer/stream/udp.o\
				 
endif
LIB_MPNAME := $(LIB_NAME)
ifeq ($(EXT_MEM_POOL_ON), Y)
	OBJS += ext_mem_interface.o
endif

ifeq ($(ENABLE_OPEN_HTTPS), Y)
	OBJS += ./mplayer/ffmpeg/libavformat/ff_tls.o
endif
ifeq ($(ENABLE_LIVE555_STREAM), Y)

    OBJS +=  ./mplayer/libmpdemux/demux_rtp.o\
				  ./mplayer/libmpdemux/demux_rtp_codec.o\
					./mplayer/stream/stream_live555.o
 
   ifeq ($(WITH_TCPIP_PROTOCOL), Y)
     SUBDIRS = live555		
	 LIB_MPNAME += $(KWARE_LIBDIR)/libliveMedia.a $(KWARE_LIBDIR)/libgroupsock.a $(KWARE_LIBDIR)/libUsageEnvironment.a $(KWARE_LIBDIR)/libBasicUsageEnvironment.a
   endif


endif


ifeq ($(USE_UC_OS_OS), Y)
all:  clear_lib  $(OBJS)
	set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i; done
	$(SHOW_COMMAND)$(AR) crs  $(LIB_NAME)  $(OBJS)
	$(SHOW_COMMAND)$(RANLIB) $(LIB_NAME)
	echo -=Mips $(LIB_NAME) is built.=-
%.o: %.c
	$(SHOW_COMMAND)$(CC) $(CFLAGS) $(INCLUDES) -c $*.c -o $@

%.o: %.cpp
	$(SHOW_COMMAND)$(CXX) $(CFLAGS) $(INCLUDES) -c $*.cpp -o $@
else
all:  clear_lib  $(OBJS)
	set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i; done
	$(SHOW_COMMAND)$(AR) crs  $(LIB_NAME)  $(OBJS)
	$(SHOW_COMMAND)$(RANLIB) $(LIB_NAME)	
	$(RM) -fr  *.o
	for lib in $(LIB_MPNAME); do $(AR) x $$lib; done
	$(RM) -fr  $(LIB_MPNAME)
	$(SHOW_COMMAND)$(AR) crs  $(LIB_NAME)  *.o
	$(RM) -fr  *.o
	$(SHOW_COMMAND)$(RANLIB) $(LIB_NAME)
	echo -=Mips $(LIB_NAME) is built.=-
%.o: %.c
	$(SHOW_COMMAND)$(CC) $(CFLAGS) $(INCLUDES) -c $*.c -o $@

%.o: %.cpp
	$(SHOW_COMMAND)$(CXX) $(CFLAGS) $(INCLUDES) -c $*.cpp -o $@
endif


clear_lib:

	$(SHOW_COMMAND)$(RM) -fr  $(LIB_NAME)
  


##############################
# Clean up.
##############################
clean:


	$(SHOW_COMMAND)$(RM) -fr  $(LIB_NAME)
	find ./ -name "*.o" | xargs rm -fr
	find ./ -name "*.bbg" | xargs rm -fr
	find ./ -name "*.da" | xargs rm -fr
	find ./ -name "*.info" | xargs rm -fr
	find ./ -name "*.gcov" | xargs rm -fr
	find ./ -name "*.d" | xargs rm -fr
	set -e; for i in $(SUBDIRS); do $(MAKE) clean -C $$i; done
