#!/bin/sh
echo "Content-type: text/html"
echo ""
if [ "$REQUEST_METHOD" = "POST" ]; then
read POST_STRING
 fi
TMP=$(uhttpd -d "$POST_STRING" | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F '=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "var prot="\"$TMP\" > /www/vx/js/lan.js
uci set network.@interface[1].proto=$TMP

TMP=$(uhttpd -d "$POST_STRING" | awk 'NR=1 {{lev=substr($3,1)}{print lev}}' -F '=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "var addr="\"$TMP\" >> /www/vx/js/lan.js
uci set network.@interface[1].ipaddr=$TMP

TMP=$(uhttpd -d "$POST_STRING" | awk 'NR=1 {{lev=substr($4,1)}{print lev}}' -F '=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "var mask="\"$TMP\" >> /www/vx/js/lan.js
uci set network.@interface[1].netmask=$TMP

TMP=$(uhttpd -d "$POST_STRING" | awk 'NR=1 {{lev=substr($5,1)}{print lev}}' -F '=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "var gat="\"$TMP\" >> /www/vx/js/lan.js
uci set network.@interface[1].gateway=$TMP
uci commit network
/etc/init.d/network reload