#TOPDIR = .$(shell pwd | sed 's/\(.*\)\/montage-tech\(.*\)/\2/g' | sed 's:/[^/]*:/..:g')
TOPDIR=../../..
-include $(TOPDIR)/common.mk

OBJS := file_playback_sequence.o  file_seq_misc.o file_seq_internal.o file_out_subtitle.o 

ifeq ($(USE_UC_OS_OS), Y)

  ifeq ($(WITH_TCPIP_PROTOCOL), Y)
  	ifeq ($(USE_PB_FIFO), Y)
    	LIB_NAME                := $(KWARE_LIBDIR)/libPlaybackSeqFifo.a
    else
    	LIB_NAME                := $(KWARE_LIBDIR)/libPlaybackSeq.a
    endif
  else
    LIB_NAME                := $(KWARE_LIBDIR)/libPlaybackSeqLite.a
  endif
else

LIB_NAME                := $(KWARE_LIBDIR)/libPlaybackSeq.a
endif

all:  gen_ver clear_lib  $(OBJS)
	$(SHOW_COMMAND)$(AR) crs  $(LIB_NAME)  $(OBJS)
	echo "start ranlib"
	$(SHOW_COMMAND)$(RANLIB) $(LIB_NAME)
	@echo -=Mips $(LIB_NAME) is built.=-

%.o: %.c
	$(SHOW_COMMAND)$(CC) $(CFLAGS) $(INCLUDES) -c $*.c -o $@

gen_ver:
	rm -fr .hg_ver.h
	echo "static char * p_hg_ver = \"$(HG_VER) and current parents $(HG_PARENTS)\";"  > .hg_ver.h

clear_lib:

	$(SHOW_COMMAND)$(RM) -fr $(LIB_NAME)
##############################
# Clean up.
##############################
clean:
	$(SHOW_COMMAND)$(RM) -fr $(OBJS)
	$(SHOW_COMMAND)$(RM)  -fr $(LIB_NAME)
	$(SHOW_COMMAND)$(RM)  -fr *.d
