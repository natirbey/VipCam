#!/bin/sh
echo "Content-type: text/html"
echo ""
if [ "$REQUEST_METHOD" = "POST" ]; then
		read POST_STRING
fi
#echo $POST_STRING > /tmp/tmp

VKL=$(uhttpd -d $POST_STRING | awk 'NR==1 {{lev=substr($2,1)}{print lev}}' -F 'vkl_wifi_sta=' | awk 'NR==1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "var wifi_sta="$VKL > /www/vx/js/wifi_sta.js
if [ $VKL -eq 1 ];then
		TMPN=0
	else
		TMPN=1
fi
echo "wifi_sta="$TMPN > /root/wifi_sta.ini
SSID=$(uhttpd -d $POST_STRING | awk 'NR==1 {{lev=substr($2,1)}{print lev}}' -F 'ssid_sta=' | awk 'NR==1 {{lev=substr($1,1)}{print lev}}' -F '&')
#uci set wireless.@wifi-iface[1].ssid=$SSID
echo "var ssid_sta="\'$SSID\' >> /www/vx/js/wifi_sta.js
PASS=$(uhttpd -d $POST_STRING | awk 'NR==1 {{lev=substr($2,1)}{print lev}}' -F 'pass_sta=' | awk 'NR==1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "var pass_sta="\'$PASS\' >> /www/vx/js/wifi_sta.js
#uci set wireless.@wifi-iface[1].key=$PASS
if [ $VKL -eq 1 ];then
		iwinfo wlan0 scan | grep $SSID > /dev/null
		#if [[ "$MODE" = "ap" || "$MODE" = "ap_wds" ]]; then
		#if [[ $? -ne 0 || $TMP -eq 0 ]];then
		if [ $? -ne 0 ];then
			#error
			#uci delete wireless.@wifi-iface[1]
			#uci commit wireless
			#wifi reload
			echo 3
		else
			#uci delete wireless.@wifi-iface[1] 
			#uci commit wireless 
			#uci add wireless wifi-iface  > /dev/null
			#uci commit wireless
			#uci set wireless.@wifi-iface[1].device="radio0"
			#uci set wireless.@wifi-iface[1].mode="sta"
			#uci set wireless.@wifi-iface[1].network="wifi"

			iwinfo wlan0 scan | grep -B 1 $SSID > /tmp/wifi.tmp
			sleep 1
			iwinfo wlan0 scan | grep -A 3 $SSID >> /tmp/wifi.tmp
			BSSID=$(awk 'NR==1 {{lev=substr($2,1)}{print lev}}' -F 'Address: ' /tmp/wifi.tmp)
			CHAN=$( awk 'NR==4 {{lev=substr($2,1)}{print lev}}' -F 'Channel: ' /tmp/wifi.tmp)
			uci set wireless.@wifi-device[0].channel=$CHAN
			uci set wireless.@wifi-iface[1].bssid=$BSSID
			uci set wireless.@wifi-iface[1].ssid=$SSID
			uci set wireless.@wifi-iface[1].key=$PASS
#		uci set wireless.@wifi-iface[1].wds=1

			WPA=$(awk 'NR==6 {{lev=substr($2,1)}{print lev}}' -F 'Encryption: '  /tmp/wifi.tmp | awk 'NR==1 {{lev=substr($1,1)}{print lev}}')
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
		uci set wireless.@wifi-iface[1].encryption=$TMP
		uci set wireless.@wifi-iface[1].disabled=0
		uci commit wireless
		wifi reload
#		otvalivaetsa wifi

		sleep 10
		#S=$(ping -c1 -w1 192.168.43.1 | awk 'NR==2 {if ($2=="bytes"){print "0"} else {print "1"}}')
		#S=$(ping -c1 -w1 192.168.1.20 | awk 'NR==2 {if ($2=="bytes"){print "0"} else {print "1"}}')
			if [ "$(iwinfo wlan0 assoclist | awk 'NR=1 {{lev=substr($1,1)}{print lev}}')" != "No" ];then
				S=$(ping -c2 -w2 ya.ru | awk 'NR==3 {if ($2=="bytes"){print "0"} else {print "1"}}')
				if [ $S -eq 0 ];then
					echo 1
				else
					#uci delete wireless.@wifi-iface[1] 
					uci set wireless.@wifi-iface[1].disabled=1
					uci commit wireless 
					wifi reload
					#vosstanavlivaetsa wifi
					#sleep 5
					echo 0
				fi
			else
				echo 5
			fi
		fi
	else
		echo 4
		uci set wireless.@wifi-iface[1].disabled=1
		uci commit wireless 
		wifi reload
fi
#else
#uci set wireless.@wifi-iface[1].disabled=1
#uci commit wireless 
#wifi reload