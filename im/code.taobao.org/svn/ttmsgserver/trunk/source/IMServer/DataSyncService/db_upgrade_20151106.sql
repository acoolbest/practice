#add fields for tables such as IMUser,IMUserTemp
alter table IMUser add  `alias` varchar(512) COLLATE utf8mb4_bin DEFAULT '' COMMENT 'alias';
alter table IMUser add  `buyproduct` varchar(512) COLLATE utf8mb4_bin DEFAULT '' COMMENT 'buyproduct';
alter table IMUserTemp add  `alias` varchar(512) COLLATE utf8mb4_bin DEFAULT '' COMMENT 'alias';
alter table IMUserTemp add  `buyproduct` varchar(512) COLLATE utf8mb4_bin DEFAULT '' COMMENT 'buyproduct';

#alter auto_increment for IMUser
#select auto_increment from information_schema.tables where table_name='IMUser';
alter table IMUser auto_increment = 10792;

#update id for table 'IMUser'
update IMUser set id=10000+id;

#update IMUserPosition
update IMUserPosition set userid=userid+10000;

#update IMGroup
update IMGroup set creator=10000+creator;

#update IMGroupMember
update IMGroupMember set userid=userid+10000;

#update IMRelationShip;
update IMRelationShip set smallid=smallid+10000 where smallid<>0;
update IMRelationShip set bigid=bigid+10000;

#update IMRecentSession
update IMRecentSession set userid=userid+10000 where userid<>0;
update IMRecentSession set peerid=peerid+10000 where peerid<>0 and type = 1;

#update IMMessage
update IMMessage_0 set toid=toid+10000;
update IMMessage_0 set fromid=fromid+10000 where fromid<>0;
update IMMessage_1 set toid=toid+10000;
update IMMessage_1 set fromid=fromid+10000 where fromid<>0;
update IMMessage_2 set toid=toid+10000;
update IMMessage_2 set fromid=fromid+10000 where fromid<>0;
update IMMessage_3 set toid=toid+10000;
update IMMessage_3 set fromid=fromid+10000 where fromid<>0;
update IMMessage_4 set toid=toid+10000;
update IMMessage_4 set fromid=fromid+10000 where fromid<>0;
update IMMessage_5 set toid=toid+10000;
update IMMessage_5 set fromid=fromid+10000 where fromid<>0;
update IMMessage_6 set toid=toid+10000;
update IMMessage_6 set fromid=fromid+10000 where fromid<>0;
update IMMessage_7 set toid=toid+10000;
update IMMessage_7 set fromid=fromid+10000 where fromid<>0;

#update group message
update IMGroupMessage_0 set userid=userid+10000 where userid<>0;
update IMGroupMessage_1 set userid=userid+10000 where userid<>0;
update IMGroupMessage_2 set userid=userid+10000 where userid<>0;
update IMGroupMessage_3 set userid=userid+10000 where userid<>0;
update IMGroupMessage_4 set userid=userid+10000 where userid<>0;
update IMGroupMessage_5 set userid=userid+10000 where userid<>0;
update IMGroupMessage_6 set userid=userid+10000 where userid<>0;
update IMGroupMessage_7 set userid=userid+10000 where userid<>0;

#update IMAudio
update IMAudio set fromid=fromid+10000,toid=toid+10000;

#update IMTransmitFile
update IMTransmitFile set userid=userid+10000,touserid=touserid+10000;

#add a user named'workflow'
insert into IMUser(id,userid,avatar,status,created,updated) values(1,'workflow','http://res.im.ioa.cn:12380/data/head/workflow_head_1.jpg',0,unix_timestamp(), unix_timestamp());