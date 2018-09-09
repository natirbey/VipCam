#!/bin/sh
oldt=0
oldt_st=0
#chuvstv_old=0
flg_day_old=1
while :
do
sleep 1
	source /root/ini_cloud.ini
	source /root/ini.ini
#	source /root/cam.ini
	source /root/range.ini
	source /root/kill_all.ini

	hour=$(date +%H)                                                                          
 if [ $hour -ge $rangea ] && [ $hour -lt $rangeb ];then
    flg_day=1
    echo "flg_day=1" > /tmp/hour.txt                                                  
    let tmp=$oldt+$per_zap_day
	let tmp_st=$oldt_st+$per_zap_day_st
 else                                                                              
    flg_day=0
	let tmp=$oldt+$per_zap_night
	let tmp_st=$oldt_st+$per_zap_night_st
    echo "flg_day=0" > /tmp/hour.txt
 fi
 
 if [ $rec_st -eq 1 ];then                                                                   
    T_st=$(date +%s)
        if [ "$T_st" -gt "$tmp_st" ];then             
            oldt_st=$T_st                                                   
                if [ $per_zap_vkl_day_st -eq 1 ] && [ $flg_day -eq 1 ];then /root/./time_st.sh
                fi                                                                        
                if [ $per_zap_vkl_night_st -eq 1 ] && [ $flg_day -eq 0 ];then /root/./time_st.sh
                fi                                                                        
        fi
 fi

 if [ $rec_cl -eq 1 ];then
		T=$(date +%s)
		if [ "$T" -gt "$tmp" ];then
			oldt=$T
			if [ $per_zap_vkl_day -eq 1 ] && [ $flg_day -eq 1 ];then /root/./time.sh
			fi
			if [ $per_zap_vkl_night -eq 1 ] && [ $flg_day -eq 0 ];then /root/./time.sh
			fi
		fi
 fi
done


