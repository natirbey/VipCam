#!/bin/sh
echo "Content-type: text/html"
echo ""
# if [ "$REQUEST_METHOD" = "POST" ]; then
	# read POST_STRING
# fi
# TMP=$(uhttpd -d $POST_STRING)
# if [ $TMP -eq 1 ];then
	 echo "1" > /tmp/up
# else 
	# echo "0" > /tmp/up
# fi