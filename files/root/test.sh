#!/bin/sh
#a=$(iwinfo wlan0 scan | grep ESSID | awk '{print $2}' -F '"')
#echo $a > /tmp/table
#echo $a
#n="15"
#echo -n $(iwinfo wlan0 scan | grep ESSID | awk '{print $2}' -F '"' | sed ':a;N;$!ba;s/\n/,/g' | awk '{print NF,$0}' | tr -d '\n')
#echo -n $(iwinfo wlan0 scan | grep ESSID | awk '{print $2}' -F '"' | sed ':a;N;$!ba;s/\n/,/g')
#echo -n $(iwinfo wlan0 scan | grep ESSID | awk '{print $2}' -F '"' | sed ':a;N;$!ba;s/\n/,/g' | awk '{print NF","$0}' -F ',' | tr -d '\n')
A=0
C=1
while [ "$A" -eq 0 ]; do
B=192.168.1.$C
A=$(ping -c2 -w2 $B | awk 'NR==3 {if ($2=="bytes"){print "0"} else {print "1"}}')
let "C=C+1"
echo "B="$B
echo "A="$A
done
