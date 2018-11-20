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
#	id int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '����id',
#	fileid varchar(36) NOT NULL DEFAULT '' COMMENT '�ļ�uuid',
#	filename varchar(255) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT'�ļ���',
#	filesize int(11) unsigned NOT NULL DEFAULT 0 COMMENT '�ļ���С',
#	userid int(11) unsigned NOT NULL COMMENT '�ļ��ϴ���',
#	peerid int(11) unsigned NOT NULL COMMENT '�ļ����շ�(1�����ˣ�2:Ⱥ��)',
#	peertype tinyint(1) unsigned NOT NULL DEFAULT 1 COMMENT '�ļ��������(1�����ˣ�2:Ⱥ��)',
#	filecreatedtime int(11) unsigned NOT NULL DEFAULT 0 COMMENT '�ļ�����ʱ��',
#	validperiod tinyint(3) unsigned NOT NULL DEFAULT 7 COMMENT '�ļ���Ч��',
#	status int(1) NOT NULL DEFAULT 0 COMMENT '��¼״̬(0:������1:ɾ��)',
#	created int(11) unsigned NOT NULL DEFAULT 0 COMMENT '��¼����ʱ��',
#	updated int(11) unsigned NOT NULL DEFAULT 0 COMMENT '����ʱ��',
#	PRIMARY KEY (id),
#	KEY idx_IMTransmitFileA_userid(userid),
#	KEY idx_IMTransmitFileA_peerid(peerid),
#	KEY idx_IMTransmitFileA_created(created),
#	KEY idx_IMTransmitFileA_peertype(peertype),
#	KEY idx_IMtransmitFileA_updated(updated)
#)ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='IM�����ļ���';
#
#
#DROP TABLE IF EXISTS IMDownloadFileList;
#CREATE TABLE IMDownloadFileList
#(
#	id int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '����id',
#	fileid varchar(36) NOT NULL DEFAULT '' COMMENT '�ļ�uuid',
#	userid int(11) unsigned NOT NULL COMMENT '�����û�id',
#	created int(11) unsigned NOT NULL DEFAULT 0 COMMENT '����ʱ��',
#	PRIMARY KEY(id),
#	KEY idx_IMDownloadFileList_fileid(fileid),
#	KEY idx_IMDownloadFileList_userid(userid),
#	KEY idx_IMDownloadFileList_created(created)
#)ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='IM�����ļ������б�';
#
#
#DROP TABLE IF EXISTS IMTransmitFileStatistics;
#CREATE TABLE IMTransmitFileStatistics
#(
#	id int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '����id',
#	fileid varchar(36) NOT NULL DEFAULT '' COMMENT '�ļ�uuid',
#	downloads smallint(5) unsigned NOT NULL DEFAULT 1 COMMENT '���ش���',
#	created int(11) unsigned NOT NULL DEFAULT 0 COMMENT '����ʱ��',
#	updated int(11) unsigned NOT NULL DEFAULT 0 COMMENT '����ʱ��',
#	PRIMARY KEY(id),
#	KEY idx_IMTransmitFileStatistics_fileid(fileid)
#)ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='IM�����ļ�����ͳ�Ʊ�';
#
#
##update db on June 1st
#ALTER TABLE IMCompany ADD validitystatus tinyint(1) NOT NULL DEFAULT 0 COMMENT '������Ч��״̬';
#ALTER TABLE IMCompany ADD validity int(3) unsigned  NOT NULL DEFAULT 30 COMMENT '��Ч��(��)';
#ALTER TABLE IMCompany ADD isforcepassword tinyint(1) NOT NULL DEFAULT 0 COMMENT '�Ƿ�ǿ���޸�����';
#ALTER TABLE IMCompany ADD expireddate int(11) unsigned NOT NULL DEFAULT 0 COMMENT '������Ч��';

#update db on June 30th
#����ע�����ݿ����:

#�û�Ⱥ����޸Ĺ�˾ֵ
update IMGroup a, IMUserA b set a.companyid=b.companyid where a.creator=b.id and a.companyid=0;

#ɾ��ԭ��
DROP TABLE IF EXISTS IMUser;
DROP TABLE IF EXISTS IMDepart;
DROP TABLE IF EXISTS IMUserPosition;
DROP TABLE IF EXISTS IMTransmitFile;

#�޸ı���
RENAME TABLE IMUserA TO IMUser;
RENAME TABLE IMDepartA TO IMDepart;
RENAME TABLE IMUserPositionA TO IMUserPosition;
RENAME TABLE Position TO IMPosition;
RENAME TABLE IMTransmitFileA TO IMTransmitFile;

