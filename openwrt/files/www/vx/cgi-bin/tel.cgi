#!/bin/sh
echo "Content-type: text/html"
echo ""
if [ "$REQUEST_METHOD" = "POST" ]; then
read POST_STRING
 fi
TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F '=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "var vkl2="$TMP > /www/vx/js/alarm_sms.js
TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($3,1)}{print lev}}' -F '=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "var tel="\"$TMP\" >> /www/vx/js/alarm_sms.js
