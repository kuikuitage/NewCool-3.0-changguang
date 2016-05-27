#-include ../../../../branch_src_sharedMemory_integration/Tools/MConfig/.config
#include ../../../../include/MakeConfig

#TOPDIR = .$(shell pwd | sed 's/\(.*\)\/montage-tech\(.*\)/\2/g' | sed 's:/[^/]*:/..:g')

TOPDIR=../../../../..
-include $(TOPDIR)/common.mk
#
# neon source directory Makefile
# 
# Use the NEON_NORMAL_BUILD or NEON_LIBTOOL_BUILD autoconf 
# macros to set up this Makefile correctly.
#



ifeq ($(USE_UC_OS_OS), Y)

   OBJS := ne_alloc.o  ne_auth.o ne_compress.o  ne_dates.o  ne_md5.o  ne_openssl.o  ne_redirect.o \
           ne_request.o  ne_session.o  ne_socket.o  ne_stats.o  ne_string.o  ne_uri.o ne_utils.o

   LIB_NAME                := $(KWARE_LIBDIR)/libneon.a

all:  clear_lib  $(OBJS)

	$(SHOW_COMMAND)$(AR) crs  $(LIB_NAME)  $(OBJS)
	echo "start ranlib"
	$(SHOW_COMMAND)$(RANLIB) $(LIB_NAME)
	@echo -=Mips libneon.a  is built.=-

%.o: %.c
	$(SHOW_COMMAND)$(CC) $(CFLAGS) $(INCLUDES) -c $*.c -o $@

clear_lib:
	$(SHOW_COMMAND)$(RM) -fr $(LIB_NAME)


##############################
# Clean up.
##############################

clean:
	$(SHOW_COMMAND)$(RM) -fr $(OBJS)
	$(SHOW_COMMAND)$(RM)  -fr $(LIB_NAME)
	$(SHOW_COMMAND)$(RM)  -fr *.d




else
#the following is only for linux


SHELL = /bin/sh

# Installation paths
prefix = /usr/local
exec_prefix = ${prefix}
libdir = ${exec_prefix}/lib

# Build paths

top_builddir = ../
top_srcdir = ..

CC = $(CROSS_TOOLCHAIN_PATH)/mipsel-linux-gcc
CXX = $(CROSS_TOOLCHAIN_PATH)/mipsel-linux-g++
AR = $(CROSS_TOOLCHAIN_PATH)/mipsel-linux-ar
RANLIB  = $(CROSS_TOOLCHAIN_PATH)/mipsel-linux-ranlib


# Toolchain settings.
#CC =$(MYCC) 
#AR =$(MYAR)
#RANLIB = $(MYRANLIB)
LIBTOOL = $(SHELL) $(top_builddir)/libtool

# Flags
CPPFLAGS = -DHAVE_CONFIG_H  -D_LARGEFILE64_SOURCE -DNE_LFS  -I${top_builddir} $(INC)  -I../../openssl-0.9.8/include

ifeq ($(EXCLUDE_OPENSSL), YES)
CFLAGS += -g -Wall $(DEFINES) -D__LINUX__  -mhard-float -fshort-wchar
else
CFLAGS +=  -g -Wall $(DEFINES)  -D__LINUX__  -mhard-float -fshort-wchar
endif

LDFLAGS = 
NEON_LINK_FLAGS = -version-info 27:2:0 -export-symbols-regex '^ne_[^_]'
# Note: don't substitute  in here; during a bundled
# build of this directory,  may include -lneon.
#LIBS =  -lz -lgssapi_krb5 -lkrb5 -lk5crypto -lcom_err 

COMPILE = $(CC) $(CPPFLAGS) $(CFLAGS)
LINK = $(CC) -no-undefined $(LDFLAGS)

NEON_BASEOBJS = ne_request.lo ne_session.lo 	    \
	ne_string.lo 		    \
	ne_uri.lo ne_dates.lo ne_alloc.lo  \
	ne_utils.lo    \
	ne_socket.lo \
	ne_redirect.lo ne_compress.lo ne_stats.lo \
	ne_auth.lo ne_md5.lo

NEON_DAVOBJS = $(NEON_BASEOBJS) \
	ne_207.lo ne_xml.lo \
	ne_props.lo ne_locks.lo \
	ne_xmlreq.lo

ifeq ($(EXCLUDE_OPENSSL), YES)
OBJECTS =  $(NEON_BASEOBJS)  ne_stubssl.lo
else
OBJECTS =  $(NEON_BASEOBJS)  ne_openssl.lo
endif

