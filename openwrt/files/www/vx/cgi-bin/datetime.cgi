#!/bin/sh
echo "Content-type: text/html"
echo ""
if [ "$REQUEST_METHOD" = "POST" ]; then
read POST_STRING
 fi
echo $POST_STRING > /tmp/tmp
 TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F '=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
 date -s "$TMP"
 TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($3,1)}{print lev}}' -F '=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&' | sed 's/%3A/:/g')
 date +%T -s "$TMP"

#echo "kill_flg=1" > /root/kill_all.ini
#		 while [ ! -e $(pgrep openRTSP) ]; do
#                killall openRTSP
#                done
#                while [ ! -e $(pgrep motion.sh) ]; do
#                killall motion.sh
#                done
#                while [ ! -e $(pgrep rtsp_client.sh) ]; do
#                killall rtsp_client.sh
#                done
#                while [ ! -e $(pgrep alarmserverm_openwrt) ]; do
#                killall alarmserverm_openwrt
#                done
#                while [ -e $(pgrep alarmserverm_openwrt) ]; do
#                /root/alarmserverm_openwrt 15002 $chuvstv &

#                done
 
#killall alarmserverm_openwrt
#killall rtsp_client.sh
#killall motion.sh
#killall openRTSP
#/root/alarmserverm_openwrt 15002 $chuvstv &
#privet=2
 #export privet
 # echo "export PRIVET=2" > /tmp/root
 # cd /tmp
 # chmod 777 root
 #./1.sh
