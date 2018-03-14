#!/bin/bash
#===================== 
#USB_MONITOR
#===================== 
BASEDIR=`dirname $(pwd)`

touch_log_file(){
	LOG_NAME="lsusb_"$(date +%Y-%m-%d)".log"
	ABSOLUTE_LOG_NAME=$BASEDIR/logs/$LOG_NAME
	if [ ! -f "$ABSOLUTE_LOG_NAME" ]; then 
		touch "$ABSOLUTE_LOG_NAME"
	fi
}

while : 
do 
	lsusb > /dev/null &
	sleep 5
    stillRunning=$(ps -ef |grep lsusb |grep -v "grep")
    if [ "$stillRunning"  ] ; then 
        echo "send 42 signal to charge, it will reset the board status" 
        kill -SIGRTMIN+8 $(pidof charge) #kill -l
		touch_log_file
		echo $(date +%Y-%m-%d_%T) >> $ABSOLUTE_LOG_NAME
    fi 
    sleep 60
done 
exit 0
