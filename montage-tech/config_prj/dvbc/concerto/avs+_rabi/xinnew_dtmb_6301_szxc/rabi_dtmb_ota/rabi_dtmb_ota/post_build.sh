LZMA=lzma
MV=mv

MKIMG=mkimg_mips
CP=cp
rm -rf ./binary

mv rabi_dtmb_ota.map ./release/
mv ./${1} ./release/
cd ./release/
ANCHOR=../../../../../../../other/anchor/bin/anchor.exe

mipsel-linux-objcopy -O binary ${1} rabi.bin
$ANCHOR -p rabi_dtmb_ota.bin rabi.bin
rm -f rabi.bin
mipsel-linux-objdump -d rabi_dtmb_ota.elf > rabi_dtmb_ota.s
$ANCHOR -i rabi_dtmb_ota.img rabi_dtmb_ota.bin 0x80008000 0x80008000

  cp rabi_dtmb_ota.bin rabi_dtmb_ota.tmp
	rm -f rabi_dtmb_ota.tmp.lzma
	lzma -z -k rabi_dtmb_ota.tmp
	$MKIMG rabi_dtmb_ota.tmp.lzma concerto_dtmb_ota_lzma_ch.img -g
	rm -f rabi_dtmb_ota.tmp.lzma

echo "post build process completes"