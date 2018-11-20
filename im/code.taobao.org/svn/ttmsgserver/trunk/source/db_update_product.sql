##database dump 
#insert into Position (uuid,companyid,sort,created,updated) values('00000000000000000000000000000000',0,1,unix_timestamp(),unix_timestamp());

#upgrade on June 13th,2016
#alter table IMUserA modify birthday int(11)  not null default 0;
#alter table IMRecentSession add lastatmsgid int(11) unsigned;
#alter table IMGroupMessage_0 add flag tinyint(1) unsigned not null default 0;
#alter table IMGroupMessage_1 add flag tinyint(1) unsigned not null default 0;
#alter table IMGroupMessage_2 add flag tinyint(1) unsigned not null default 0;
#alter table IMGroupMessage_3 add flag tinyint(1) unsigned not null default 0;
#alter table IMGroupMessage_4 add flag tinyint(1) unsigned not null default 0;
#alter table IMGroupMessage_5 add flag tinyint(1) unsigned not null default 0;
#alter table IMGroupMessage_6 add flag tinyint(1) unsigned not null default 0;
#alter table IMGroupMessage_7 add flag tinyint(1) unsigned not null default 0;
#alter table IMMessage_0 add flag tinyint(1) unsigned not null default 0;
#alter table IMMessage_1 add flag tinyint(1) unsigned not null default 0;
#alter table IMMessage_2 add flag tinyint(1) unsigned not null default 0;
#alter table IMMessage_3 add flag tinyint(1) unsigned not null default 0;
#alter table IMMessage_4 add flag tinyint(1) unsigned not null default 0;
#alter table IMMessage_5 add flag tinyint(1) unsigned not null default 0;
#alter table IMMessage_6 add flag tinyint(1) unsigned not null default 0;
#alter table IMMessage_7 add flag tinyint(1) unsigned not null default 0;
#alter table IMUserA add isinitpassword tinyint(1) unsigned not null default 0;
#alter table IMUserA drop tmppassword;
#update IMGroup set groupid=replace(uuid(),'-',''),updated=unix_timestamp() where groupid is NULL;
#alter table IMGroup modify groupid varchar(36);
#
#DROP TABLE IF EXISTS IMTransmitFileA;
#CREATE TABLE IMTransmitFileA
#(
#	id int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '主键id',
#	fileid varchar(36) NOT NULL DEFAULT '' COMMENT '文件uuid',
#	filename varchar(255) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT'文件名',
#	filesize int(11) unsigned NOT NULL DEFAULT 0 COMMENT '文件大小',
#	userid int(11) unsigned NOT NULL COMMENT '文件上传者',
#	peerid int(11) unsigned NOT NULL COMMENT '文件接收方(1：个人；2:群组)',
#	peertype tinyint(1) unsigned NOT NULL DEFAULT 1 COMMENT '文件接收类别(1：个人；2:群组)',
#	filecreatedtime int(11) unsigned NOT NULL DEFAULT 0 COMMENT '文件创建时间',
#	validperiod tinyint(3) unsigned NOT NULL DEFAULT 7 COMMENT '文件有效期',
#	status int(1) NOT NULL DEFAULT 0 COMMENT '记录状态(0:正常；1:删除)',
#	created int(11) unsigned NOT NULL DEFAULT 0 COMMENT '记录生成时间',
#	updated int(11) unsigned NOT NULL DEFAULT 0 COMMENT '更新时间',
#	PRIMARY KEY (id),
#	KEY idx_IMTransmitFileA_userid(userid),
#	KEY idx_IMTransmitFileA_peerid(peerid),
#	KEY idx_IMTransmitFileA_created(created),
#	KEY idx_IMTransmitFileA_peertype(peertype),
#	KEY idx_IMtransmitFileA_updated(updated)
#)ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='IM离线文件表';
#
#
#DROP TABLE IF EXISTS IMDownloadFileList;
#CREATE TABLE IMDownloadFileList
#(
#	id int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '主键id',
#	fileid varchar(36) NOT NULL DEFAULT '' COMMENT '文件uuid',
#	userid int(11) unsigned NOT NULL COMMENT '下载用户id',
#	created int(11) unsigned NOT NULL DEFAULT 0 COMMENT '下载时间',
#	PRIMARY KEY(id),
#	KEY idx_IMDownloadFileList_fileid(fileid),
#	KEY idx_IMDownloadFileList_userid(userid),
#	KEY idx_IMDownloadFileList_created(created)
#)ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='IM离线文件下载列表';
#
#
#DROP TABLE IF EXISTS IMTransmitFileStatistics;
#CREATE TABLE IMTransmitFileStatistics
#(
#	id int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '主键id',
#	fileid varchar(36) NOT NULL DEFAULT '' COMMENT '文件uuid',
#	downloads smallint(5) unsigned NOT NULL DEFAULT 1 COMMENT '下载次数',
#	created int(11) unsigned NOT NULL DEFAULT 0 COMMENT '创建时间',
#	updated int(11) unsigned NOT NULL DEFAULT 0 COMMENT '更新时间',
#	PRIMARY KEY(id),
#	KEY idx_IMTransmitFileStatistics_fileid(fileid)
#)ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='IM离线文件下载统计表';
#
#
##update db on June 1st
#ALTER TABLE IMCompany ADD validitystatus tinyint(1) NOT NULL DEFAULT 0 COMMENT '开启有效期状态';
#ALTER TABLE IMCompany ADD validity int(3) unsigned  NOT NULL DEFAULT 30 COMMENT '有效期(天)';
#ALTER TABLE IMCompany ADD isforcepassword tinyint(1) NOT NULL DEFAULT 0 COMMENT '是否强制修改密码';
#ALTER TABLE IMCompany ADD expireddate int(11) unsigned NOT NULL DEFAULT 0 COMMENT '域名有效期';