#������
ALTER TABLE IMCompany CHANGE Id id int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id';
ALTER TABLE IMCompany CHANGE domainid companyuuid varchar(40) NOT NULL COMMENT '��˾uuid';
ALTER TABLE IMCompany DROP domainname;
ALTER TABLE IMCompany ADD companyno varchar(10) NOT NULL DEFAULT '' COMMENT '��˾���';
ALTER TABLE IMCompany CHANGE companyName companyname varchar(100) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '��˾���Ƽ��';
ALTER TABLE IMCompany ADD companyfullname varchar(100) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '��˾ȫ��';
ALTER TABLE IMCompany ADD industrytype varchar(10) COLLATE utf8mb4_bin  NOT NULL DEFAULT '' COMMENT '��ҵ���';
ALTER TABLE IMCompany ADD isjoined tinyint(1) unsigned NOT NULL DEFAULT 1 COMMENT '�Ƿ�����������֯';
ALTER TABLE IMCompany ADD isinvite tinyint(1) unsigned NOT NULL DEFAULT 1 COMMENT '�Ƿ�������˼���';
ALTER TABLE IMCompany ADD passwordstrength tinyint(1) unsigned NOT NULL DEFAULT 1 COMMENT '����ǿ��';
ALTER TABLE IMCompany ADD passwordlength tinyint(1) unsigned NOT NULL DEFAULT 8 COMMENT '���볤��';


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

#IMUser����
ALTER TABLE IMUser DROP jobnumber;
ALTER TABLE IMUser DROP isadmin;
ALTER TABLE IMUser DROP adminstatus;
ALTER TABLE IMUser DROP contactstatus;
ALTER TABLE IMUser DROP companyid;
ALTER TABLE IMUser DROP isiplimit;
ALTER TABLE IMUser ADD email varchar(100) COMMENT '����';
ALTER TABLE IMUser ADD isactived  tinyint(1) NOT NULL DEFAULT 0;
ALTER TABLE IMUser CHANGE lastmodifytime lastmodifiedtime int(11) unsigned NOT NULL DEFAULT 0;
ALTER TABLE IMUser ADD salt varchar(50) NOT NULL DEFAULT '' COMMENT '��ֵ';
ALTER TABLE IMUser MODIFY username varchar(50) COMMENT '�û���';
ALTER TABLE IMUser MODIFY name varchar(50) COLLATE utf8mb4_bin COMMENT '����';
ALTER TABLE IMUser ADD account varchar(100) COMMENT '��ҵ�ʺ�';
#ALTER TABLE IMUser add syncstatus tinyint(1) unsigned NOT NULL DEFAULT 0 COMMENT '�û�ͬ��״̬(0:δͬ����1:ͬ��)';

CREATE INDEX idx_IMUser_phone ON IMUser(phone);
CREATE INDEX idx_IMUser_email ON IMUser(email);
CREATE INDEX idx_IMUser_account ON IMUser(account);

#�޸�IMUserPosition��
ALTER TABLE IMUserPosition ADD jobgrade varchar(20) COLLATE utf8mb4_bin COMMENT 'ְ��';

#����Ա����
DROP TABLE IF EXISTS IMStaff;
CREATE TABLE IMStaff
(
	uuid varchar(40) NOT NULL COMMENT 'Ա��uuid',
	useruuid varchar(40) NOT NULL DEFAULT '' COMMENT'�û�uuid',
	userid int(11) unsigned NOT NULL DEFAULT 0 COMMENT '�û�id',
	jobnumber varchar(20) COMMENT '����',
	mobilephone varchar(32) COMMENT '�ֻ�����',
	staffname varchar(50) COLLATE utf8mb4_bin COMMENT 'Ա������',
	isadmin tinyint(1) unsigned NOT NULL DEFAULT 0 COMMENT '�Ƿ�Ϊ����Ա(0:��1:��)',
	adminstatus tinyint(1) unsigned NOT NULL DEFAULT 0 COMMENT '����Ա״̬(0:���ã�1:����)',
	issuperadmin tinyint(1) unsigned NOT NULL DEFAULT 0 COMMENT '�Ƿ񳬼�����Ա(0:��1:��)',	
	isactived tinyint(1) unsigned NOT NULL DEFAULT 0 COMMENT '�Ƿ񼤻�(0:��1:��)',
	contactstatus tinyint(1) unsigned NOT NULL DEFAULT 1 COMMENT 'ͨѶ¼״̬(0:����ʾ��1:��ʾ)',
	status tinyint(2) NOT NULL DEFAULT 1 COMMENT '�û�״̬(-1:ɾ����0:���ã�1:����)',
	companyid int(11) unsigned NOT NULL DEFAULT 0 COMMENT '��˾id',
	keyword varchar(200) NOT NULL DEFAULT '' COMMENT '�ؼ���',
	remark varchar(500) COLLATE utf8mb4_bin COMMENT '��ע',
	buyproduct varchar(256) COMMENT '������Ʒ',
	email varchar(100) COMMENT '����',
	username varchar(50) COMMENT '�û���',
	account varchar(100) COMMENT '��ҵ�ʺ�',
	defaultemail varchar(100) COMMENT 'Ĭ������',
	created int(11) unsigned NOT NULL DEFAULT 0 COMMENT '��������',
	updated int(11) unsigned NOT NULL DEFAULT 0 COMMENT '�޸�����',
	PRIMARY KEY (uuid),
	KEY idx_IMStaff_companyid (companyid),
	KEY idx_IMStaff_userid (userid),
	KEY idx_IMStaff_useruuid(useruuid),
	KEY idx_IMStaff_updated (updated),
	KEY idx_IMStaff_status (status)
)ENGINE=MyISAM DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='Ա����';

