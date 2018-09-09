#!/bin/sh
echo "Content-type: text/html"
echo ""
#iwinfo wlan0 scan | grep ESSID > /tmp/ssid_sta
#awk 'NR==1 {{lev=substr($2,1)}{print lev}}' -F ' "' /tmp/ssid_sta > /tmp/ssid
#awk 'NR==2 {{lev=substr($2,1)}{print lev}}' -F ' "' /tmp/ssid_sta >> /tmp/ssid
#awk 'NR==3 {{lev=substr($2,1)}{print lev}}' -F ' "' /tmp/ssid_sta >> /tmp/ssid
#awk 'NR==4 {{lev=substr($2,1)}{print lev}}' -F ' "' /tmp/ssid_sta >> /tmp/ssid
#echo $(sed ':a;N;$!ba;s/\n//g' /tmp/ssid)

#A=$(iwinfo wlan0 scan | grep ESSID | awk '{print $2}' -F '"' | tr -d '\n')
#A=$(iwinfo wlan0 scan | grep ESSID | awk '{print $2}' -F '"')

#A=$(iwinfo wlan0 scan | grep ESSID | awk '{print $2}' -F '"' | sed ':a;N;$!ba;s/\n/,/g' | tr -d '\n')
#echo "$A" > /tmp/A
#echo $A
#echo -n $(iwinfo wlan0 scan | grep ESSID | awk '{print $2}' -F '"' | sed ':a;N;$!ba;s/\n/,/g' | tr -d '\n')
#echo -n echo $n,$(iwinfo wlan0 scan | grep ESSID | awk '{print $2}' -F '"' | sed ':a;N;$!ba;s/\n/,/g' | tr -d '\n')
echo -n $(iwinfo wlan0 scan | grep ESSID | awk '{print $2}' -F '"' | sed ':a;N;$!ba;s/\n/,/g' | awk '{print NF","$0}' -F ',' | tr -d '\n')