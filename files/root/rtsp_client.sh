#!/bin/sh
source /root/ini_cloud.ini
source /tmp/hour.txt
source /root/ini.ini
source /root/cam.ini
hour=$(date +%H)
#if [ $rec_cl -eq 1 ] && [[ "`pgrep -f motion.sh | wc -l`" -eq "0" ]];then
if [ $rec_cl -eq 1 ] && [[ "`pgrep -f motion.sh | wc -l`" -eq "0" ]];then
	if [ $zap_dv_day -eq 1 ] && [ $flg_day -eq 1 ];then 
		/root/./motion.sh
	fi
	if [ $zap_dv_night -eq 1 ] && [ $flg_day -eq 0 ];then 
		/root/./motion.sh
        fi
fi
#if [ $rec_st -eq 1 ] && [[ "`pgrep -f motion_st.sh | wc -l`" -eq "0" ]];then
if [ $rec_st -eq 1 ] && [[ "`pgrep -f motion_st.sh | wc -l`" -eq "0" ]];then
        if [ $zap_dv_day_st -eq 1 ] && [ $flg_day -eq 1 ];then 
		/root/./motion_st.sh & 
        fi              		
        if [ $zap_dv_night_st -eq 1 ] && [ $flg_day -eq 0 ];then 
		/root/./motion_st.sh &
        fi
 fi

