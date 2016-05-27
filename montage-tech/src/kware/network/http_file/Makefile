##----------------------------------------------------------------------------
# Created with genmake.pl v1.1 on Tue Sep  7 11:11:28 2004
# genmake.pl home: http://muquit.com/muquit/software/
# Copryright: GNU GPL (http://www.gnu.org/copyleft/gpl.html)
##----------------------------------------------------------------------------
##############################
# MUST define src root dir!!!
##############################


#TOPDIR = .$(shell pwd | sed 's/\(.*\)\/montage-tech\(.*\)/\2/g' | sed 's:/[^/]*:/..:g')
TOPDIR=../../../..
-include $(TOPDIR)/common.mk

INC +=  -I$(TOPDIR)/inc/kware/network/HTTPC  \
             -I$(TOPDIR)/src/kware/network/HTTPC/src \
                      -I$(TOPDIR)/inc/kware/network \
                          -I$(TOPDIR)/inc/kware/network/http_file


ifeq ($(USE_UC_OS_OS), Y)
LIB_NAME = $(KWARE_LIBDIR)/libDownloadFile.a
else
LIB_NAME = $(KWARE_LIBDIR)/libhttpc.a
endif

INCLUDES+= $(INC)

SRCS = http_download.cpp   download_api.cpp  httpc.c http_download_mini.cpp download_manager.cpp md5.c

OBJS = http_download.o download_api.o httpc.o http_download_mini.o download_manager.o md5.o



%.o : %.c
	$(SHOW_COMMAND)${CC} -c ${CFLAGS} ${INC} $< -o $@

%.o : %.cpp
	$(SHOW_COMMAND)${CXX} -c ${CFLAGS} ${INC} $< -o $@


all: clear_lib  $(OBJS)

	$(AR) cru $(LIB_NAME) $(OBJS)
	$(RANLIB) $(LIB_NAME) 
	@echo -=Mips $(LIB_NAME) is built.=-

clear_lib:
	$(SHOW_COMMAND)$(RM) -fr $(LIB_NAME)


clean:
	$(RM)  -fr *.o  $(LIB_NAME)  *.d

