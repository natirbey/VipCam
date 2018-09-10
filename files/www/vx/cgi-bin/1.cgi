#!/bin/sh
echo "Content-type: text/html"
echo ""
#echo "9"
GAT=$(route | awk 'NR==3 {{print $2}}')
			if [ $GAT ==  "OpenWrt.lan" ];then
				echo 8
				#uci set network.lan.ipaddr="192.168.1.200"
				#uci commit network
				#/etc/init.d/network restart
				#sleep 3 
				exit 0
			fi
S=$(ping -c2 -w2 ya.ru | awk 'NR==3 {if ($2=="bytes"){print "0"} else {print "1"}}')
#S=$(ping -c2 -w2 192.168.1.7 | awk 'NR==3 {if ($2=="bytes"){print "0"} else {print "1"}}')
# GAT=$(route | awk 'NR==3 {{print $2}}')
		 # if [ $GAT != "*" ];then
			 # uci set network.relay.gateway=$GAT
			 # uci commit network
			 # /etc/init.d/relayd restart
			 # S=$(ping -c2 -w2 $GAT | awk 'NR==3 {if ($2=="bytes"){print "0"} else {print "1"}}')
			if [ $S -eq 0 ];then
			echo 1
			else
			echo 9
			  # sed '/wifi_sta/s/1/0/g' /usr/share/3g/wifi_sta.js
					  # uci set wireless.@wifi-iface[1].disabled=1
					  # uci commit wireless 
					  # wifi 
			fi
					#wifi reload
			#uci delete wireless.@wifi-iface[1] 
			#uci commit wireless 
			#wifi reload
			#vosstanavlivaetsa wifi
			#sleep 5 
			#echo 0
		# else
			# echo 9
			# sed '/wifi_sta/s/1/0/g' /usr/share/3g/wifi_sta.js
					# uci set wireless.@wifi-iface[1].disabled=1
					# uci commit wireless 
					# wifi
#		 fi 