#TOPDIR = .$(shell pwd | sed 's/\(.*\)\/montage-tech\(.*\)/\2/g' | sed 's:/[^/]*:/..:g')
TOPDIR=../../..
-include $(TOPDIR)/common.mk

OBJS := mplayer_sdk.o 

all:  $(OBJS)

%.o: %.c
	$(SHOW_COMMAND)$(CC) $(CFLAGS) $(INCLUDES) -c $*.c -o $@

##############################
# Clean up.
##############################
clean:
	$(SHOW_COMMAND)$(RM) -fr $(OBJS)

