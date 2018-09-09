#!/bin/sh
echo "Content-type: text/html"
echo ""
if [ "$REQUEST_METHOD" = "POST" ]; then
read POST_STRING
 fi
TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F '=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "var vkl1="$TMP > /www/vx/js/alarm.js
TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($3,1)}{print lev}}' -F '=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "var mail="\"$TMP\" >> /www/vx/js/alarm.js
echo "root="$TMP > /etc/ssmtp/ssmtp.conf
echo "mailhub=smtp.gmail.com:495" >> /etc/ssmtp/ssmtp.conf
echo "rewriteDomain=gmail.com" >> /etc/ssmtp/ssmtp.conf
echo "FromLineOverride=YES" >> /etc/ssmtp/ssmtp.conf
echo "UseTLS=YES" >> /etc/ssmtp/ssmtp.conf
echo "AuthUser="$TMP >> /etc/ssmtp/ssmtp.conf
TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($4,1)}{print lev}}' -F '=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "var pass="\"$TMP\" >> /usr/share/3ginfo/alarm.js
echo "AuthPass="$TMP >> /etc/ssmtp/ssmtp.conf
chmod 775 /etc/ssmtp/ssmtp.conf
