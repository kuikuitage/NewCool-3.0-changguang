###############################################################################
# Copyright (c) 2008 Montage Tech - All Rights Reserved
# Owner: Yongmin Du <yongmin.du@montage-tech.com>
#        Yiping  Xu <peacer.xu@motage-tech.com>
#		 Michael Qiu <michael.qiu@montage-tech.com>
###############################################################################


###############################################################################
#
#
#                             Top Directory
#
#
###############################################################################

TOP_DIR = .$(shell pwd | sed 's/\(.*\)\/montage-tech\(.*\)/\2/g' | sed 's:/[^/]*:/..:g')
#TOP_DIR = $(shell pwd | sed 's/\(.*\)\/montage-tech\(.*\)/\1/g')/montage-tech/
CUR_BRANCH = $(shell hg branch)

-include $(TOP_DIR)/makeConfig



###############################################################################
#
#
#    variable about  Tool Chain
#
#
###############################################################################
ifeq ($(USE_LINUX_OS), Y)
CROSS_TOOLCHAIN_PATH = /home/public/mt-toolchain/usr/bin
#CROSS = /home/public/mt-toolchain/usr/bin/mipsel-linux-
CROSS  = /usr/local/codesourcery/mips-4.3/bin/mips-linux-gnu-
ifeq ($(LIBC),glibc)
export CC  = $(CROSS)gcc -EL
export CXX = $(CROSS)g++ -EL
export AR := $(CROSS)ar
export AS := $(CROSS)as
export STRIP := $(CROSS)strip
export CPP = $(CC) -EL
export OBJCOPY  = $(CROSS)objcopy
export OBJDUMP  = $(CROSS)objdump
export NM       = $(CROSS)nm
export LD       = $(CROSS)ld -EL
export RANLIB    = $(CROSS)ranlib
export LIB_DIR  = $(PUBLIC_DIR)/lib/glibc
else
export CC  = $(CROSS)gcc -muclibc -EL
export CXX = $(CROSS)g++ -muclibc -EL
export AR := $(CROSS)ar
export AS := $(CROSS)as
export STRIP := $(CROSS)strip
export CPP = $(CC) -muclibc -EL
export OBJCOPY  = $(CROSS)objcopy
export OBJDUMP  = $(CROSS)objdump
export NM       = $(CROSS)nm
export LD       = $(CROSS)ld -muclibc -EL
export RANLIB    = $(CROSS)ranlib
export LIB_DIR  = $(PUBLIC_DIR)/lib/uclibc
endif
else

ifeq ($(shell uname),Linux)
#default value, mips4k value
CROSS_TOOLCHAIN_PATH = /home/public/mt-toolchain/usr/bin

ifeq ($(USE_TOOLCHAIN_MIPS24k), Y)
CROSS_TOOLCHAIN_PATH = /home/public/mt-toolchain_24k/usr/bin
endif

CROSS = $(CROSS_TOOLCHAIN_PATH)/mipsel-linux-
else
CROSS = mipsel-linux-
endif

AS    = $(CROSS)gcc
CC    = $(CROSS)gcc
CXX    = $(CROSS)g++
LD    = $(CROSS)ld
AR    = $(CROSS)ar
RANLIB    = $(CROSS)ranlib
OD    = $(CROSS)objdump
OC    = $(CROSS)objcopy
GS    = $(CROSS)gasp
NM    = $(CROSS)nm
RD    = $(CROSS)readelf
STRIP = $(CROSS)strip
SIZE  = $(CROSS)size
LIBTOOL = $(CROSS)libtool
endif
CAT   = cat
LS    = ls
XARGS = xargs
RM    = rm
CP    = cp
MKDIR = mkdir
ECHO  = echo
SED   = sed
FIND  = find
MV    = mv
MAKE= make



###############################################################################
#
#
#
#
#                                   define  MACRO
#
#
#
#
#
#
#############################################################################

MACRODEF =  \
	-D_LARGEFILE_SOURCE \
	-D_FILE_OFFSET_BITS=64 \
	-D_LARGEFILE64_SOURCE \
        -Wall \



ifeq ($(USE_LINUX_OS), Y)
  MACRODEF  +=  -D__LINUX__
endif