#update db on June 30th
#自由注册数据库表变更:

#用户群组表修改公司值
update IMGroup a, IMUserA b set a.companyid=b.companyid where a.creator=b.id and a.companyid=0;

#删除原表
DROP TABLE IF EXISTS IMUser;
DROP TABLE IF EXISTS IMDepart;
DROP TABLE IF EXISTS IMUserPosition;
DROP TABLE IF EXISTS IMTransmitFile;

#修改表名
RENAME TABLE IMUserA TO IMUser;
RENAME TABLE IMDepartA TO IMDepart;
RENAME TABLE IMUserPositionA TO IMUserPosition;
RENAME TABLE Position TO IMPosition;
RENAME TABLE IMTransmitFileA TO IMTransmitFile;

#机构表
ALTER TABLE IMCompany CHANGE Id id int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id';
ALTER TABLE IMCompany CHANGE domainid companyuuid varchar(40) NOT NULL COMMENT '公司uuid';
ALTER TABLE IMCompany DROP domainname;
ALTER TABLE IMCompany ADD companyno varchar(10) NOT NULL DEFAULT '' COMMENT '公司编号';
ALTER TABLE IMCompany CHANGE companyName companyname varchar(100) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '公司名称简称';
ALTER TABLE IMCompany ADD companyfullname varchar(100) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '公司全名';
ALTER TABLE IMCompany ADD industrytype varchar(10) COLLATE utf8mb4_bin  NOT NULL DEFAULT '' COMMENT '行业类别';
ALTER TABLE IMCompany ADD isjoined tinyint(1) unsigned NOT NULL DEFAULT 1 COMMENT '是否允许加入该组织';
ALTER TABLE IMCompany ADD isinvite tinyint(1) unsigned NOT NULL DEFAULT 1 COMMENT '是否邀请别人加入';
ALTER TABLE IMCompany ADD passwordstrength tinyint(1) unsigned NOT NULL DEFAULT 1 COMMENT '密码强度';
ALTER TABLE IMCompany ADD passwordlength tinyint(1) unsigned NOT NULL DEFAULT 8 COMMENT '密码长度';


CREATE INDEX idx_IMCompany_companyuuid ON IMCompany(companyuuid);	
DROP INDEX idx_IMCompany_domainid ON IMCompany;
#ALTER TABLE IMCompany ADD companytype tinyint(1) NOT NULL DEFAULT 0,
#ALTER TABLE IMCompany ADD conactname varchar(50),
#ALTER TABLE IMCompany ADD email varchar(100),
#ALTER TABLE IMCompany ADD telephone varchar(30),
#ALTER TABLE IMCompany ADD mobile varchar(30),
#ALTER TABLE IMCompany ADD address varchar(255),
#ALTER TABLE IMCompany ADD fax varchar(30)
#ALTER TABLE IMCompany ADD postcode varchar(20),
#ALTER TABLE IMCompany ADD officialsite varchar(200)

