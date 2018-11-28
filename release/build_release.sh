cd ..
make
cd release
/Applications/ffmpeg -i Tile.png -f rawvideo -pix_fmt rgb565 tile.raw
~/Documents/GitHub/odroid-go-firmware/tools/mkfw/mkfw OpenTyrian tile.raw 0 16 1048576 app ../build/OpenTyrian.bin
rm OpenTyrian.fw
mv firmware.fw OpenTyrian.fw
~/Documents/GitHub/odroid-go-firmware-20181001/tools/mkfw/mkfw OpenTyrian tile.raw 0 16 1048576 app ../build/OpenTyrian.bin
rm OpenTyrian-20181001.fw
mv firmware.fw OpenTyrian-20181001.fw
