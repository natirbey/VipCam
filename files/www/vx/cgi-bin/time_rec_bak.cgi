#!/bin/sh
echo "Content-type: text/html"
echo ""
#source /root/wifi.ini
#E=$(uci get wireless.@wifi-iface[0].disabled)
#if [ $wifi -eq 0 ] || [ $E -eq 1 ];then
	#uci get wireless.@wifi-device[0].disabled
	source /root/wifi.ini
	source /root/wifi_ap.ini
	A=$(uci get wireless.@wifi-iface[1].disabled)
	B=$(uci get wireless.@wifi-iface[0].disabled)
	# uci set wireless.@wifi-device[0].disabled=0
	# uci set wireless.@wifi-iface[1].disabled=1
	# uci set wireless.@wifi-iface[0].disabled=0
	# uci commit wireless
	# wifi reload
#!!!	if [ $wifi_ap -eq 1 ];then
	uci set wireless.@wifi-iface[1].disabled=1
	uci set wireless.@wifi-iface[0].disabled=0
	uci commit wireless
	 wifi reload
	sleep $vidimost
#!!!	fi
	cp /www/vx/menu.html /www/vx/index.html
#function delay
#{
#	while [ "$(iwinfo wlan0 assoclist | awk 'NR=1 {{lev=substr($1,1)}{print lev}}')" != "No" ];do
#	sleep $vidimost
#	echo 1
#	done
#}
#delay
	source /root/wifi.ini
	source /root/wifi_sta.ini
	source /root/wifi_ap.ini
#	sleep 10
#	if [ $wifi_sta -eq 0 ] && [[ "$(iwinfo wlan0 assoclist | awk 'NR=1 {{lev=substr($1,1)}{print lev}}')" == "No" ]];then
#	echo 2
#	delay
#	fi
#	if [ $wifi -eq 0 ];then
#		uci set wireless.@wifi-device[0].disabled=1
#	fi
	#C=$(uci get wireless.@wifi-iface[1].disabled)
	#D=$(uci get wireless.@wifi-iface[0].disabled)
	if [ $A -eq $wifi_sta ];then
		uci set wireless.@wifi-iface[1].disabled=$A
	fi
	if [ $B -eq $wifi_ap ];then
		uci set wireless.@wifi-iface[0].disabled=$B
	fi
        	uci commit wireless
        	wifi reload
#		fi
#fi
