#!/bin/sh
source /root/ini.ini
#source /tmp/hour_st.txt
source /tmp/hour.txt
source /root/cam.ini
R=$(awk 'NR==1' /etc/config/ipcam)
DD=$(date +%Y-%m-%d)
#DD_vid=$(date +%m%d"20"%y"-"%H"_"%M"_"%S)
if [ ! -d /tmp/USB/CAM/$DD/MOTION/VIDEO ];then
	mkdir /tmp/USB/CAM/$DD
	mkdir /tmp/USB/CAM/$DD/MOTION
	mkdir /tmp/USB/CAM/$DD/MOTION/VIDEO
fi
#svobodnoe mesto na diske:
SIZE_AV=$(df | grep /tmp/USB | awk '{print $4}')
#razmer poslednego fajla:
SIZE_F=$(ls -1Atrl /tmp/USB/CAM/$DD/MOTION/VIDEO | tail -n3 | awk 'NR==1 {print $3}')
let "SIZE_FU=SIZE_F/1024"
let "PR=SIZE_FU*3"

while [ "$SIZE_AV" -lt "$PR" ]; do
	SIZE_AV=$(df | grep /tmp/USB | awk '{print $4}')
	SIZE_F=$(ls -1Atrl /tmp/USB/CAM/$DD/MOTION/VIDEO | tail -n3 | awk 'NR==1 {print $3}')
	let "SIZE_FU=SIZE_F/1024"
	let "PR=SIZE_FU*5"
#nahodim samuyu staruju papku:
	DIR_OLD=$(ls -1Atl /tmp/USB/CAM | tail -n1 | awk '{print $7}')
#esli pustaja - udaljaem
		if [ "$(ls -A /tmp/USB/CAM/$DIR_OLD/MOTION/VIDEO)" ];then
#nahodim i udalyaem v samoy staroy papke samiy stariy fayl:
			rm /tmp/USB/CAM/$DIR_OLD/MOTION/VIDEO/$(ls -1Atl /tmp/USB/CAM/$DIR_OLD/MOTION/VIDEO | tail -n1 | awk '{print $7}')
				else
				rm -r /tmp/USB/CAM/$DIR_OLD
		fi
done
# openRTSP -4 -b 200000 -d $L $R>/tmp/USB/CAM/$DD/MOTION/VIDEO/$1.mp4
#while [ ! -e $(pgrep openRTSP) ]; do                                   
#sleep 1                                                          
#done
DD_vid=$(date +%m%d"20"%y"-"%H"_"%M"_"%S)
if [ $flg_day -eq 1 ];then
openRTSP -V -4 -f 20 -d $dlit_rol_dv_day_st $RTSP>/tmp/USB/CAM/$DD/MOTION/VIDEO/$DD_vid.mp4
else
openRTSP -V -4 -f 20 -d $dlit_rol_dv_night_st $RTSP>/tmp/USB/CAM/$DD/MOTION/VIDEO/$DD_vid.mp4
fi

