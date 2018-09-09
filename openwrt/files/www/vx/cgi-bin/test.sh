#!/bin/sh
SSID="wifi"
WPA=$( iwinfo wlan0 scan | grep -C 3 $SSID | awk 'NR==5 {{lev=substr($2,1)}{print lev}}' -F 'Encryption: ' | awk 'NR==1 {{lev=substr($1,1)}{print lev}}' )
TKIP=$( iwinfo wlan0 scan | grep -C 3 $SSID | awk 'NR==5 {{lev=substr($2,1)}{print lev}}' -F 'Encryption: ' | awk 'NR==1 {{lev=substr($3,1)}{print lev}}' )

if [ "$WPA" != "mixed" ]; then
case $WPA in
"WPA" ) TMP1="psk"
;;
"WPA2" ) TMP1="psk2"
;;
"none" ) TMP1="none"
;;
esac
else
TMP1="psk-mixed"
fi

case $TKIP in
"(TKIP)" ) TMP2="tkip"
;;
"(CCMP)" ) TMP2="ccmp"
;;
"(TKIP, CCMP)" ) TMP2="tkip+ccmp"
;;
esac

echo $TMP1
echo $TMP2
B=$TMP1+$TMP2
echo $B
