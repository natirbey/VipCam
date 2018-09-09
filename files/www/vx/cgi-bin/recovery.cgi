#!/bin/sh
echo "Content-type: text/html"
echo ""
GAT=$(route | awk 'NR==3 {{print $2}}')
				if [ $GAT == "*" ] || [ $GAT == "OpenWrt.lan" ];then
					sed -n '/wifi_sta/s/1/0/g' /www/vx/js/wifi_sta.js
					echo "wifi_sta=1" > /root/wifi_sta.ini
					uci set wireless.@wifi-iface[1].disabled=1
					uci commit wireless
					#wifi reload
					wifi
					echo 1
				else
					echo 2
				fi
#echo 2
rm /tmp/up
cp /www/vx/menu.html /www/vx/index.html
#/etc/init.d/3ginfo restart