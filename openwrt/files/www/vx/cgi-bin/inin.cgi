#!/bin/sh
echo "Content-type: text/html"
echo ""
if [ "$REQUEST_METHOD" = "POST" ]; then
read POST_STRING
 fi
source /root/rest.ini
TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'rec_cl=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "var rec_cl="$TMP > /www/vx/js/inin.js
echo "rec_cl=$TMP                                                             #vkl/vikl zapic v oblako" > /root/ini_cloud.ini

#TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'rangea=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
#echo "var rangea="$TMP >> /usr/share/3g/inin.js
#echo "rangea=$TMP                                                             #nachalo dnja">> /root/ini_cloud.ini

TMP1=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'email=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
TMP3=$(uhttpd -d "$POST_STRING" | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'email=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '@')
echo "var email="\"$TMP1\" >> /www/vx/js/inin.js
echo "email=\"$TMP3\"                                                         #login curl">> /root/ini_cloud.ini

TMP2=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'pass=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "var pass="\"$TMP2\" >> /www/vx/js/inin.js
echo "pass=\"$TMP2\"                                                          #parol curl">> /root/ini_cloud.ini

#TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'rangeb=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
#echo "var rangeb="$TMP >> /usr/share/3g/inin.js
#echo "rangeb=$TMP                                                             #konec dnja">> /root/ini_cloud.ini

TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'service=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "var service="$TMP >> /www/vx/js/inin.js
echo "service=$TMP                                                            #vibor servisa">> /root/ini_cloud.ini

TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'zap_dv_day=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')

echo "var zap_dv_day="$TMP >> /www/vx/js/inin.js
echo "zap_dv_day=$TMP                                                         #vkl/vikl zapis po dvigenilu day">> /root/ini_cloud.ini

TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'dlit_rol_dv_day=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
if [ $TMP -gt $max_dlit_mot ];then
TMP=$max_dlit_mot
fi
echo "var dlit_rol_dv_day="$TMP >> /www/vx/js/inin.js
echo "dlit_rol_dv_day=$TMP                                                    #dlitelnost rolika po dvigeniju dnjem">> /root/ini_cloud.ini

TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'zap_dv_night=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "var zap_dv_night="$TMP >> /www/vx/js/inin.js
echo "zap_dv_night=$TMP                                                       #vkl/vikl zapic po dvigeniju night">> /root/ini_cloud.ini

TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'dlit_rol_dv_night=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
if [ $TMP -gt $max_dlit_mot ];then
TMP=$max_dlit_mot
fi
echo "var dlit_rol_dv_night="$TMP >> /www/vx/js/inin.js
echo "dlit_rol_dv_night=$TMP                                                  #dlitelnost rolika po dvigeniju no4ju">> /root/ini_cloud.ini

TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'per_zap_vkl_day=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
echo "var per_zap_vkl_day="$TMP >> /www/vx/js/inin.js                                                      
echo "per_zap_vkl_day=$TMP                                                    #vkl/vikl periodi4eskuyu zapic dnjem">> /root/ini_cloud.ini

TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'per_zap_day=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
if [ $TMP -lt $max_per_reg ];then
TMP=$max_per_reg
fi
echo "var per_zap_day="$TMP >> /www/vx/js/inin.js  
echo "per_zap_day=$TMP							      #period zapici dnjem">> /root/ini_cloud.ini

TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'prod_per_day=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
if [ $TMP -gt $max_dlit_time ];then
TMP=$max_dlit_time
fi
echo "var prod_per_day="$TMP >> /www/vx/js/inin.js
echo "prod_per_day=$TMP                                                       #prodolgitelnost periodi4eskoj zapici dnjem">> /root/ini_cloud.ini

TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'per_zap_night=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')
if [ $TMP -lt $max_per_reg ];then
TMP=$max_per_reg
fi
echo "var per_zap_night="$TMP >> /www/vx/js/inin.js
echo "per_zap_night=$TMP						      #period rolika zapisi no4ju">> /root/ini_cloud.ini 

TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'prod_per_night=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&') 
if [ $TMP -gt $max_dlit_time ];then
TMP=$max_dlit_time
fi  
echo "var prod_per_night="$TMP >> /www/vx/js/inin.js                                                              
echo "prod_per_night=$TMP                                                     #prodolgitelnost periodi4eskoy zapisi no4ju">> /root/ini_cloud.ini

TMP=$(uhttpd -d $POST_STRING | awk 'NR=1 {{lev=substr($2,1)}{print lev}}' -F 'per_zap_vkl_night=' | awk 'NR=1 {{lev=substr($1,1)}{print lev}}' -F '&')                    
echo "var per_zap_vkl_night="$TMP >> /www/vx/js/inin.js                                                                                                            
echo "per_zap_vkl_night=$TMP                                                    #vkl/vikl periodi4eskuyu zapic no4">> /root/ini_cloud.ini
