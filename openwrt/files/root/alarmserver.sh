#!/bin/sh
while : 
do
source /root/cam.ini
alarm_server
B=$?
if [ $B -eq 2 ];then
./rtsp_client.sh
fi
sleep $chuvstv
done
