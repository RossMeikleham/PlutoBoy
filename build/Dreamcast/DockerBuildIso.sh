make all
cp /opt/mksdiso/src/makeip/ip.txt /opt/mksdiso/src/makeip/IP.TMPL .
makeip ip.txt IP.BIN
mkdir -p iso
scramble PlutoboyDC.bin iso/1ST_READ.BIN
genisoimage -V MY_GAME -G IP.BIN -joliet -rock -l -o PlutoboyDC.iso iso
cp PlutoboyDC.iso PlutoboyDC.elf /mnt
