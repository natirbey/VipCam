#!/bin/sh
source /root/ini.ini
#source /tmp/hour_st.txt
source /tmp/hour.txt
source /root/cam.ini
DD=$(date +%Y-%m-%d)
#DD_vid=$(date +%m%d"20"%y"-"%H"_"%M"_"%S)
if [ ! -d /tmp/USB/CAM/$DD/TIME/VIDEO ];then
	mkdir /tmp/USB/CAM/$DD
	mkdir /tmp/USB/CAM/$DD/TIME
	mkdir /tmp/USB/CAM/$DD/TIME/VIDEO
fi
#svobodnoe mesto na diske:
SIZE_AV=$(df | grep /tmp/USB | awk '{print $4}')
#razmer poslednego fajla:
SIZE_F=$(ls -1Atrl /tmp/USB/CAM/$DD/TIME/VIDEO | tail -n3 | awk 'NR==1 {print $3}')
let "SIZE_FU=SIZE_F/1024"
let "PR=SIZE_FU*3"

while [ "$SIZE_AV" -lt "$PR" ]; do
	SIZE_AV=$(df | grep /tmp/USB | awk '{print $4}')
	SIZE_F=$(ls -1Atrl /tmp/USB/CAM/$DD/TIME/VIDEO | tail -n3 | awk 'NR==1 {print $3}')
	let "SIZE_FU=SIZE_F/1024"
	let "PR=SIZE_FU*5"
#nahodim samuyu staruju papku:
	DIR_OLD=$(ls -1Atl /tmp/USB/CAM | tail -n1 | awk '{print $7}')
#esli pustaja - udaljaem
	if [ "$(ls -A /tmp/USB/CAM/$DIR_OLD/TIME/VIDEO)" ];then
#nahodim i udalyaem v samoy staroy papke samiy stariy fayl:
		rm /tmp/USB/CAM/$DIR_OLD/TIME/VIDEO/$(ls -1Atl /tmp/USB/CAM/$DIR_OLD/TIME/VIDEO | tail -n1 | awk '{print $7}')
			else
			rm -r /tmp/USB/CAM/$DIR_OLD
	fi 
done
R=$(awk 'NR==1' /etc/config/ipcam)
#openRTSP -4 -b 200000 -d $L -p $P1 $R>/tmp/Video/CAM/$DD/TIME/VIDEO/$1.mp4
#openRTSP -V -4 -f 4 -d $L $R>/tmp/Video/CAM/$DD/TIME/VIDEO/$DD_vid.mp4
while [ ! -e $(pgrep openRTSP) ]; do                                   
sleep 1                                                          
done
DD_vid=$(date +%m%d"20"%y"-"%H"_"%M"_"%S)
if [ $flg_day -eq 1 ];then  
openRTSP -V -4 -f 20 -d $prod_per_day_st $RTSP>/tmp/USB/CAM/$DD/TIME/VIDEO/$DD_vid.mp4
else
openRTSP -V -4 -f 20 -d $prod_per_night_st $RTSP>/tmp/USB/CAM/$DD/TIME/VIDEO/$DD_vid.mp4
fi

