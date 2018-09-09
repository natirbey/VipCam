#!/bin/sh
echo "Content-type: text/html"
echo ""
if [ "$REQUEST_METHOD" = "POST" ]; then
read POST_STRING
 fi
TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'rec_st=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "rec_st=$TMP                                                            #vkl/vikl zapis na storage"> /root/ini.ini
echo "var rec_st="$TMP > /www/vx/js/ini.js

TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'zap_dv_day_st=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "zap_dv_day_st="$TMP"                                                          #vkl/vikl zapis po dvigenilu dnjem na storage">> /root/ini.ini
echo "var zap_dv_day_st="$TMP >> /www/vx/js/ini.js

TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'zap_dv_night_st=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "zap_dv_night_st=$TMP                                                             #vkl/vikl vkl/vikl zapis po dvigenilu no4ju na storage">> /root/ini.ini
echo "var zap_dv_night_st="$TMP >> /www/vx/js/ini.js

TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'per_zap_vkl_day_st=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "per_zap_vkl_day_st=$TMP                                                          #vkl/vikl periodi4eskuju zapis dnjem na storage">> /root/ini.ini
echo "var per_zap_vkl_day_st="$TMP >> /www/vx/js/ini.js

TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'per_zap_vkl_night_st=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "per_zap_vkl_night_st=$TMP                                                       #vkl/vikl periodi4eskuju zapis no4ju na storage">> /root/ini.ini
echo "var per_zap_vkl_night_st="$TMP >> /www/vx/js/ini.js

TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'dlit_rol_dv_day_st=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "dlit_rol_dv_day_st="$TMP"                                                        #dlitelnost rolikov po dvigeniju dnjem na storage">> /root/ini.ini
echo "var dlit_rol_dv_day_st="$TMP >> /www/vx/js/ini.js

TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'dlit_rol_dv_night_st=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "dlit_rol_dv_night_st=$TMP                                                        #dlitelnost rolikov po dvigeniju no4ju na storage">> /root/ini.ini
echo "var dlit_rol_dv_night_st="$TMP >> /www/vx/js/ini.js

TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'per_zap_day_st=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "per_zap_day_st="$TMP"                                                        #period zapisi po dvigeniju na storage">> /root/ini.ini
echo "var per_zap_day_st="$TMP >> /www/vx/js/ini.js

TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'prod_per_day_st=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "prod_per_day_st="$TMP"                                                        #prodolgitelnost periodi4eskoj zapisi dnjem na storage">> /root/ini.ini
echo "var prod_per_day_st="$TMP >> /www/vx/js/ini.js

#TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'rangea=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
#echo "rangea_st="$TMP"                                                        #na4alo dnja">> /root/ini.ini
#echo "var rangea="$TMP >> /usr/share/3g/ini.js

#TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'rangeb=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
#echo "rangeb_st="$TMP"                                                        #konec dnja">> /root/ini.ini
#echo "var rangeb="$TMP >> /usr/share/3g/ini.js

TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'per_zap_night_st=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "per_zap_night_st="$TMP"                                                        #period zapisi no4ju na storage">> /root/ini.ini
echo "var per_zap_night_st="$TMP >> /www/vx/js/ini.js

TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'prod_per_night_st=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "prod_per_night_st="$TMP"                                                        #prodolgitelnost periodi4eskoj zapici no4ju na storage">> /root/ini.ini
echo "var prod_per_night_st="$TMP >> /www/vx/js/ini.js
