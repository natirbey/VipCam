#!/bin/sh
echo "Content-type: text/html"
echo ""
if [ "$REQUEST_METHOD" = "POST" ]; then
read POST_STRING
 fi
#uhttpd -d "$POST_STRING" | awk 'NR=1 {{lev=substr($1,11)}{print lev}}' > /etc/config/ipcam
TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'streamer=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
#echo "var RTSP="\'$(cat /etc/config/ipcam)\' > /usr/share/3g/ipcam.js
echo "var RTSP="\'$TMP\' > /www/vx/js/cam.js                                                                                                            
echo "RTSP=$TMP                                                " > /root/cam.ini

TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'chuvstv=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "var chuvstv="$TMP >> /www/vx/js/cam.js                                                                                                          
echo "chuvstv=$TMP                                                    #4uvstvitelnost kameri no4" >> /root/cam.ini
chmod 777 /www/vx/js/cam.js
echo $TMP > /root/4uvstv.ini
#echo "kill_flg=1" > /root/kill_all.ini 