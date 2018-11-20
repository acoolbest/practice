#database dump 
insert into Position (uuid,companyid,sort,created,updated) values('00000000000000000000000000000000',0,1,unix_timestamp(),unix_timestamp());

alter table IMUserA modify birthday int(11)  not null default 0;
alter table IMRecentSession add lastatmsgid int(11) unsigned;
alter table IMGroupMessage_0 add flag tinyint(1) unsigned not null default 0;
alter table IMGroupMessage_1 add flag tinyint(1) unsigned not null default 0;
alter table IMGroupMessage_2 add flag tinyint(1) unsigned not null default 0;
alter table IMGroupMessage_3 add flag tinyint(1) unsigned not null default 0;
alter table IMGroupMessage_4 add flag tinyint(1) unsigned not null default 0;
alter table IMGroupMessage_5 add flag tinyint(1) unsigned not null default 0;
alter table IMGroupMessage_6 add flag tinyint(1) unsigned not null default 0;
alter table IMGroupMessage_7 add flag tinyint(1) unsigned not null default 0;
alter table IMMessage_0 add flag tinyint(1) unsigned not null default 0;
alter table IMMessage_1 add flag tinyint(1) unsigned not null default 0;
alter table IMMessage_2 add flag tinyint(1) unsigned not null default 0;
alter table IMMessage_3 add flag tinyint(1) unsigned not null default 0;
alter table IMMessage_4 add flag tinyint(1) unsigned not null default 0;
alter table IMMessage_5 add flag tinyint(1) unsigned not null default 0;
alter table IMMessage_6 add flag tinyint(1) unsigned not null default 0;
alter table IMMessage_7 add flag tinyint(1) unsigned not null default 0;
alter table IMUserA add isinitpassword tinyint(1) unsigned not null default 0;
alter table IMUserA drop tmppassword;
update IMGroup set groupid=replace(uuid(),'-',''),updated=unix_timestamp() where groupid is NULL;
alter table IMGroup modify groupid varchar(36);

DROP TABLE IF EXISTS IMTransmitFileA;
CREATE TABLE IMTransmitFileA
(
	id int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '主键id',
	fileid varchar(36) NOT NULL DEFAULT '' COMMENT '文件uuid',
	filename varchar(255) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT'文件名',
	filesize int(11) unsigned NOT NULL DEFAULT 0 COMMENT '文件大小',
	userid int(11) unsigned NOT NULL COMMENT '文件上传者',
	peerid int(11) unsigned NOT NULL COMMENT '文件接收方(1：个人；2:群组)',
	peertype tinyint(1) unsigned NOT NULL DEFAULT 1 COMMENT '文件接收类别(1：个人；2:群组)',
	filecreatedtime int(11) unsigned NOT NULL DEFAULT 0 COMMENT '文件创建时间',
	validperiod tinyint(3) unsigned NOT NULL DEFAULT 7 COMMENT '文件有效期',
	status int(1) NOT NULL DEFAULT 0 COMMENT '记录状态(0:正常；1:删除)',
	created int(11) unsigned NOT NULL DEFAULT 0 COMMENT '记录生成时间',
	updated int(11) unsigned NOT NULL DEFAULT 0 COMMENT '更新时间',
	PRIMARY KEY (id),
	KEY idx_IMTransmitFileA_userid(userid),
	KEY idx_IMTransmitFileA_peerid(peerid),
	KEY idx_IMTransmitFileA_created(created),
	KEY idx_IMTransmitFileA_peertype(peertype),
	KEY idx_IMtransmitFileA_updated(updated)
)ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='IM离线文件表';


DROP TABLE IF EXISTS IMDownloadFileList;
CREATE TABLE IMDownloadFileList
(
	id int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '主键id',
	fileid varchar(36) NOT NULL DEFAULT '' COMMENT '文件uuid',
	userid int(11) unsigned NOT NULL COMMENT '下载用户id',
	created int(11) unsigned NOT NULL DEFAULT 0 COMMENT '下载时间',
	PRIMARY KEY(id),
	KEY idx_IMDownloadFileList_fileid(fileid),
	KEY idx_IMDownloadFileList_userid(userid),
	KEY idx_IMDownloadFileList_created(created)
)ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='IM离线文件下载列表';


DROP TABLE IF EXISTS IMTransmitFileStatistics;
CREATE TABLE IMTransmitFileStatistics
(
	id int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '主键id',
	fileid varchar(36) NOT NULL DEFAULT '' COMMENT '文件uuid',
	downloads smallint(5) unsigned NOT NULL DEFAULT 1 COMMENT '下载次数',
	created int(11) unsigned NOT NULL DEFAULT 0 COMMENT '创建时间',
	updated int(11) unsigned NOT NULL DEFAULT 0 COMMENT '更新时间',
	PRIMARY KEY(id),
	KEY idx_IMTransmitFileStatistics_fileid(fileid)
)ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='IM离线文件下载统计表';


#update db on June 1st
ALTER TABLE IMCompany ADD validitystatus tinyint(1) NOT NULL DEFAULT 0 COMMENT '开启有效期状态';
ALTER TABLE IMCompany ADD validity int(3) unsigned  NOT NULL DEFAULT 30 COMMENT '有效期(天)';
ALTER TABLE IMCompany ADD isforcepassword tinyint(1) NOT NULL DEFAULT 0 COMMENT '是否强制修改密码';
ALTER TABLE IMCompany ADD expireddate int(11) unsigned NOT NULL DEFAULT 0 COMMENT '域名有效期';
