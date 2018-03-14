#!/bin/sh 
#===================== 
#rm old log
#===================== 

log_path=`dirname $(pwd)`/logs

expiration_day=7

currentDate=`date +%s`
echo "current date: " $currentDate
for file in `find $log_path -name "*.log"`
do
	name=$file
	modifyDate=$(stat -c %Y $file)
	logExistTime=$(($currentDate - $modifyDate))
	logExistTime=$(($logExistTime/86400))
	if [ $logExistTime -gt $expiration_day ]; then
		echo "File: "$name"\tModifyDate:"$(stat -c %y $file)"\tExistTime:"$logExistTime"days\tDelete:yes"
		rm -f $file
	else
		echo "File: "$name"\tModifyDate:"$(stat -c %y $file)"\tExistTime:"$logExistTime"days\tDelete:no"
	fi
done
