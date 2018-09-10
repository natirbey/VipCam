#!/bin/sh
echo "Content-type: text/html"
echo ""
if [ "$REQUEST_METHOD" = "POST" ]; then
	read POST_STRING
fi
echo $POST_STRING > /tmp/tmp1
TMP=$(uhttpd -d $POST_STRING | awk 'NR==1 {{lev=substr($2,1)}{print lev}}' -F 'vidimost=' | awk 'NR==1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "var vidimost="$TMP > /www/vx/js/wifi_full.js
echo "vidimost="$TMP > /root/wifi.ini
TMP=$(uhttpd -d $POST_STRING | awk 'NR==1 {{lev=substr($2,1)}{print lev}}' -F 'vkl_wifi=' | awk 'NR==1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo $TMP >> /tmp/tmp
echo "var wifi="$TMP > /www/vx/js/wifi.js
if [ $TMP -eq 1 ];then
#	ifconfig wlan0-1 up
	TMPN=0
else 
	TMPN=1
#	ifconfig wlan0-1 down
fi
echo "wifi_ap="$TMPN > /root/wifi_ap.ini
uci set wireless.@wifi-iface[0].disabled=$TMPN

TMP=$(uhttpd -d $POST_STRING | awk 'NR==1 {{lev=substr($2,1)}{print lev}}' -F 'channel=' | awk 'NR==1 {{lev=substr($1,1)}{print lev}}' -F '&')
uci set wireless.@wifi-device[0].channel=$TMP
echo "var channel="$TMP >> //www/vx/js/wifi.js

TMP=$(uhttpd -d $POST_STRING | awk 'NR==1 {{lev=substr($2,1)}{print lev}}' -F 'ssid=' | awk 'NR==1 {{lev=substr($1,1)}{print lev}}' -F '&')
uci set wireless.@wifi-iface[0].ssid=$TMP
echo "var ssid="\'$TMP\' >> /www/vx/js/wifi.js

TMP=$(uhttpd -d $POST_STRING | awk 'NR==1 {{lev=substr($2,1)}{print lev}}' -F 'pass=' | awk 'NR==1 {{lev=substr($1,1)}{print lev}}' -F '&')
uci set wireless.@wifi-iface[0].key=$TMP
echo "var pass="\'$TMP\' >> /www/vx/js/wifi.js

TMP=$(uhttpd -d $POST_STRING | awk 'NR==1 {{lev=substr($2,1)}{print lev}}' -F 'encript=' | awk 'NR==1 {{lev=substr($1,1)}{print lev}}' -F '&')
uci set wireless.@wifi-iface[0].encryption=$TMP
echo "var encript="\'$TMP\' >> /www/vx/js/wifi.js
chmod 777 /www/vx/js/wifi.js

#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


#VKL=$(uhttpd -d $POST_STRING | awk 'NR==1 {{lev=substr($2,1)}{print lev}}' -F 'vkl_wifi_sta=' | awk 'NR==1 {{lev=substr($1,1)}{print lev}}' -F '&')
#echo "var wifi_sta="$VKL > /usr/share/3g/wifi_sta.js
#if [ $VKL -eq 1 ];then
#		TMPN=0
#	else
#		TMPN=1
#fi
#echo "wifi_sta="$TMPN > /root/wifi_sta.ini
#SSID=$(uhttpd -d $POST_STRING | awk 'NR==1 {{lev=substr($2,1)}{print lev}}' -F 'ssid_sta=' | awk 'NR==1 {{lev=substr($1,1)}{print lev}}' -F '&')
#echo "var ssid_sta="\'$SSID\' >> /usr/share/3g/wifi_sta.js
#PASS=$(uhttpd -d $POST_STRING | awk 'NR==1 {{lev=substr($2,1)}{print lev}}' -F 'pass_sta=' | awk 'NR==1 {{lev=substr($1,1)}{print lev}}' -F '&')
#echo "var pass_sta="\'$PASS\' >> /usr/share/3g/wifi_sta.js
#if [ $VKL -eq 1 ];then
#		iwinfo wlan0 scan | grep -w $SSID > /dev/null
#		if [ $? -ne 0 ];then
#			echo 3
#		else
#			iwinfo wlan0 scan | grep -B 1 $SSID > /tmp/wifi.tmp
#			sleep 1
#			iwinfo wlan0 scan | grep -A 3 $SSID >> /tmp/wifi.tmp
#			BSSID=$(awk 'NR==1 {{lev=substr($2,1)}{print lev}}' -F 'Address: ' /tmp/wifi.tmp)
#			CHAN=$( awk 'NR==4 {{lev=substr($2,1)}{print lev}}' -F 'Channel: ' /tmp/wifi.tmp)
#			uci set wireless.@wifi-device[0].channel=$CHAN
#			uci set wireless.@wifi-iface[1].bssid=$BSSID
#			uci set wireless.@wifi-iface[1].ssid=$SSID
#			uci set wireless.@wifi-iface[1].key=$PASS
##		uci set wireless.@wifi-iface[1].wds=1

#			WPA=$(awk 'NR==6 {{lev=substr($2,1)}{print lev}}' -F 'Encryption: '  /tmp/wifi.tmp | awk 'NR==1 {{lev=substr($1,1)}{print lev}}')
#			TKIP=$(awk 'NR==6 {{lev=substr($2,1)}{print lev}}' -F 'Encryption: '  /tmp/wifi.tmp | awk 'NR==1 {{lev=substr($3,1)}{print lev}}')
#			if [ "$WPA" != "mixed" ]; then
#				case $WPA in
#				"WPA" ) TMP1="psk"
#				;;	
#				"WPA2" ) TMP1="psk2"
#				;;
#				"none" ) TMP1="none"
#				;;
#				esac
#			else
#				TMP1="psk-mixed"
#			fi
#			if [ "$WPA" != "none" ]; then
#				case $TKIP in
#				"(TKIP)" ) TMP2="tkip"
#				;;
#				"(CCMP)" ) TMP2="ccmp"
#				;;
#				"(TKIP, CCMP)" ) TMP2="tkip+ccmp"
#				;;
#				esac
#				TMP=$TMP1+$TMP2
#				else TMP="none"
#			fi
#		uci set wireless.@wifi-iface[1].encryption=$TMP
#		uci set wireless.@wifi-iface[1].disabled=0
#		uci commit wireless
#		wifi
#		#wifi reload
##		otvalivaetsa wifi  
#		# sleep 10
#		sleep 5
#		a=0;
#		echo $a > /tmp/9
#			while [ "$a" -le 10 ]; do
#			echo $a > /tmp/8
#			# if [ "$(iwinfo radio0 assoclist | awk 'NR=1 {{lev=substr($1,1)}{print lev}}')" != "No" ];then
#			GAT=$(route | awk 'NR==3 {{print $2}}')
#			if [ $GAT ==  "OpenWrt.lan" ];then
#				echo 8
#				#uci set network.lan.ipaddr="192.168.1.200"
#				#uci commit network
#				#/etc/init.d/network restart
#				#sleep 3 
#				exit 0
#			fi
#			if [ $GAT != "*" ] && [ $GAT != "OpenWrt.lan" ];then
#				uci set network.relay.gateway=$GAT
#				uci commit network
#				/etc/init.d/relayd restart
#				sleep 1
#				a=12;
#			else
#				sleep 1
#				let "a=a+1"
#			fi
#			done
#			echo $a > /tmp/7
#			if [ "$a" -eq 12 ];then
#				# GAT=$(route | awk 'NR==3 {{print $2}}') 
#				# if [ $GAT != "*" ] && [ $GAT != "OpenWrt.lan" ];then
#					# uci set network.relay.gateway=$GAT
#					# uci commit network
#					# /etc/init.d/relayd restart
#				echo $GAT > /tmp/gat
#				S=$(ping -c2 -w2 $GAT | awk 'NR==3 {if ($2=="bytes"){print "0"} else {print "1"}}')
#				#S=$(ping -c2 -w2 ya.ru | awk 'NR==3 {if ($2=="bytes"){print "0"} else {print "1"}}') !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#				if [ $S -eq 0 ];then
#					echo $S > /tmp/s
#					echo 1
#				else
#					echo $S > /tmp/s
#					echo 0
#				fi
#				# else
#					# echo 5
#					# sed -n '/wifi_sta/s/1/0/g' /usr/share/3g/wifi_sta.js
#					# uci set wireless.@wifi-iface[1].disabled=1
#					# uci commit wireless 
#					# wifi
#					# #wifi reload
#				# fi
#			else
#				echo 5
#				sed -n '/wifi_sta/s/1/0/g' /usr/share/3g/wifi_sta.js
#				uci set wireless.@wifi-iface[1].disabled=1
#				uci commit wireless 
#				wifi
#				#wifi reload
#			fi
#		fi
#	else
#		echo 4
#		uci set wireless.@wifi-iface[1].disabled=1
#		uci commit wireless 
#		wifi
#		#wifi reload
#fi
echo 2
uci commit wireless
wifi reload