ifeq ($(USE_CHIP_WARRIORS), Y)
  MACRODEF  +=  -DCHIP_WARRIORS
endif

ifeq ($(USE_CHIP_SONATA), Y)
  MACRODEF  +=  -DCHIP_SONATA
endif

ifeq ($(USE_CHIP_CONCERTO), Y)
  MACRODEF  +=  -DCHIP_CONCERTO
endif

ifeq ($(CUR_BRANCH), Flounder_SDK_2.0)
  MACRODEF  +=  -DSDK_2_0
  HG_VER =  $(shell hg heads Flounder_SDK_2.0 | grep 'changeset')
  WITH_OTT_DATA = N
endif

ifeq ($(CUR_BRANCH), warriors_3.0)
  MACRODEF  +=  -DWARRIORS_3_0
  HG_VER =  $(shell hg heads warriors_3.0 | grep 'changeset')
  WITH_OTT_DATA = Y
endif

ifeq ($(CUR_BRANCH), default)
  MACRODEF  +=  -DDRV_DEFAULT
  HG_VER =  $(shell hg heads default | grep 'changeset')
  WITH_OTT_DATA = Y
endif

ifeq ($(CUR_BRANCH), sonata_sdk_4.0)
  MACRODEF  +=  -DSDK_4_0
  HG_VER =  $(shell hg heads sonata_sdk_4.0 | grep 'changeset')
  WITH_OTT_DATA = N
endif

ifeq ($(CUR_BRANCH), sonata_net)
  MACRODEF  +=  -DSONATA_NET
  HG_VER =  $(shell hg heads sonata_net | grep 'changeset')
  WITH_OTT_DATA = Y
endif

ifeq ($(CUR_BRANCH), concerto_ucos)
  MACRODEF  +=  -DCONCERTO_UCOS
  HG_VER =  $(shell hg heads concerto_ucos | grep 'changeset')
  WITH_OTT_DATA = Y
endif




  HG_PARENTS = $(shell hg parents | grep 'changeset')


ifeq ($(WITH_OTT_DATA), Y)
  MACRODEF  +=  -DWITH_OTT_DATA
endif




ifeq ($(BUILD_LINUX_KERNEL), Y)
  MACRODEF  +=  -DLINUX_KERNEL
endif


ifeq ($(TEST_SPEED_OPEN), Y)
  MACRODEF  +=  -DTEST_SPEED_OPEN
endif


ifeq ($(WITH_TCPIP_PROTOCOL), N)
      
   ENABLE_LIVE555_STREAM  = N
   ENABLE_DEMUX_RTSP      = N
   ENABLE_DEMUX_HTTP      = N
   ENABLE_DEMUX_RTMP      = N
   ENABLE_DEMUX_MMS       = N
else
   MACRODEF += -DWITH_TCPIP_PROTOCOL

endif



ifeq ($(ENABLE_LIVE555_STREAM), Y)
  MACRODEF += -DSUPPORT_LIVE555_STREAM 
endif

ifeq ($(ENABLE_DEMUX_RTSP), Y)
  MACRODEF += -DENABLE_DEMUX_RTSP   
endif

ifeq ($(ENABLE_DEMUX_HTTP), Y)
  MACRODEF  +=  -DENABLE_DEMUX_HTTP
endif

ifeq ($(ENABLE_DEMUX_RTMP), Y)
  MACRODEF  +=  -DENABLE_DEMUX_RTMP
endif

ifeq ($(ENABLE_DEMUX_MMS), Y)
  MACRODEF  +=  -DENABLE_DEMUX_MMS
endif


