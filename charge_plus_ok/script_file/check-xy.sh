#!/bin/sh 
#===================== 
#CHARGE  
#===================== 

FULLPATH=$(cd "$(dirname "$0")"; pwd)
#echo "full path to currently executed script is : ${FULLPATH}"

BASEDIR=$(dirname "$(dirname $FULLPATH)")

XYDIR=${BASEDIR}/xy/application
XY_FILE=${XYDIR}/xyinstaller
XY_CTRL=${FULLPATH}/config.json

read_xy_conf(){
	read_from_config=$(awk '/xyCtrl/{print $3}' ${XY_CTRL})
	read_from_config=${read_from_config#*\"}
	read_from_config=${read_from_config%%\"*}
#	echo $read_from_config
	if [ "$read_from_config" ]; then
		if [ "$read_from_config" -eq "1" ]; then
			ret=1
		else
			ret=0
		fi
	else
		ret=1
	fi
	return $ret
}

cd ${XYDIR}

echo "current xy dir is " ${XYDIR}
stillRunning=$(ps -ef |grep ${XY_FILE} |grep -v "grep")
if [ "$stillRunning"  ] ; then
	echo "xy service was already started by another way" 
	echo "kill it and then startup by this shell, other wise this shell will loop out this message annoyingly" 
	kill -9 $(pidof ${XY_FILE})
fi

while : 
do 
#    echo "current xy dir is " ${XYDIR}
    stillRunning=$(ps -ef |grep ${XY_FILE} |grep -v "grep") 
    if [ "$stillRunning"  ] ; then 
		read_xy_conf
		if [ $? -eq "0" ] ; then
        echo "xy no need to run" 
        kill -9 $(pidof ${XY_FILE})
		fi
    else 
#        echo "xy service was not started" 
		read_xy_conf
		if [ $? -eq "1" ] ; then
			echo "starting xy service ..." 
			chmod a+x ${XY_FILE}
			export LD_LIBRARY_PATH=${XYDIR}:${FULLPATH}:$LD_LIBRARY_PATH
			${XY_FILE} &
			echo "xy service is running!" 
		fi
    fi 
    sleep 10
done 
