#!/bin/sh
F=$(ls /dev/sd?1)
if [ $F ];then
mount -t vfat -o codepage=866,iocharset=utf8 $F /tmp/USB/CAM
fi