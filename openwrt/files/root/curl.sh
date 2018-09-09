#!/bin/sh
while :
do
source /root/ini_cloud.ini
if [ "$(ls -A /tmp/ya_mot/)" ]; then
A=$(ls -1Atl /tmp/ya_mot/ | tail -n1 | awk '{print $7}')
while fuser -s /tmp/ya_mot/$A; do
sleep 1
done 
DD_new=$(date +%Y-%m-%d)
if [ -e /tmp/old.txt ];then
        DD_old=$(cat /tmp/old.txt)
        if [ $DD_new != $DD_old ];then
                curl -v --stderr /tmp/logi1 -k -u $email:$pass -X MKCOL https://webdav.yandex.ru/CAM
                curl -v -k -u $email:$pass -X MKCOL https://webdav.yandex.ru/CAM/$DD_new
                curl -v -k -u $email:$pass -X MKCOL https://webdav.yandex.ru/CAM/$DD_new/MOTION
                curl -v -k -u $email:$pass -X MKCOL https://webdav.yandex.ru/CAM/$DD_new/MOTION/VIDEO
        fi
else
				curl -v --stderr /tmp/flg_mot -k -u $email:$pass -X MKCOL https://webdav.yandex.ru/CAM
				curl -v -k -u $email:$pass -X MKCOL https://webdav.yandex.ru/CAM/$DD_new
				curl -v -k -u $email:$pass -X MKCOL https://webdav.yandex.ru/CAM/$DD_new/MOTION
				curl -v -k -u $email:$pass -X MKCOL https://webdav.yandex.ru/CAM/$DD_new/MOTION/VIDEO
				if [ $(cat /tmp/flg_mot | grep -c created) -eq 1 ];then
					echo $DD_new > /tmp/old.txt
				fi
fi
#echo $DD_new > /tmp/old.txt
curl -v -m 180 --stderr /tmp/logi2 -k -u $email:$pass -T /tmp/ya_mot/$A https://webdav.yandex.ru/CAM/$DD_new/MOTION/VIDEO/
rm /tmp/ya_mot/$A
fi

if [ "$(ls -A /tmp/ya_time/)" ]; then
B=$(ls -1Atl /tmp/ya_time/ | tail -n1 | awk '{print $7}')
while fuser -s /tmp/ya_time/$B; do
sleep 1
done
DD_new=$(date +%Y-%m-%d)
if [ -e /tmp/old1.txt ];then
        DD_old=$(cat /tmp/old1.txt)
        if [ $DD_new != $DD_old ];then
                curl -v --stderr /tmp/logi5 -k -u $email:$pass -X MKCOL https://webdav.yandex.ru/CAM
                curl -v -k -u $email:$pass -X MKCOL https://webdav.yandex.ru/CAM/$DD_new
                curl -v -k -u $email:$pass -X MKCOL https://webdav.yandex.ru/CAM/$DD_new/TIME
                curl -v -k -u $email:$pass -X MKCOL https://webdav.yandex.ru/CAM/$DD_new/TIME/VIDEO
        fi
else
				curl -v --stderr /tmp/flg_time -k -u $email:$pass -X MKCOL https://webdav.yandex.ru/CAM
				curl -v -k -u $email:$pass -X MKCOL https://webdav.yandex.ru/CAM/$DD_new
				curl -v -k -u $email:$pass -X MKCOL https://webdav.yandex.ru/CAM/$DD_new/TIME
				curl -v -k -u $email:$pass -X MKCOL https://webdav.yandex.ru/CAM/$DD_new/TIME/VIDEO
				if [ $(cat /tmp/flg_time | grep -c created) -eq 1 ];then
					echo $DD_new > /tmp/old1.txt
				fi
fi

#echo $DD_new > /tmp/old1.txt
curl -v -m 180 --stderr /tmp/logi7 -k -u $email:$pass -T /tmp/ya_time/$B https://webdav.yandex.ru/CAM/$DD_new/TIME/VIDEO/
rm /tmp/ya_time/$B
fi
done
