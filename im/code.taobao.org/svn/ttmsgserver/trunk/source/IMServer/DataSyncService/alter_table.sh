#DROP TABLE IF EXISTS IMTransmitFile;
#CREATE TABLE `IMTransmitFile` (
#  `id` int(10) unsigned NOT NULL AUTO_INCREMENT COMMENT '主键',
#  `userId` int(10) unsigned NOT NULL COMMENT '发送者用户ID',
#  `toUserId` int(10) unsigned NOT NULL COMMENT '接收者用户ID',
#  `fileName` varchar(500) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '文件名',
#  `fsize` int(10) unsigned NOT NULL COMMENT '文件大小',
#  `taskId` varchar(100) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '任务ID',
#  `status` tinyint(3) unsigned NOT NULL DEFAULT '1' COMMENT '状态(0 => 等待接收,1=> 已接收)',
#  `filestate` tinyint(3) unsigned DEFAULT '4' COMMENT 'file state',
#  `filerole` tinyint(3) unsigned DEFAULT '3' COMMENT 'file transmition mode',
#  `serverip` varchar(16) COLLATE utf8mb4_bin DEFAULT '127.0.0.1' COMMENT 'file server ip address',
#  `serverport` smallint(5) unsigned DEFAULT '8600' COMMENT 'file server''s port',
#  `validperiod` smallint(3) unsigned DEFAULT '30' COMMENT 'file''s valid period',
#  `created` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '创建时间',
#  `updated` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '更新时间',
#  PRIMARY KEY (`id`),
#  KEY `idx_toUserId_created_status` (`toUserId`,`created`,`status`),
#  KEY `idx_userId_created_status` (`userId`,`created`,`status`),
#  KEY `idx_created` (`created`)
#) ENGINE=MyISAM AUTO_INCREMENT=39 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='IM离线文件表';