ifeq ($(SUPPORT_ALL_DATA_PROVIDER), N)
   ifeq ($(SUPPORT_INTERNAL_OTT_DP), Y)
      MACRODEF  +=  -DENABLE_INTERNAL_OTT_DP
   endif
   ifeq ($(SUPPORT_NETMEDIA_DP), Y)
      MACRODEF  +=  -DENABLE_NETMEDIA_DP
   endif

   ifeq ($(SUPPORT_FLICKR_DP), Y)
      MACRODEF  +=  -DENABLE_FLICKR_DP
   endif
   ifeq ($(SUPPORT_UTIL_DP), Y)
      MACRODEF  +=  -DENABLE_UTIL_DP
   endif
   ifeq ($(SUPPORT_YOUTUBE_DP), Y)
      MACRODEF  +=  -DENABLE_YOUTUBE_DP
   endif
   ifeq ($(SUPPORT_REDX_DP), Y)
      MACRODEF  +=  -DENABLE_REDX_DP
   endif
   ifeq ($(SUPPORT_YOUX_DP), Y)
      MACRODEF  +=  -DENABLE_YOUX_DP
   endif
   ifeq ($(SUPPORT_VIMEO_DP), Y)
      MACRODEF  +=  -DENABLE_VIMEO_DP
   endif
   ifeq ($(SUPPORT_ONLINEMOVIES_DP), Y)
      MACRODEF  +=  -DENABLE_ONLINEMOVIES_DP
   endif
   ifeq ($(SUPPORT_ONLAINFILM_DP), Y)
      MACRODEF  +=  -DENABLE_ONLAINFILM_DP
   endif
   ifeq ($(SUPPORT_DAILYMOTION_DP), Y)
      MACRODEF  +=  -DENABLE_DAILYMOTION_DP
   endif
   ifeq ($(SUPPORT_ALJAZEER_DP), Y)
      MACRODEF  +=  -DENABLE_ALJAZEER_DP
   endif
   ifeq ($(SUPPORT_VEOH_DP), Y)
      MACRODEF  +=  -DENABLE_VEOH_DP
   endif
   ifeq ($(SUPPORT_YAHOOVIDEO_DP), Y)
      MACRODEF  +=  -DENABLE_YAHOOVIDEO_DP
   endif
   ifeq ($(SUPPORT_ADDANIMATE_DP), Y)
      MACRODEF  +=  -DENABLE_ADDANIMATE_DP
   endif
   ifeq ($(SUPPORT_KONTRTUBE_DP), Y)
      MACRODEF  +=  -DENABLE_KONTRTUBE_DP
   endif
   ifeq ($(SUPPORT_SPORTSVIDEO_DP), Y)
      MACRODEF  +=  -DENABLE_SPORTSVIDEO_DP
   endif
   ifeq ($(SUPPORT_ARABICFILM_DP), Y)
      MACRODEF  +=  -DENABLE_ARABICFILM_DP
   endif
   ifeq ($(SUPPORT_MBCSHAHID_DP), Y)
      MACRODEF  +=  -DENABLE_MBCSHAHID_DP
   endif
   ifeq ($(SUPPORT_YOUKU_DP), Y)
      MACRODEF  +=  -DENABLE_YOUKU_DP
   endif
   ifeq ($(SUPPORT_DUBAIMEDIA_DP), Y)
      MACRODEF  +=  -DENABLE_DUBAIMEDIA_DP
   endif

   ifeq ($(SUPPORT_NMYOUPORN_DP), Y)
      MACRODEF  +=  -DENABLE_NMYOUPORN_DP
   endif
   ifeq ($(SUPPORT_NMYOUTUBE_DP), Y)
      MACRODEF  +=  -DENABLE_NMYOUTUBE_DP
   endif

   ifeq ($(SUPPORT_DONANG_DP), Y)
      MACRODEF  +=  -DENABLE_DONANG_DP
   endif

    ifeq ($(SUPPORT_WEBYOUTUBE_DP), Y)
      MACRODEF  +=  -DENABLE_WEBYOUTUBE_DP
   endif

    ifeq ($(SUPPORT_SELFIP_DP), Y)
      MACRODEF  +=  -DENABLE_SELFIP_DP
   endif

    ifeq ($(SUPPORT_BEFUCK_DP), Y)
      MACRODEF  +=  -DENABLE_BEFUCK_DP
   endif
   
   ifeq ($(SUPPORT_TUKIF_DP), Y)
      MACRODEF  +=  -DENABLE_TUKIF_DP
   endif
   
   ifeq ($(SUPPORT_ANYPORN_DP), Y)
      MACRODEF  +=  -DENABLE_ANYPORN_DP
   endif

   ifeq ($(SUPPORT_EPORNER_DP), Y)
      MACRODEF  +=  -DENABLE_EPORNER_DP
   endif

   ifeq ($(SUPPORT_BASKINO_DP), Y)
      MACRODEF  +=  -DENABLE_BASKINO_DP
   endif

   ifeq ($(SUPPORT_ZNDIPTV_DP), Y)
      MACRODEF  +=  -DENABLE_ZNDIPTV_DP
   endif

   ifeq ($(SUPPORT_BSWIPTV_DP), Y)
      MACRODEF  +=  -DENABLE_BSWIPTV_DP
   endif

   ifeq ($(SUPPORT_ZGWIPTV_DP), Y)
      MACRODEF  +=  -DENABLE_ZGWIPTV_DP
   endif


   ifeq ($(SUPPORT_DAQIVO_DP), Y)
      MACRODEF  +=  -DENABLE_DAQIVO_DP
   endif

   ifeq ($(SUPPORT_BSW_DP), Y)
      MACRODEF  +=  -DENABLE_BSW_DP
   endif

   ifeq ($(SUPPORT_SKY_DP), Y)
      MACRODEF  +=  -DENABLE_SKY_DP
   endif
   ifeq ($(SUPPORT_ZGWLIVE_DP), Y)
      MACRODEF  +=  -DENABLE_ZGWLIVE_DP
   endif
   ifeq ($(SUPPORT_ZNDLIVE_DP), Y)
      MACRODEF  +=  -DENABLE_ZNDLIVE_DP
   endif

