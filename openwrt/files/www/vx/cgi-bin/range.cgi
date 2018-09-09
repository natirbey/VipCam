#!/bin/sh
echo "Content-type: text/html"
echo ""
if [ "$REQUEST_METHOD" = "POST" ]; then
read POST_STRING
 fi
 TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'rangea=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "rangea="$TMP"                                                        #na4alo dnja" > /root/range.ini
echo "var rangea="$TMP > /www/vx/js/range.js

TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'rangeb=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "rangeb="$TMP"                                                        #konec dnja">> /root/range.ini
echo "var rangeb="$TMP >> /www/vx/js/range.js
chmod 777 /www/vx/js/range.js
chmod 777 /root/range.ini

 #privet=2
 #export privet
 # echo "export PRIVET=2" > /tmp/root
 # cd /tmp
 # chmod 777 root
 #./1.sh