DROP TABLE IF EXISTS IMCompanyA;
CREATE TABLE IMCompanyA (
  Id int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '主键',
  domainid varchar(64) NOT NULL DEFAULT '' COMMENT '域名标识UUID',
  domainname varchar(255) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '域名',
  `companyName` varchar(255) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '公司名称',
  status tinyint(3) unsigned  NOT NULL DEFAULT 0 COMMENT '域名状态(0==>正常)',
  expiredate bigint(16) unsigned NOT NULL DEFAULT 0 COMMENT '域名到期日(0==>永久有效)',
  `created` int(11) unsigned NOT NULL DEFAULT 0 COMMENT '创建时间',
  `updated` int(11) unsigned NOT NULL DEFAULT 0 COMMENT '更新时间' ,
  PRIMARY KEY (Id),
	KEY idx_IMCompanyA_id (id),
	KEY idx_IMCompanyA_created (created)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8 COMMENT='公司名称';

DROP TABLE IF EXISTS IMCompanyATemp;
CREATE TABLE IMCompanyATemp (
  `Id` int(11) unsigned NOT NULL AUTO_INCREMENT,
  domainid varchar(64) NOT NULL DEFAULT '' COMMENT '域名标识UUID',
  domainname varchar(255) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '域名',
  `companyName` varchar(255) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '公司名称',
  status tinyint(3) unsigned  NOT NULL DEFAULT 0 COMMENT'域名状态(0==>正常)',
  expiredate bigint(16) unsigned NOT NULL DEFAULT 0 COMMENT '域名到期日(0==>永久有效)',
  `created` int(11) unsigned NOT NULL DEFAULT 0 COMMENT '创建时间',
  `updated` int(11) unsigned NOT NULL DEFAULT 0 COMMENT '更新时间',
  PRIMARY KEY (`Id`),
  KEY idx_IMCompanyATemp_id(id),
  KEY idx_IMCompanyATemp_created(created)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8 COMMENT='公司名称临时表';


DROP TABLE IF EXISTS IMDepartA;
CREATE TABLE IMDepartA (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '部门id',
  `companyid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '关联部门名称表',
  departid varchar(32) NOT NULL DEFAULT ''  COMMENT '部门标识UUID',
  `departName` varchar(64) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '部门名称',
  `priority` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '显示优先级',
  `parentId` int(11) unsigned NOT NULL COMMENT '上级部门id',
  `status` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '状态',
  `created` int(11) unsigned NOT NULL DEFAULT 0 COMMENT '创建时间',
  `updated` int(11) unsigned NOT NULL DEFAULT 0 COMMENT '更新时间',
  PRIMARY KEY (`id`),
  KEY `idx_IMDepartA_departName` (`departName`),
  KEY `idx_IMDepartA_priority_status` (`priority`,`status`),
  KEY idx_IMDepartA_departid(departid)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin;

DROP TABLE IF EXISTS IMDepartATemp;
CREATE TABLE IMDepartATemp (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '部门id',
  `companyid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '关联部门名称表',
  departid varchar(32) NOT NULL DEFAULT '' COMMENT '部门标识UUID',
  `departName` varchar(64) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '部门名称',
  `priority` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '显示优先级',
  `parentId` varchar(32)  NOT NULL COMMENT '上级部门id',
  `status` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '状态',
  `created` int(11) unsigned NOT NULL COMMENT '创建时间',
  `updated` int(11) unsigned NOT NULL COMMENT '更新时间',
  PRIMARY KEY (`id`),
  KEY `idx_IMDepartATemp_departName` (`departName`),
  KEY `idx_IMDepartATemp_priority_status` (`priority`,`status`),
  KEY idx_IMDepartATemp_departid(departid)
) ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin;

DROP TABLE IF EXISTS IMUserA;
CREATE TABLE `IMUserA` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '用户id',
  userid varchar(32) NOT NULL DEFAULT '' COMMENT '用户名标识UUID',
  `sex` tinyint(1) unsigned NOT NULL DEFAULT '0' COMMENT '1男2女0未知',
  `name` varchar(32) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '用户名',
    keyword varchar(32) NOT NULL DEFAULT '' COMMENT'关键字',
  `jodNumber` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '工号',
  `domain` varchar(32) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '拼音',
  `nick` varchar(32) CHARACTER SET utf8 NOT NULL DEFAULT '' COMMENT '花名,绰号等',
  `password` varchar(32) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '密码',
  `signature` varchar(255) CHARACTER SET utf8 DEFAULT NULL COMMENT '个性签名',
  `salt` varchar(4) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '混淆码',
  `phone` varchar(20) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '手机号码',
  `telephone` varchar(255) COLLATE utf8mb4_bin DEFAULT NULL COMMENT '固定电话',
  `faxNum` varchar(255) COLLATE utf8mb4_bin DEFAULT NULL,
  `postcode` varchar(255) COLLATE utf8mb4_bin DEFAULT NULL COMMENT '邮编',
  `address` varchar(255) CHARACTER SET utf8 DEFAULT NULL COMMENT '地址',
  `birthday` varchar(255) CHARACTER SET utf8 DEFAULT NULL COMMENT '生日',
  `email` varchar(64) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT 'email',
  `avatar` varchar(255) COLLATE utf8mb4_bin DEFAULT '' COMMENT '自定义用户头像',
  `status` tinyint(2) unsigned DEFAULT '0' COMMENT '1. 试用期 2. 正式 3. 离职 4.实习',
  `created` int(11) unsigned NOT NULL COMMENT '创建时间',
  `updated` int(11) unsigned NOT NULL COMMENT '更新时间',
  PRIMARY KEY (`id`),
  KEY `idx_IMUser_domain` (`domain`),
  KEY `idx_IMUser_name` (`name`),
  KEY idx_IMUser_phone (`phone`)
) ENGINE=InnoDB AUTO_INCREMENT=123 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin;


DROP TABLE IF EXISTS IMUserATemp;
CREATE TABLE `IMUserATemp` (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '用户id',
  userid varchar(32) NOT NULL DEFAULT '' COMMENT '用户标识UUID',
  `sex` tinyint(1) unsigned NOT NULL DEFAULT '0' COMMENT '1男2女0未知',
  `name` varchar(32) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '用户名',
  `domain` varchar(32) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '域名',
   `email` varchar(64) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT 'email',
  phone varchar(20) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '手机号码',
  keyword varchar(32) NOT NULL DEFAULT '' COMMENT'关键字',
  `password` varchar(32) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '密码',
  `birthday` varchar(255) CHARACTER SET utf8 DEFAULT NULL COMMENT '生日',
  `avatar` varchar(255) COLLATE utf8mb4_bin DEFAULT '' COMMENT '自定义用户头像',
  `status` tinyint(2) unsigned DEFAULT '0' COMMENT '1. 试用期 2. 正式 3. 离职 4.实习',
  `created` int(11) unsigned NOT NULL COMMENT '创建时间',
  `updated` int(11) unsigned NOT NULL COMMENT '更新时间',
  PRIMARY KEY (`id`),
  KEY `idx_IMUserTemp_domain` (`domain`),
  KEY `idx_IMUserTemp_name` (`name`),
  KEY idx_IMUseRTemp_phone (`phone`)
) ENGINE=InnoDB AUTO_INCREMENT=123 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin;


DROP TABLE IF EXISTS IMUserPosition;
CREATE TABLE IMUserPosition
(
	id int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '用户职位标识',
	departid int(11) unsigned NOT NULL DEFAULT 0 COMMENT '部门ID',
	position varchar(255) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '职位',
	PRIMARY KEY (`id`),
  KEY `idx_IMUserPosition_departid` (`departid`)
)ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin;

DROP TABLE IF EXISTS IMUserPositionTemp;
CREATE TABLE IMUserPositionTemp
(
	id int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '用户职位标识',
	departid varchar(32)  NOT NULL DEFAULT '' COMMENT '部门UUID',
	position varchar(255) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '职位',
	PRIMARY KEY (`id`),
  KEY `idx_IMUserPositionTemp_departid` (`departid`)
)ENGINE=InnoDB AUTO_INCREMENT=3 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin;
