#!/bin/sh
	source /root/wifi.ini
	source /root/wifi_ap.ini
	A=$(uci get wireless.@wifi-iface[1].disabled)
	B=$(uci get wireless.@wifi-iface[0].disabled)
	
	uci set wireless.@wifi-iface[1].disabled=1
	uci set wireless.@wifi-iface[0].disabled=0
	uci commit wireless
	 wifi reload
	sleep $vidimost

while [ -f /tmp/up ]; do
sleep 1
done 
	cp /usr/share/3g/menu.html /usr/share/3g/index.html

	source /root/wifi.ini
	source /root/wifi_sta.ini
	source /root/wifi_ap.ini

	if [ $A -eq $wifi_sta ];then
		uci set wireless.@wifi-iface[1].disabled=$A
	fi
	if [ $B -eq $wifi_ap ];then
		uci set wireless.@wifi-iface[0].disabled=$B
	fi
        	uci commit wireless
        	wifi reload