else
      MACRODEF  +=  -DENABLE_ZNDIPTV_DP  -DENABLE_BSWIPTV_DP  -DENABLE_DAQIVOD_DP  -DENABLE_BSW_DP  -DENABLE_BSW_DP\
		 -DENABLE_DAQIVO_DP  \
                 -DENABLE_ZGWIPTV_DP  -DENABLE_SKY_DP  -DENABLE_ZGWLIVE_DP  -DENABLE_ZNDLIVE_DP  -DENABLE_REDX_DP\
                 -DENABLE_YOUX_DP  -DENABLE_FLICKR_DP  -DENABLE_YOUTUBE_DP  -DENABLE_VIMEO_DP  -DENABLE_ONLINEMOVIES_DP\
                 -DENABLE_ONLAINFILM_DP  -DENABLE_DAILYMOTION_DP  -DENABLE_ALJAZEER_DP  -DENABLE_VEOH_DP  -DENABLE_YAHOOVIDEO_DP\
                 -DENABLE_ADDANIMATE_DP  -DENABLE_KONTRTUBE_DP  -DENABLE_SPORTSVIDEO_DP  -DENABLE_ARABICFILM_DP  -DENABLE_MBCSHAHID_DP\
                 -DENABLE_YOUKU_DP  -DENABLE_DUBAIMEDIA_DP -DENABLE_NMYOUPORN_DP -DENABLE_NMYOUTUBE_DP  -DENABLE_DONANG_DP -DENABLE_WEBYOUTUBE_DP  -DENABLE_SELFIP_DP -DENABLE_ANYPORN_DP -DENABLE_TUKIF_DP\
		 -DENABLE_BEFUCK_DP  -DENABLE_EPORNER_DP -DENABLE_BASKINO_DP
endif




ifeq ($(ENABLE_EFENCE), Y)
  MACRODEF  +=  -DENABLE_EFENCE
endif

ifeq ($(USE_CHIP_SONATA), Y)
  MACRODEF  +=  -DCHIP_SONATA
endif

ifeq ($(ENABLE_OPEN_SSL), Y)
	MACRODEF  +=  -DENABLE_OPEN_SSL
endif

ifeq ($(ENABLE_OPEN_HTTPS), Y)
	MACRODEF  +=  -DENABLE_OPEN_HTTPS
endif

ifeq ($(DOWNLOAD_HTTP_NEW), Y)
	MACRODEF  +=  -DDOWNLOAD_HTTP_NEW
endif

MACRODEF +=   -DASSERT_ON \
                -DUSE_EXT_HEAP 
ifeq ($(USE_UC_OS_OS), Y)  ### just for ucos os 
  MACRODEF  +=  -DTINYXML_WITH_UFS
endif
ifeq ($(ENABLE_PRINT_ON), Y)
  MACRODEF  +=  -DPRINT_ON
endif

ifeq ($(USE_64M_CONFIG), Y)
  MACRODEF  +=  -DEXT_MEM_POOL
  EXT_MEM_POOL_ON = Y
endif

