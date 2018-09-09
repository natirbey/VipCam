L=$(awk 'NR==2 {{lev=substr($1,3,10)}{print lev}}' /root/ini.ini)
R=$(awk 'NR==1' /etc/config/ipcam)
openRTSP -4 -b 200000 -d $L $R>/tmp/ya/cam/$1.mp4
