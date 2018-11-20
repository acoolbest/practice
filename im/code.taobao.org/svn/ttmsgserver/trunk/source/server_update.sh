#!/bin/sh

#rename file name
currdate=$(date +%Y%m%d)
mv /usr/local/imserver/bin/db_proxy_server/db_proxy_server /usr/local/imserver/bin/db_proxy_server/db_proxy_server.$currdate.earlier
mv /usr/local/imserver/bin/http_msg_server/http_msg_server /usr/local/imserver/bin/http_msg_server/http_msg_server.$currdate.eariler
mv /usr/local/imserver/bin/msfs/msfs /usr/local/imserver/bin/msfs/msfs.$currdate.earlier
mv /usr/local/imserver/bin/push_server/push_server /usr/local/imserver/bin/push_server/push_server.$currdate.eariler
#mv /usr/local/imserver/bin/datasync/datasync /usr/local/imserver/bin/datasync/datasync.$currdate.earlier
mv /usr/local/imserver/bin/file_server/file_server /usr/local/imserver/bin/file_server/file_server.$currdate.earlier
mv /usr/local/imserver/bin/login_server/login_server /usr/local/imserver/bin/login_server/login_server.$currdate.earlier
mv /usr/local/imserver/bin/msg_server/msg_server /usr/local/imserver/bin/msg_server/msg_server.$currdate.earlier
mv /usr/local/imserver/bin/route_server/route_server /usr/local/imserver/bin/route_server/route_server.$currdate.earlier
mv /usr/local/imserver/bin/messagedispatch/messagedispatch /usr/local/imserver/bin/messagedispatch/messagedispatch.$currdate.eariler

#copy files
\cp db_proxy_server /usr/local/imserver/bin/db_proxy_server/db_proxy_server 
\cp http_msg_server /usr/local/imserver/bin/http_msg_server/http_msg_server 
\cp msfs /usr/local/imserver/bin/msfs/msfs 
\cp push_server /usr/local/imserver/bin/push_server/push_server
#\cp datasync /usr/local/imserver/bin/datasync/datasync
\cp file_server /usr/local/imserver/bin/file_server/file_server
\cp login_server /usr/local/imserver/bin/login_server/login_server
\cp msg_server /usr/local/imserver/bin/msg_server/msg_server
\cp route_server /usr/local/imserver/bin/route_server/route_server
#\cp datasync /usr/local/imserver/bin/datasync/datasync
\cp messagedispatch /usr/local/imserver/bin/messagedispatch/messagedispatch
#\cp entsync/* /usr/local/imserver/apps/sbin
#chmod a+x  /usr/local/imserver/apps/sbin/*.sh
