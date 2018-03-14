#!/bin/sh 
#===================== 
#CHARGE  
#===================== 

FULLPATH=$(cd "$(dirname "$0")"; pwd)
# echo "full path to currently executed script is : ${FULLPATH}"

SETSERIAL_SH_FILE=${FULLPATH}/setserial.sh
CHARGE_FILE=${FULLPATH}/charge

cd ${FULLPATH}

chmod a+x ${SETSERIAL_SH_FILE}
${SETSERIAL_SH_FILE}
sleep 3 
while : 
do 
    echo "current charge dir is " ${FULLPATH}
    stillRunning=$(ps -ef |grep ${CHARGE_FILE} |grep -v "grep") 
    if [ "$stillRunning"  ] ; then 
        echo "charge service was already started by another way" 
        echo "kill it and then startup by this shell, other wise this shell will loop out this message annoyingly" 
        kill -9 $(pidof ${CHARGE_FILE})
    else 
        echo "charge service was not started" 
        echo "starting charge service ..." 
        chmod a+x ${CHARGE_FILE}
        export LD_LIBRARY_PATH=${FULLPATH}:$LD_LIBRARY_PATH
        ${CHARGE_FILE}
        echo "charge service was exited!" 
    fi 
    sleep 10 
done 