ifeq ($(ENABLE_USE_SCRIPT), Y)
  MACRODEF  +=  -DUSE_SCRIPT_FUNCTION
endif

ifeq ($(USE_PB_FIFO), Y)
  MACRODEF  +=  -DUSE_PB_FIFO
endif

###############################################################################
#
#
#
#
#
#                     define  include  path for compiler
#
#
#
#
#
#
#############################################################################



INC :=  \
	-I$(TOP_DIR)/inc \
	-I$(TOP_DIR)/inc/drv \
	-I$(TOP_DIR)/inc/hal \
	-I$(TOP_DIR)/inc/mdl \
	-I$(TOP_DIR)/inc/util \
        -I$(TOP_DIR)/inc/util/rw_reg \
	-I$(TOP_DIR)/inc/os \
	-I$(TOP_DIR)/inc/libra \
	-I$(TOP_DIR)/inc/mboot \
	-I$(TOP_DIR)/inc/drv/bus  \
	-I$(TOP_DIR)/inc/drv/drvbase  \
	-I$(TOP_DIR)/inc/firmware \
	-I$(TOP_DIR)/inc/hal/arch  \
	-I$(TOP_DIR)/inc/util/log  \
	-I$(TOP_DIR)/inc/kware/ts_seq \
	-I$(TOP_DIR)/src/os/ucos/inc  \
	-I$(TOP_DIR)/src/os/ucos/inc/warriors  \
 	-I$(TOP_DIR)/src/firmware/core/include \
 	-I$(TOP_DIR)/src/firmware/video/video_dec/warriors/common/inc \
 	-I$(TOP_DIR)/src/firmware/audio/audio_common/warriors/ \
 	-I$(TOP_DIR)/src/firmware/core/warriors/ \
	-I$(TOP_DIR)/src/kware \
	-I$(TOP_DIR)/src/kware/ufs/fat \
	-I$(TOP_DIR)/src/kware/demux_mp/mplayer \
	-I$(TOP_DIR)/src/kware/demux_mp/mplayer/ffmpeg \
	-I$(TOP_DIR)/src/kware/demux_mp/mplayer/libmpdemux \
	-I$(TOP_DIR)/src/kware/demux_mp/mplayer/stream \
	-I$(TOP_DIR)/src/kware/demux_mp/mplayer/libass \
	-I$(TOP_DIR)/src/kware/demux_mp/mplayer/sub  \
	-I$(TOP_DIR)/src/kware/demux_mp/mplayer/libvo  \
	-I$(TOP_DIR)/src/kware/demux_mp/mplayer/libmpcodes  \
	-I$(TOP_DIR)/src/kware/simple_script  \
	-I$(TOP_DIR)/inc/kware/file_playback \
	-I$(TOP_DIR)/inc/kware/demux_mp \
	-I$(TOP_DIR)/inc/kware/network/HTTPC \
	-I$(TOP_DIR)/inc/kware/network/http_file \
	-I$(TOP_DIR)/inc/kware/network/tcpip \
	-I$(TOP_DIR)/inc/kware/network/http \
	-I$(TOP_DIR)/inc/kware/network/ \
	-I$(TOP_DIR)/inc/kware \
	-I$(TOP_DIR)/inc/kware/subt_ttx \
	-I$(TOP_DIR)/inc/kware/pvr \
	-I$(TOP_DIR)/inc/kware/RSSClient/RSSParser/ \
	-I$(TOP_DIR)/inc/kware/RSSClient/tinyxml/ \
	-I$(TOP_DIR)/inc/kware/record/ \







ifeq ($(BUILD_LINUX_KERNEL), Y)

	INC +=  \
		-I$(TOP_DIR)/src/os/inc \
		-I$(TOP_DIR)/src/os/linux/inc \
		-I$(TOP_DIR)/src/drv \
		-I$(TOP_DIR)/src/ \
		-I$(TOP_DIR)/src/drv/drvbase \
		-I$(TOP_DIR)/src/drv/vdec \
		-I$(TOP_DIR)/src/drv/audio \
		-I$(TOP_DIR)/inc/kware/file_playback \
		-I$(LINUX_KERNEL_DIR)/include/inc/drv/drvbase \
		-I$(LINUX_KERNEL_DIR)/include  \
		-I$(LINUX_KERNEL_DIR)/drivers/char/vdec  \
		-I$(LINUX_KERNEL_DIR)/include/inc_for_user  \
		-I$(LINUX_KERNEL_DIR)/drivers/char/nim/dvbs/ds3000  \
		-I$(LINUX_KERNEL_DIR)/include/ \
 		\

