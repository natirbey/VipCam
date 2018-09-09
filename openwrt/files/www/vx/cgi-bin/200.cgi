#!/bin/sh
echo "Content-type: text/html"
echo ""
A=0
C=2
while [ "$A" -eq 0 ]; do
B=192.168.1.$C
A=$(ping -c2 -w2 $B | awk 'NR==3 {if ($2=="bytes"){print "0"} else {print "1"}}')
let "C=C+1"
done 
# uci set network.lan.ipaddr=$B
# uci set network.relay.ipaddr=$B
echo "var ip="\"$B\" > /usr/share/3g/ftp.js
echo -n $B
#echo "var ip=\"ftp://192.168.1.200\"" > /usr/share/3g/ftp.js