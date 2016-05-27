LZMA=lzma
MV=mv

MKIMG=mkimg_mips
rm -rf ./binary

mv rabi_dtmb.map ./release/
mv ./${1} ./release/
cd ./release/


ANCHOR=../../../../../../../../config_prj/other/anchor/bin/anchor.exe
ANCHOR2=../../../../../../../../../config_prj/other/anchor/bin/anchor.exe
ANCHOR3=../../../../../../../../../../config_prj/other/anchor/bin/anchor.exe


mipsel-linux-objcopy -O binary ${1} rabi_dtmb.bin
$ANCHOR -p concerto.bin rabi_dtmb.bin
rm -f rabi_dtmb.bin

#mipsel-linux-objdump -d rabi_dtmb.elf > concerto.s
$ANCHOR -i concerto.img concerto.bin 0x80100000 0x80100000

      cp concerto.bin concerto.tmp
	rm -f concerto.tmp.lzma
	lzma -z -k concerto.tmp
	#rm -f concerto.tmp
	$MKIMG concerto.tmp.lzma concerto_lzma.img -g
	rm -f concerto.tmp.lzma

$MV -f concerto.bin ./binary/
$MV -f concerto_lzma.img ./binary/
cd binary/ota_binary
$ANCHOR3 -b ./ota_flash.cfg ./../bootloader.bin
cd ../
echo  "anchor maincode.bin ............"
$ANCHOR2 -b ./maincode.cfg ./maincode.bin
echo  "anchor flash.bin ............"
$ANCHOR2 -b ./flash.cfg ./flash.tmp.bin
$ANCHOR2 -p flash.bin flash.tmp.bin
rm -f flash.tmp.bin
cp ./flash.bin ../flash.bin
cd ../

echo  "backup rabi_dtmb.elf & rabi_dtmb.map ............"
cp ./rabi_dtmb.elf ./binary
cp ./rabi_dtmb.map ./binary

echo "post build process completes"