endif






#####################################################################
#
#
#
#
#
#                CFLAG and AFLAG  for compiler
#
#
#
#
#
#####################################################################

ifeq ($(ENABLE_SHOW_COMMAND), Y)
	SHOW_COMMAND = 
else
 	SHOW_COMMAND = @
endif
 

OPTIMIZE_DEFINES = -O2

CFLAGS ?=  -EL -march=mips32r2 \
               -mxgot \
               -mlong-calls  \
               -mabi=32 \
               -mno-abicalls \
               -Wall \
               -Wno-pointer-sign  \
               -Wno-pointer-to-int-cast \
               -ffunction-sections  \
               $(INC) $(MACRODEF) $(OPTIMIZE_DEFINES) $(DEBUG_DEFINES)  \
             \


ifeq ($(USE_CHIP_CONCERTO), Y)
 CFLAGS += -mhard-float -fshort-wchar
endif
ifeq ($(ENABLE_DEBUG_OPTION), Y)
 CFLAGS += -g -MD
endif

CFALGS += -MD         

AFLAGS ?= $(INC) -EL -mips1 -c



#######################################################################
#
#
#                        option for linker
#
#
#######################################################################

LIBDIR = $(TOP_DIR)/lib/lib_warriors


#######################################################################
#
#
#                        option for linker to kware
#
#
#######################################################################
#default value, mips4k value
ifeq ($(USE_64M_CONFIG), Y)
KWARE_LIBDIR = $(TOP_DIR)/lib/lib_kware/mips4k/lib_64M
else
KWARE_LIBDIR = $(TOP_DIR)/lib/lib_kware/mips4k
endif

ifeq ($(USE_TOOLCHAIN_MIPS24k), Y)
ifeq ($(USE_64M_CONFIG), Y)
KWARE_LIBDIR = $(TOP_DIR)/lib/lib_kware/mips24k/lib_64M
else
KWARE_LIBDIR = $(TOP_DIR)/lib/lib_kware/mips24k
endif
endif



LIBS  := -lm  \
         -lstdc++ \
         -lrt   \
         -lpthread  \
 
ifeq ($(ENABLE_EFENCE), N)
  LIBS +=   -lc
endif



LIBS +=   \
          -ldrvAPI \
          -lmtos_lnx \
          -lfileplaybackSeq\
          -lmplayer_mp\
          -lTsSeq\
          -lUtil \
          -lufs\
          -lHal \
          -lnetplay\
           -lft\
            -leva_drv_w  -lstandby  \


ifeq ($(ENABLE_RSSCLIENT_DATA_PROVIDER), Y)
  LIBS +=   -lneon  -lssl -lcrypto -lhttpc 
endif



ifeq ($(ENABLE_DEMUX_RTSP), Y)
LIBS +=  -lBasicUsageEnvironment -lUsageEnvironment  -lliveMedia  -lgroupsock
endif



LIBS +=  -lLinuxTestFramework -lModules

ifeq ($(ENABLE_EFENCE), Y)
  LIBS +=   -lefence_wrs
endif

ifeq ($(ENABLE_USE_FREEIMAGE), Y)
  LIBS +=   -lfreeimage
endif

ifeq ($(ENABLE_RSSCLIENT_DATA_PROVIDER), Y)
  LIBS +=   -lRSSClient  -ltinyxml
endif


ifeq ($(ENABLE_USE_SCRIPT), Y)
  LIBS +=   -lSimpleScript
endif

LDFLAGS =  -L$(LIBDIR) -L$(KWARE_LIBDIR)
ifeq ($(USE_LINUX_OS), Y)
CFLAGS =  -Wall -O2 -MD -g -mips32r2 -march=mips32r2 -fno-builtin-cos -fno-builtin-sin $(INC) $(MACRODEF) $(OPTIMIZE_DEFINES) $(DEBUG_DEFINES)
endif
#####################################################################3
#
#
#  other varible
#
#####################################################################33












