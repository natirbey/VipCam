#!/bin/sh
echo "Content-type: text/html"
echo ""
if [ "$REQUEST_METHOD" = "POST" ]; then
read POST_STRING
fi
echo $POST_STRING > /tmp/tmp
TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'vkl_wifi=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo $TMP > /tmp/tmp1
echo "var wifi="$TMP > /www/vx/js/wifi.js
echo "wifi="$TMP > /root/wifi.ini
if [ $TMP -eq 1 ];then
TMPN=0
else TMPN=1
fi
uci set wireless.@wifi-device[0].disabled=$TMPN

MODE=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'mode=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
uci set wireless.@wifi-iface[0].mode=$TMP
echo "var mode="\'$MODE\' >> /www/vx/js/wifi.js

#esli wds

if [[ "$MODE" = "st_wds" || "$MODE" = "ap_wds" ]]; then
uci set wireless.@wifi-iface[0].wds=1
else set wireless.@wifi-iface[0].wds=0
fi
uci set wireless.@wifi-iface[0].mode=$MODE

SSID=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'ssid=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
uci set wireless.@wifi-iface[0].ssid=$SSID
echo "var ssid="\'$SSID\' >> /www/vx/js/wifi.js

TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'pass=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
uci set wireless.@wifi-iface[0].key=$TMP
echo "var pass="\'$TMP\' >> /www/vx/js/wifi.js

#esli client
if [[ "$MODE" = "sta" || "$MODE" = "st_wds" ]]; then
#TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'encript=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
#uci set wireless.@wifi-iface[0].encryption=$TMP
iwinfo wlan0 scan | grep -B 1 $SSID > /tmp/wifi.tmp
iwinfo wlan0 scan | grep -A 3 $SSID >> /tmp/wifi.tmp
BSSID=$(awk 'NR==1 {{lev=substr($2,1)}{print lev}}' -F 'Address: ' /tmp/wifi.tmp)
CHAN=$( awk 'NR==4 {{lev=substr($2,1)}{print lev}}' -F 'Channel: ' /tmp/wifi.tmp)
uci set wireless.@wifi-device[0].channel=$CHAN
uci set wireless.@wifi-iface[0].bssid=$BSSID
#echo 5 > /tmp/999
#obrabotka encript
#ENCRIPT=$(iwinfo wlan0 scan | grep -C 3 $SSID | awk 'NR==5 {{lev=substr($2,1)}{print lev}}' -F 'Encryption: ')
#echo $ENCRIPT >> /tmp/bssid
WPA=$(awk 'NR==6 {{lev=substr($2,1)}{print lev}}' -F 'Encryption: '  /tmp/wifi.tmp | awk 'NR==1 {{lev=substr($1,1)}{print lev}}')
#PSK=$( iwinfo wlan0 scan | grep -C 3 $SSID | awk 'NR==5 {{lev=substr($2,1)}{print lev}}' -F 'Encryption: ' | awk 'NR==1 {{lev=substr($2,1)}{print lev}}' ) 
TKIP=$(awk 'NR==6 {{lev=substr($2,1)}{print lev}}' -F 'Encryption: '  /tmp/wifi.tmp | awk 'NR==1 {{lev=substr($3,1)}{print lev}}')
if [ "$WPA" != "mixed" ]; then
case $WPA in
"WPA" ) TMP1="psk"
;;
"WPA2" ) TMP1="psk2"
;;
"none" ) TMP1="none"
;;
esac
else
TMP1="psk-mixed"
fi
if [ "$WPA" != "none" ]; then
case $TKIP in
"(TKIP)" ) TMP2="tkip"
;;
"(CCMP)" ) TMP2="ccmp"
;;
"(TKIP, CCMP)" ) TMP2="tkip+ccmp"
;;
esac
TMP=$TMP1+$TMP2
else TMP="none"
fi
uci set wireless.@wifi-iface[0].encryption=$TMP
#uci commit wireless                                                                                                                          
#wifi reload
#echo "var encript="\'$TMP\' >> /usr/share/3g/wifi.js
fi
#uci commit wireless
if [[ "$MODE" = "ap" || "$MODE" = "ap_wds" ]]; then
TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'channel=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
uci set wireless.@wifi-device[0].channel=$TMP
#uci delete wireless.@wifi-iface[0].bssid
echo "var channel="$TMP >> /www/vx/js/wifi.js

TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'vidimost=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
if [ "$TMP" != "" ];then
echo "var vidimost="$TMP >> /www/vx/js/wifi.js
echo "vidimost="$TMP >> /root/wifi.ini
else 
echo "var vidimost="20 >> /www/vx/js/wifi.js
echo "vidimost="20 >> /root/wifi.ini
fi

TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'encript=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
uci set wireless.@wifi-iface[0].encryption=$TMP
echo "var encript="\'$TMP\' >> /www/vx/js/wifi.js
echo "encript="\'$TMP\' >> /root/wifi.ini
chmod 777 /www/vx/js/wifi.js
fi
uci commit wireless
wifi reload
