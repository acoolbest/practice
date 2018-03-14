#!/bin/sh 
#===================== 
#CHARGE  
#===================== 

FULLPATH=$(cd "$(dirname "$0")"; pwd)
# echo "full path to currently executed script is : ${FULLPATH}"

CHARGE_FILE=${FULLPATH}/updateservice
CHARGE_EXE=updateservice
cd ${FULLPATH}

sleep 3 
while : 
do 
    echo "current charge dir is " ${FULLPATH}
    stillRunning=$(ps -ef |grep ${CHARGE_EXE} |grep -v "grep") 
    if [ "$stillRunning"  ] ; then 
        echo "update service was already started by another way" 
        echo "kill it and then startup by this shell, other wise this shell will loop out this message annoyingly" 
        kill -9 $(pidof ${CHARGE_EXE})
    else 
        echo "charge service was not started" 
        echo "starting charge service ..." 
        chmod a+x ${CHARGE_FILE}
        export LD_LIBRARY_PATH=${FULLPATH}:/usr/local/apr/lib:$LD_LIBRARY_PATH
        ${CHARGE_FILE}
        echo "update service was exited!" 
    fi 
    sleep 10 
done 
