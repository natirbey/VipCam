#!/bin/sh
echo "Content-type: text/html"
echo ""
 TMP1=$(date +%Y-%m-%d)
 TMP2=$(date +%T)
 echo "var s_date="\"$TMP1\" > /www/vx/js/date.js
 echo "var s_time="\"$TMP2\" >> /www/vx/js/date.js