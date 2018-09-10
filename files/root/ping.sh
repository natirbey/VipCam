#S=$(ping -c1 -w1 ya.ru | awk 'NR==2 {if ($2=="bytes"){print "0"}}')
S=$(ping -c1 -w1 ya.ru | awk 'NR==1 {if ($2=="bad"){print "1"}}')
echo $S
