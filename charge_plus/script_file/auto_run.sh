#!/bin/sh 
#===================== 
#AUTO RUN SCRIPT
#===================== 

echo "auto run script is running"
FULLPATH=$(cd "$(dirname "$0")"; pwd)
# echo "full path to currently executed script is : ${FULLPATH}"
BASEDIR=`dirname $FULLPATH`
# echo "parent dir is $BASEDIR"

CHECK_SH_FILE=${FULLPATH}/check.sh
DISP_SH_FILE=${BASEDIR}/web/disp.sh
CHECK_XY_SH_FILE=${FULLPATH}/check-xy.sh
CHECK_UPDATE_FILE=${FULLPATH}/check_update.sh
USB_MONITOR_SH_FILE=${FULLPATH}/usb_monitor.sh

chmod a+x ${DISP_SH_FILE}
stillRunning=$(ps -ef |grep ${DISP_SH_FILE} |grep -v "grep") 
if [ -z "$stillRunning"  ] ; then 
	${DISP_SH_FILE} &
fi

chmod a+x ${CHECK_SH_FILE}
stillRunning=$(ps -ef |grep ${CHECK_SH_FILE} |grep -v "grep") 
if [ -z "$stillRunning"  ] ; then 
	${CHECK_SH_FILE} &
fi

chmod a+x ${CHECK_XY_SH_FILE}
stillRunning=$(ps -ef |grep ${CHECK_XY_SH_FILE} |grep -v "grep") 
if [ -z "$stillRunning"  ] ; then 
	${CHECK_XY_SH_FILE} &
fi

chmod a+x ${CHECK_UPDATE_FILE}
stillRunning=$(ps -ef |grep ${CHECK_UPDATE_FILE} |grep -v "grep") 
if [ -z "$stillRunning"  ] ; then 
	${CHECK_UPDATE_FILE} &
fi

chmod a+x ${USB_MONITOR_SH_FILE}
stillRunning=$(ps -ef |grep ${USB_MONITOR_SH_FILE} |grep -v "grep") 
if [ -z "$stillRunning"  ] ; then 
	${USB_MONITOR_SH_FILE} &
fi

echo "auto run script is end"