#IMUser表变更
ALTER TABLE IMUser DROP jobnumber;
ALTER TABLE IMUser DROP isadmin;
ALTER TABLE IMUser DROP adminstatus;
ALTER TABLE IMUser DROP contactstatus;
ALTER TABLE IMUser DROP companyid;
ALTER TABLE IMUser DROP isiplimit;
ALTER TABLE IMUser ADD email varchar(100) COMMENT '邮箱';
ALTER TABLE IMUser ADD isactived  tinyint(1) NOT NULL DEFAULT 0;
ALTER TABLE IMUser CHANGE lastmodifytime lastmodifiedtime int(11) unsigned NOT NULL DEFAULT 0;
ALTER TABLE IMUser ADD salt varchar(50) NOT NULL DEFAULT '' COMMENT '盐值';
ALTER TABLE IMUser MODIFY username varchar(50) COMMENT '用户名';
ALTER TABLE IMUser MODIFY name varchar(50) COLLATE utf8mb4_bin COMMENT '姓名';
ALTER TABLE IMUser ADD account varchar(100) COMMENT '企业帐号';
#ALTER TABLE IMUser add syncstatus tinyint(1) unsigned NOT NULL DEFAULT 0 COMMENT '用户同步状态(0:未同步；1:同步)';

CREATE INDEX idx_IMUser_phone ON IMUser(phone);
CREATE INDEX idx_IMUser_email ON IMUser(email);
CREATE INDEX idx_IMUser_account ON IMUser(account);

#修改IMUserPosition表
ALTER TABLE IMUserPosition ADD jobgrade varchar(20) COLLATE utf8mb4_bin COMMENT '职级';

#增加员工表
DROP TABLE IF EXISTS IMStaff;
CREATE TABLE IMStaff
(
	uuid varchar(40) NOT NULL COMMENT '员工uuid',
	useruuid varchar(40) NOT NULL DEFAULT '' COMMENT'用户uuid',
	userid int(11) unsigned NOT NULL DEFAULT 0 COMMENT '用户id',
	jobnumber varchar(20) COMMENT '工号',
	mobilephone varchar(32) COMMENT '手机号码',
	staffname varchar(50) COLLATE utf8mb4_bin COMMENT '员工姓名',
	isadmin tinyint(1) unsigned NOT NULL DEFAULT 0 COMMENT '是否为管理员(0:否，1:是)',
	adminstatus tinyint(1) unsigned NOT NULL DEFAULT 0 COMMENT '管理员状态(0:禁用，1:启用)',
	issuperadmin tinyint(1) unsigned NOT NULL DEFAULT 0 COMMENT '是否超级管理员(0:否，1:是)',	
	isactived tinyint(1) unsigned NOT NULL DEFAULT 0 COMMENT '是否激活(0:否，1:是)',
	contactstatus tinyint(1) unsigned NOT NULL DEFAULT 1 COMMENT '通讯录状态(0:不显示，1:显示)',
	status tinyint(2) NOT NULL DEFAULT 1 COMMENT '用户状态(-1:删除，0:禁用，1:启用)',
	companyid int(11) unsigned NOT NULL DEFAULT 0 COMMENT '公司id',
	keyword varchar(200) NOT NULL DEFAULT '' COMMENT '关键字',
	remark varchar(500) COLLATE utf8mb4_bin COMMENT '备注',
	buyproduct varchar(256) COMMENT '订购产品',
	email varchar(100) COMMENT '邮箱',
	username varchar(50) COMMENT '用户名',
	account varchar(100) COMMENT '企业帐号',
	defaultemail varchar(100) COMMENT '默认邮箱',
	created int(11) unsigned NOT NULL DEFAULT 0 COMMENT '创建日期',
	updated int(11) unsigned NOT NULL DEFAULT 0 COMMENT '修改日期',
	PRIMARY KEY (uuid),
	KEY idx_IMStaff_companyid (companyid),
	KEY idx_IMStaff_userid (userid),
	KEY idx_IMStaff_useruuid(useruuid),
	KEY idx_IMStaff_updated (updated),
	KEY idx_IMStaff_status (status)
)ENGINE=MyISAM DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='员工表';