.SUFFIXES:
.SUFFIXES: .c .lo .o

NEON_TARGET = libneon.a

# Thanks to gettext for this neat trick.
all: all-yes

all-yes: $(NEON_TARGET)
all-no:
	@echo "Bundled neon build not being used."

.c.lo:
	$(COMPILE) -c $< -o $@
.c.o:
	$(COMPILE) -c $< -o $@

libneon.la: $(OBJECTS)
#	$(LINK) -rpath $(libdir) $(NEON_LINK_FLAGS) -o $@ $(OBJECTS) $(LIBS)

libneon.a: $(OBJECTS)
	@rm -f $@
	$(AR) cru $@ $(OBJECTS)
	$(RANLIB) $@
	cp $@   $(KWARE_LIBDIR)

clean:
	rm -f $(NEON_TARGET) *.o *.lo *.bbg *.bb *.gc*
	rm -rf .libs

c++.c:
	find . -name ne_\*.h -print | sed 's/.*/#include "&"/;/ne_priv/d' > $@
	echo "int main(void) {}" >> $@

check-c++: c++.c
	c++ -I. c++.c

check-incl:
	@for f in ne_*.h; do \
	 echo Checking $$f...; \
	 echo "#include \"$$f\"" > checkincl.c; \
	 $(COMPILE) -c checkincl.c -o checkincl.o || exit 1; done

neonreq = ne_request.h ne_session.h ne_utils.h ne_string.h ne_socket.h \
	ne_alloc.h $(top_builddir)/config.h ne_internal.h ne_uri.h ne_stats.h

ne_stats.lo: ne_stats.c ne_stats.h

ne_request.lo: ne_request.c $(neonreq) ne_private.h ne_uri.h

ne_session.lo: ne_session.c ne_session.h ne_alloc.h \
	ne_utils.h ne_private.h ne_uri.h $(top_builddir)/config.h

ne_openssl.lo: ne_openssl.c ne_session.h ne_ssl.h ne_privssl.h \
	ne_private.h $(top_builddir)/config.h

ne_gnutls.lo: ne_gnutls.c ne_session.h ne_ssl.h ne_privssl.h \
	ne_private.h $(top_builddir)/config.h

ne_socket.lo: ne_socket.c ne_socket.h $(top_builddir)/config.h \
	ne_privssl.h ne_string.h

ne_auth.lo: ne_auth.c ne_auth.h $(neonreq) ne_private.h \
	ne_dates.h ne_md5.h ne_uri.h 

ne_basic.lo: ne_basic.c ne_basic.h $(neonreq)

ne_utils.lo: ne_utils.c $(top_builddir)/config.h \
	ne_utils.h ne_dates.h

ne_xml.lo: ne_xml.c ne_xml.h ne_string.h ne_utils.h \
	$(top_builddir)/config.h

ne_207.lo: ne_207.c ne_207.h ne_xml.h ne_uri.h \
	$(top_builddir)/config.h ne_utils.h ne_internal.h

ne_string.lo: ne_string.c ne_string.h ne_alloc.h \
	$(top_builddir)/config.h

ne_alloc.lo: ne_alloc.c ne_alloc.h $(top_builddir)/config.h

ne_dates.lo: ne_dates.c ne_dates.h $(top_builddir)/config.h

ne_uri.lo: ne_uri.c ne_uri.h ne_utils.h ne_string.h ne_alloc.h \
	$(top_builddir)/config.h

ne_md5.lo: ne_md5.c ne_md5.h $(top_builddir)/config.h

ne_props.lo: ne_props.c $(top_builddir)/config.h \
	ne_props.h ne_207.h ne_xml.h $(neonreq)

ne_locks.lo: ne_locks.c $(neonreq) ne_locks.h ne_207.h ne_xml.h

ne_redirect.lo: ne_redirect.c $(neonreq) ne_redirect.h \
	ne_uri.h ne_private.h

ne_compress.lo: ne_compress.c $(neonreq) ne_compress.h

ne_acl.lo: ne_acl.c ne_acl.h $(neonreq)

ne_xmlreq.lo: ne_xmlreq.c ne_xmlreq.h ne_xml.h $(neonreq)

ne_stubssl.lo: ne_stubssl.c $(neonreq)

ne_i18n.lo: ne_i18n.c ne_i18n.h $(top_builddir)/config.h
endif
