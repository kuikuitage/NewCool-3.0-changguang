#TOPDIR = .$(shell pwd | sed 's/\(.*\)\/montage-tech\(.*\)/\2/g' | sed 's:/[^/]*:/..:g')
TOPDIR=../../../../..
-include $(TOPDIR)/common.mk

INCLUDES = -Iinclude -I../UsageEnvironment/include -I../groupsock/include
##### Change the following for your environment: 
COMPILE_OPTS =		$(INCLUDES) -I. -O2 -DSOCKLEN_T=socklen_t -D_LARGEFILE_SOURCE=1 -D_FILE_OFFSET_BITS=64
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

LIB = libBasicUsageEnvironment.$(LIB_SUFFIX)
ALL = $(LIB)
#all:	$(ALL)

OBJS = BasicUsageEnvironment0.$(OBJ) BasicUsageEnvironment.$(OBJ) \
	BasicTaskScheduler0.$(OBJ) BasicTaskScheduler.$(OBJ) \
	DelayQueue.$(OBJ) BasicHashTable.$(OBJ)


LIB_NAME                := $(KWARE_LIBDIR)/libBasicUsageEnvironment.a

all:  clear_lib  $(OBJS)
	$(SHOW_COMMAND)$(AR) crs  $(LIB_NAME)  $(OBJS)
	echo "start ranlib"
	$(SHOW_COMMAND)$(RANLIB) $(LIB_NAME)
	@echo -=Mips libBasicUsageEnvironment.a is built.=-
	
	
.$(C).$(OBJ):
	$(C_COMPILER) -c $(C_FLAGS) $<       

.$(CPP).$(OBJ):
	$(CPLUSPLUS_COMPILER) -c $(CPLUSPLUS_FLAGS) $<

BasicUsageEnvironment0.$(CPP):	include/BasicUsageEnvironment0.hh
include/BasicUsageEnvironment0.hh:	include/BasicUsageEnvironment_version.hh include/DelayQueue.hh
BasicUsageEnvironment.$(CPP):	include/BasicUsageEnvironment.hh
include/BasicUsageEnvironment.hh:	include/BasicUsageEnvironment0.hh
BasicTaskScheduler0.$(CPP):	include/BasicUsageEnvironment0.hh include/HandlerSet.hh
BasicTaskScheduler.$(CPP):	include/BasicUsageEnvironment.hh include/HandlerSet.hh
DelayQueue.$(CPP):		include/DelayQueue.hh
BasicHashTable.$(CPP):		include/BasicHashTable.hh

clear_lib:

	$(SHOW_COMMAND)$(RM) -fr $(LIB_NAME)
	
clean:
	-rm -rf *.$(OBJ) $(ALL) core *.core *~ include/*~  *.d $(LIB_NAME)

##### Any additional, platform-specific rules come here:
