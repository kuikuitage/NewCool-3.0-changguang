#TOPDIR = .$(shell pwd | sed 's/\(.*\)\/montage-tech\(.*\)/\2/g' | sed 's:/[^/]*:/..:g')
TOPDIR=../../../../..
-include $(TOPDIR)/common.mk

INCLUDES = -Iinclude -I../groupsock/include
##### Change the following for your environment: 
COMPILE_OPTS =		$(INCLUDES) -I.  -O2 -DSOCKLEN_T=socklen_t -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64
C =			c
C_COMPILER =		$(CC)
C_FLAGS =		$(COMPILE_OPTS)
C_FLAGS += $(CFLAGS)
CPP =			cpp
CPLUSPLUS_COMPILER =	$(CXX)
CPLUSPLUS_FLAGS =	$(COMPILE_OPTS) -Wall -DBSD=1
CPLUSPLUS_FLAGS += $(CFLAGS)
OBJ =			o
LINK =			$(CXX) -o
LINK_OPTS =		-L.
CONSOLE_LINK_OPTS =	$(LINK_OPTS)
LIBRARY_LINK =		$(AR) cr 
LIBRARY_LINK_OPTS =	
LIB_SUFFIX =			a
LIBS_FOR_CONSOLE_APPLICATION =
LIBS_FOR_GUI_APPLICATION =
EXE =
##### End of variables to change

USAGE_ENVIRONMENT_LIB = libUsageEnvironment.$(LIB_SUFFIX)
ALL = $(USAGE_ENVIRONMENT_LIB)
#all:	$(ALL)

OBJS = UsageEnvironment.$(OBJ) HashTable.$(OBJ) strDup.$(OBJ)

#$(USAGE_ENVIRONMENT_LIB): $(OBJS)
#	$(LIBRARY_LINK)$@ $(LIBRARY_LINK_OPTS) $(OBJS)

LIB_NAME                := $(KWARE_LIBDIR)/libUsageEnvironment.a

all:  clear_lib  $(OBJS)
	$(SHOW_COMMAND)$(AR) crs  $(LIB_NAME)  $(OBJS)
	echo "start ranlib"
	$(SHOW_COMMAND)$(RANLIB) $(LIB_NAME)
	@echo -=Mips libUsageEnvironment.a is built.=-
	
.$(C).$(OBJ):
	$(C_COMPILER) -c $(C_FLAGS) $<       

.$(CPP).$(OBJ):
	$(CPLUSPLUS_COMPILER) -c $(CPLUSPLUS_FLAGS) $<

UsageEnvironment.$(CPP):	include/UsageEnvironment.hh
include/UsageEnvironment.hh:	include/UsageEnvironment_version.hh include/Boolean.hh include/strDup.hh
HashTable.$(CPP):		include/HashTable.hh
include/HashTable.hh:		include/Boolean.hh
strDup.$(CPP):			include/strDup.hh

clear_lib:
	$(SHOW_COMMAND)$(RM) -fr $(LIB_NAME)
	
clean:
	-rm -rf *.$(OBJ) $(ALL) core *.core *~ include/*~  *.d $(LIB_NAME)

##### Any additional, platform-specific rules come here:
