#!/bin/sh
echo "Content-type: text/html"
echo ""
if [ "$REQUEST_METHOD" = "POST" ]; then
read POST_STRING
 fi
TMP=$(uhttpd -d "$POST_STRING" | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F '=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
TMP1=$(uhttpd -d "$POST_STRING" | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F '=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '@')
echo "var login="\"$TMP\" > /www/vx/js/ya.js
#uci set network.@interface[1].proto=$TMP

TMP2=$(uhttpd -d "$POST_STRING" | awk 'NR=1 {{lev=substr($3,1)}{print lev}}' -F '=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "var pass="\"$TMP2\" >> /www/vx/js/ya.js
echo "https://webdav.yandex.ru/ "$TMP1 $TMP2 > /etc/davfs2/secrets
chmod 0600 /etc/davfs2/secrets
#reboot

TMP4=$(uhttpd -d "$POST_STRING" | awk 'NR=1 {{lev=substr($4,1)}{print lev}}' -F '=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "var mount="$TMP4 >> /www/vx/js/ya.js
echo $TMP > /root/mount