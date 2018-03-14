#!/bin/bash 
#===================== 
#NET TRAFFIC
#===================== 
NET_DEV=/sys/class/net
NET_TRAFFIC=/proc/net/dev

FULLPATH=$(cd "$(dirname "$0")"; pwd)
# echo "full path to currently executed script is : ${FULLPATH}"
BASEDIR=`dirname $FULLPATH`
# echo "parent dir is $BASEDIR"
NET_TRAFFIC_JSON=${BASEDIR}/config/net_traffic.json
date
echo $NET_TRAFFIC_JSON
if [ ! -f "$NET_TRAFFIC_JSON" ]; then 
	echo "[ERROR] please touch the file of "$NET_TRAFFIC_JSON
	exit 0
fi 
net_new="netTrafficNew" #���ڴ洢��ĿǰΪֹδ���͵��������������ܺͣ����͵�������ʱ����charge����������
						#���㹫ʽΪ��δ���͵��������������ܺ� = δ���͵��������������ܺ� + ��ǰϵͳ������ - �ϴ�ϵͳ������
net_old="netTrafficOld" #���ڴ洢�ϴδ�ϵͳ�л�ȡ������������

traffic_record=0
ls ${NET_DEV} | ( while read line
	do
		#echo $line
		if [ "$line" != "lo" ] ; then
			ReceiveBytes=$(awk '/'$line'/{print $2}' ${NET_TRAFFIC})
			#echo $ReceiveBytes
			TransmitBytes=$(awk '/'$line'/{print $10}' ${NET_TRAFFIC})
			#echo $TransmitBytes
			traffic_record=$(($traffic_record+$ReceiveBytes+$TransmitBytes))
		fi
	done
	echo "net_traffic: "$traffic_record
	
	read_net_new=$(awk '/'$net_new'/{print $3}' ${NET_TRAFFIC_JSON})
	read_net_new=${read_net_new#*\"}
	read_net_new=${read_net_new%%\"*}
	echo "read net_new: "$read_net_new
	read_net_old=$(awk '/'$net_old'/{print $3}' ${NET_TRAFFIC_JSON})
	read_net_old=${read_net_old#*\"}
	read_net_old=${read_net_old%%\"*}
	echo "read net_old: "$read_net_old
	if [ $traffic_record -lt $read_net_old ] ; then
		read_net_new=$(($read_net_new+$traffic_record)) #�ոտ��������
		echo "record < old"
	elif [ $read_net_old -eq "0" ] ; then #�ող������ʱ����������������Ϊ0
		echo "old == 0"
	else
		read_net_new=$(($read_net_new+$traffic_record-$read_net_old))
		echo "normal"
	fi
	replace_string="\t\"$net_new\""" : ""\"$read_net_new\""","
	sed -i -e "s/$(awk '/'$net_new'/{print $0}' ${NET_TRAFFIC_JSON})/$replace_string/g" ${NET_TRAFFIC_JSON}
	
	replace_string="\t\"$net_old\""" : ""\"$traffic_record\""
	sed -i -e "s/$(awk '/'$net_old'/{print $0}' ${NET_TRAFFIC_JSON})/$replace_string/g" ${NET_TRAFFIC_JSON}
)
echo "-------------------------------------"
