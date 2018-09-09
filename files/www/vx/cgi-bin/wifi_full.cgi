#!/bin/sh
echo "Content-type: text/html"
echo ""
if [ "$REQUEST_METHOD" = "POST" ]; then
read POST_STRING
fi
echo $POST_STRING > /tmp/tmp
#TMP=$(uhttpd -d $POST_STRING | awk 'NR==1 {{lev=substr($2,1)}{print lev}}' -F 'vkl_wifi_full=' | awk 'NR==1 {{lev=substr($1,1)}{print lev}}' -F '&')
#echo "var vkl_wifi_full="$TMP > /usr/share/3g/wifi_full.js
#echo "wifi="$TMP > /root/wifi.ini
#if [ $TMP -eq 1 ];then
#TMPN=0
#else TMPN=1
#fi
#uci set wireless.@wifi-device[0].disabled=$TMPN
#echo "wifi="$TMP > /root/wifi.ini
TMP=$(uhttpd -d $POST_STRING | awk 'NR==1 {{lev=substr($2,1)}{print lev}}' -F 'vidimost=' | awk 'NR==1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "var vidimost="$TMP >> /www/vx/js/wifi_full.js
echo "vidimost="$TMP >> /root/wifi.ini
uci commit wireless
wifi reload