#����Ա����չ��Ϣ��
DROP TABLE IF EXISTS StaffDetails;
CREATE TABLE StaffDetails
 (
	uuid varchar(40)  NOT NULL  COMMENT '�û�����uuid',
  staffuuid varchar(40) NOT NULL DEFAULT '' COMMENT 'Ա��uuid',
  type int(3)  NOT NULL DEFAULT 0 COMMENT '������Ϣ����',
  title varchar(40) COLLATE utf8mb4_bin NOT NULL COMMENT '������Ϣ����',
  content varchar(1024) COLLATE utf8mb4_bin NOT NULL COMMENT '������Ϣ����',
  created int(10) unsigned NOT NULL DEFAULT '0' COMMENT '����ʱ��',
  updated int(10) unsigned NOT NULL DEFAULT '0' COMMENT '����ʱ��',
  PRIMARY KEY (uuid),
  KEY `idx_StaffDetails_staffuuid` (`staffuuid`),
  KEY `idx_StaffDetails_updated` (`updated`)
)ENGINE=MyISAM DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='Ա����չ��Ϣ�����';

#������Ҫ�û���
DROP TABLE IF EXISTS IMImportantPerson;
CREATE TABLE IMImportantPerson
(
	id int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
	userid int(11) unsigned NOT NULL DEFAULT 0 COMMENT '�û�id',
	vipid int(11) unsigned NOT NULL DEFAULT 0 COMMENT '�û���vip id',
	status tinyint(1) unsigned NOT NULL DEFAULT 0 COMMENT '״̬(0:������1:ɾ��)',
	created int(11) unsigned NOT NULL DEFAULT 0 COMMENT '��������',
	updated int(11) unsigned NOT NULL DEFAULT 0 COMMENT '��������',
	PRIMARY KEY (id),
	KEY idx_IMImportantPerson_userid (userid),
	KEY idx_IMImportantPerson_status(status),
	KEY idx_IMImportantPerson_updated(updated)
)ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='Ա����Ҫ�û���';

#����Ա����չ��Ϣ��ʱ��
DROP TABLE IF EXISTS StaffDetailsTemp;
CREATE TABLE StaffDetailsTemp
 (
	uuid varchar(40)  NOT NULL  COMMENT '�û�����uuid',
  staffuuid varchar(40) NOT NULL DEFAULT '' COMMENT 'Ա��uuid',
  type int(3)  NOT NULL DEFAULT 0 COMMENT '������Ϣ����',
  title varchar(40) COLLATE utf8mb4_bin NOT NULL COMMENT '������Ϣ����',
  content varchar(1024) COLLATE utf8mb4_bin NOT NULL COMMENT '������Ϣ����',
  created int(10) unsigned NOT NULL DEFAULT '0' COMMENT '����ʱ��',
  updated int(10) unsigned NOT NULL DEFAULT '0' COMMENT '����ʱ��',
  PRIMARY KEY (uuid),
  KEY `idx_StaffDetails_staffuuid` (`staffuuid`),
  KEY `idx_StaffDetails_updated` (`updated`)
)ENGINE=MyISAM DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='Ա����չ��Ϣ�����';

#������Ҫ�û���
DROP TABLE IF EXISTS IMImportantPersonTemp;
CREATE TABLE IMImportantPersonTemp
(
	id int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
	userid int(11) unsigned NOT NULL DEFAULT 0 COMMENT '�û�id',
	vipid int(11) unsigned NOT NULL DEFAULT 0 COMMENT '�û���vip id',
	status tinyint(1) unsigned NOT NULL DEFAULT 0 COMMENT '״̬(0:������1:ɾ��)',
	created int(11) unsigned NOT NULL DEFAULT 0 COMMENT '��������',
	updated int(11) unsigned NOT NULL DEFAULT 0 COMMENT '��������',
	PRIMARY KEY (id),
	KEY idx_IMImportantPersonTemp_userid (userid),
	KEY idx_IMImportantPersonTemp_status(status),
	KEY idx_IMImportantPersonTemp_updated(updated)
)ENGINE=MyISAM AUTO_INCREMENT=1 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='Ա����Ҫ�û���';


#IP���Ʊ�
DROP TABLE IF EXISTS IMUserIpLimit;
CREATE TABLE IMUserIpLimit
(
	uuid varchar(40)  NOT NULL  COMMENT 'IP����uuid',
  useruuid varchar(40) NOT NULL DEFAULT '' COMMENT '�û�uuid',
  limitip varchar(50) COMMENT '����IP',
  created int(10) unsigned NOT NULL DEFAULT '0' COMMENT '����ʱ��',
  updated int(10) unsigned NOT NULL DEFAULT '0' COMMENT '����ʱ��',
  PRIMARY KEY (uuid),
  KEY `idx_IMUserIpLimit_useruuid` (`useruuid`),
  KEY `idx_IMUserIpLimit_updated` (`updated`)
)ENGINE=MyISAM DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='IP���������';

#������
ALTER TABLE UserAlias DROP username;