#增加员工拓展信息表
DROP TABLE IF EXISTS StaffDetails;
CREATE TABLE StaffDetails
 (
	uuid varchar(40)  NOT NULL  COMMENT '用户详情uuid',
  staffuuid varchar(40) NOT NULL DEFAULT '' COMMENT '员工uuid',
  type int(3)  NOT NULL DEFAULT 0 COMMENT '详情信息类型',
  title varchar(40) COLLATE utf8mb4_bin NOT NULL COMMENT '详情信息标题',
  content varchar(1024) COLLATE utf8mb4_bin NOT NULL COMMENT '详情信息内容',
  created int(10) unsigned NOT NULL DEFAULT '0' COMMENT '创建时间',
  updated int(10) unsigned NOT NULL DEFAULT '0' COMMENT '更新时间',
  PRIMARY KEY (uuid),
  KEY `idx_StaffDetails_staffuuid` (`staffuuid`),
  KEY `idx_StaffDetails_updated` (`updated`)
)ENGINE=MyISAM DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='员工拓展信息详情表';

#增加重要用户表
DROP TABLE IF EXISTS IMImportantPerson;
CREATE TABLE IMImportantPerson
(
	id int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
	userid int(11) unsigned NOT NULL DEFAULT 0 COMMENT '用户id',
	vipid int(11) unsigned NOT NULL DEFAULT 0 COMMENT '用户的vip id',
	status tinyint(1) unsigned NOT NULL DEFAULT 0 COMMENT '状态(0:正常；1:删除)',
	created int(11) unsigned NOT NULL DEFAULT 0 COMMENT '创建日期',
	updated int(11) unsigned NOT NULL DEFAULT 0 COMMENT '更新日期',
	PRIMARY KEY (id),
	KEY idx_IMImportantPerson_userid (userid),
	KEY idx_IMImportantPerson_status(status),
	KEY idx_IMImportantPerson_updated(updated)
)ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='员工重要用户表';

#增加员工拓展信息临时表
DROP TABLE IF EXISTS StaffDetailsTemp;
CREATE TABLE StaffDetailsTemp
 (
	uuid varchar(40)  NOT NULL  COMMENT '用户详情uuid',
  staffuuid varchar(40) NOT NULL DEFAULT '' COMMENT '员工uuid',
  type int(3)  NOT NULL DEFAULT 0 COMMENT '详情信息类型',
  title varchar(40) COLLATE utf8mb4_bin NOT NULL COMMENT '详情信息标题',
  content varchar(1024) COLLATE utf8mb4_bin NOT NULL COMMENT '详情信息内容',
  created int(10) unsigned NOT NULL DEFAULT '0' COMMENT '创建时间',
  updated int(10) unsigned NOT NULL DEFAULT '0' COMMENT '更新时间',
  PRIMARY KEY (uuid),
  KEY `idx_StaffDetails_staffuuid` (`staffuuid`),
  KEY `idx_StaffDetails_updated` (`updated`)
)ENGINE=MyISAM DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='员工拓展信息详情表';

#增加重要用户表
DROP TABLE IF EXISTS IMImportantPersonTemp;
CREATE TABLE IMImportantPersonTemp
(
	id int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
	userid int(11) unsigned NOT NULL DEFAULT 0 COMMENT '用户id',
	vipid int(11) unsigned NOT NULL DEFAULT 0 COMMENT '用户的vip id',
	status tinyint(1) unsigned NOT NULL DEFAULT 0 COMMENT '状态(0:正常；1:删除)',
	created int(11) unsigned NOT NULL DEFAULT 0 COMMENT '创建日期',
	updated int(11) unsigned NOT NULL DEFAULT 0 COMMENT '更新日期',
	PRIMARY KEY (id),
	KEY idx_IMImportantPersonTemp_userid (userid),
	KEY idx_IMImportantPersonTemp_status(status),
	KEY idx_IMImportantPersonTemp_updated(updated)
)ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='员工重要用户表';


#IP限制表
DROP TABLE IF EXISTS IMUserIpLimit;
CREATE TABLE IMUserIpLimit
(
	uuid varchar(40)  NOT NULL  COMMENT 'IP限制uuid',
  useruuid varchar(40) NOT NULL DEFAULT '' COMMENT '用户uuid',
  limitip varchar(50) COMMENT '限制IP',
  created int(10) unsigned NOT NULL DEFAULT '0' COMMENT '创建时间',
  updated int(10) unsigned NOT NULL DEFAULT '0' COMMENT '更新时间',
  PRIMARY KEY (uuid),
  KEY `idx_IMUserIpLimit_useruuid` (`useruuid`),
  KEY `idx_IMUserIpLimit_updated` (`updated`)
)ENGINE=MyISAM DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='IP限制详情表';

#别名表
ALTER TABLE UserAlias DROP username;