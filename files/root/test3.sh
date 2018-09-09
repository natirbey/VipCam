#!/bin/sh
while :
do
A=$(ps | grep openRTSP | wc -l)
if [ $A -ge 3 ];then
echo $A
echo $A >> /root/tmp
fi
sleep 1
done
