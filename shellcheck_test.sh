#!/bin/sh 
#set -e
#set -x
#===================== 
#CHARGE  
#===================== 
FULLPATH=$(cd "$(dirname "$0")"; pwd)
BASEDIR=$(dirname "$(dirname "$FULLPATH")")

XYDIR=${BASEDIR}/xy/application
XY_FILE=${XYDIR}/xyinstaller
XY_CTRL=${FULLPATH}/config.json

read_xy_conf(){
	read_from_config=$(awk '/xyCtrl/{print $3}' "${XY_CTRL}")
	read_from_config=${read_from_config#*\"}
	read_from_config=${read_from_config%%\"*}

	if [ "$read_from_config" ]; then
		if [ "$read_from_config" -eq "0" ]; then
			ret=0
		else
			ret=1
		fi
	else
		ret=1
	fi
	return $ret
}

cd "${XYDIR}"

if [ "$(pgrep -f "${XY_FILE}")" ] ; then
	kill -9 "$(pidof "${XY_FILE}")"
fi

while : 
do
    if [ "$(pgrep -f "${XY_FILE}")" ] ; then
		if read_xy_conf ; then
			kill -9 "$(pidof "${XY_FILE}")"
		fi
    else 
		read_xy_conf
		if [ $? -eq "1" ] ; then
			chmod a+x "${XY_FILE}"
			export LD_LIBRARY_PATH=${XYDIR}:${FULLPATH}:$LD_LIBRARY_PATH
			${XY_FILE} &
		fi
    fi
    sleep 10
done
