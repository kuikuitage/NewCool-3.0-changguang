#TOPDIR = .$(shell pwd | sed 's/\(.*\)\/montage-tech\(.*\)/\2/g' | sed 's:/[^/]*:/..:g')

#-include $(TOPDIR)/common.mk
-include ../../common.mk

		SUBDIRS =    demux_mp  file_playback  network/HTTPC  network/openssl-0.9.8  network/http_file


all:
	echo "current drv branch:[MainBranchDev_Drv:$(CUR_BRANCH)]"
	set -e; for i in $(SUBDIRS); do $(MAKE) -C $$i; done

clean:
	set -e; for i in $(SUBDIRS); do $(MAKE) clean -C $$i; done

cleanall:
	set -e; for i in $(SUBDIRS); do $(MAKE) cleanall -C $$i; done



