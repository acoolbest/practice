#database dump 
mysql -uroot -p12345 -e "create database coa"
mysql --default-character-set=utf8 -uroot -p12345 coa < coa20160131.sql

alter table IMCompany modify status tinyint(3) not null default 1;

DROP TABLE IF EXISTS UserDetails;
CREATE TABLE `UserDetails` (
  `uuid` varchar(40)  NOT NULL  COMMENT '用户详情uuid',
  `useruuid` varchar(40) NOT NULL  COMMENT '用户uuid',
  `type` int(3)  NOT NULL DEFAULT '0' COMMENT '详情信息类型，1：邮箱（11：工作邮箱，12：个人邮箱）
2：电话号码（21：手机号码、22：家庭号码、23：工作号码）
3：地址（31：工作地址，32：家庭地址）
4：即时通讯（41：QQ、42：微信、43：MSN、44：Google Talk）
5：传真（51：家庭传真、52：工作传真、53：自定义）
6：自定义',
  `title` varchar(40) COLLATE utf8mb4_bin NOT NULL COMMENT '详情信息标题',
  `content` varchar(1024) COLLATE utf8mb4_bin NOT NULL COMMENT '详情信息内容',
  `created` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '创建时间',
  `updated` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '更新时间',
  PRIMARY KEY (`uuid`),
  KEY `idx_userdetails_useruuid` (`useruuid`),
  KEY `idx_userdetails_title` (`title`),
  KEY `idx_userdetails_updated` (`updated`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='用户详情表';

DROP TABLE IF EXISTS Position;
CREATE TABLE `Position` (
  `uuid` varchar(40)  NOT NULL  COMMENT '岗位uuid',
  `companyid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '公司id',
  `posname` varchar(50) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '岗位名称',
  `type` varchar(20)  COMMENT 'chiefleader：正职领导；leader：副职领导；staff：职员；other：其它；',
  `status` int(3) NOT NULL DEFAULT '0' COMMENT '状态：-1：已删除；0：正常',
  `remark` varchar(500) COLLATE utf8mb4_bin  COMMENT '备注',
  `sort` int(3) NOT NULL DEFAULT '0' COMMENT '排序',
  `created` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '创建时间',
  `updated` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '更新时间',
  PRIMARY KEY (`uuid`),
  KEY `idx_position_status` (`status`),
  KEY `idx_position_updated` (`updated`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='岗位表';


#
DROP TABLE IF EXISTS UserAlias;
CREATE TABLE `UserAlias` (
  `uuid` varchar(40)  NOT NULL  COMMENT '用户别名uuid',
  `useruuid` varchar(40) NOT NULL  COMMENT '用户uuid',
  `username` varchar(128)  NOT NULL  COMMENT '用户名',
  `aliasname` varchar(128) NOT NULL COMMENT '别名',
  `created` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '创建时间',
  `updated` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '更新时间',
  PRIMARY KEY (`uuid`),
  KEY `idx_useralias_useruuid` (`useruuid`),
  KEY `idx_useralias_updated` (`updated`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='用户别名表';
#
#
DROP TABLE IF EXISTS IMDepartA;
CREATE TABLE `IMDepartA`
(
  `uuid` varchar(40) NOT NULL COMMENT '部门uuid',
  `companyid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '公司id',
  `departname` varchar(128) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '部门名称',
  `priority` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '显示优先级',
  `parentuuid` varchar(40) COMMENT '上级部门id',
  `status` int(3) NOT NULL DEFAULT '000' COMMENT '状态 -1：删除； 0:正常',
  `created` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '创建时间',
  `updated` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '更新时间',
  `level` tinyint(3) unsigned zerofill NOT NULL DEFAULT '000' COMMENT '部门级别',
  `isdepart` tinyint(1) unsigned zerofill NOT NULL DEFAULT '000' COMMENT '是否是部门',
  `departpath` varchar(4000)  COMMENT'部门路径',
  `contactstatus` tinyint(1) unsigned zerofill NOT NULL DEFAULT '1' COMMENT'是否是部门',
  PRIMARY KEY (`uuid`),
  KEY `idx_IMDepartA_companyid` (`companyid`),
  KEY `idx_IMDepartA_status` (`status`),
  KEY `idx_IMDepartA_parentuuid` (`parentuuid`),
  KEY `idx_IMDepartA_contactstatus` (`contactstatus`),
  KEY `idx_IMDepartA_updated` (`updated`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='部门表';
#
##user position
DROP TABLE IF EXISTS IMUserPositionA;
CREATE TABLE `IMUserPositionA`
(
   `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT 'id',
  `uuid` varchar(40) COMMENT 'uuid',
  `companyid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '公司标识',
  `departuuid` varchar(40)  COMMENT '部门uuid',
  `posuuid` varchar(40)  COMMENT '岗位uuid',
  `useruuid` varchar(40) NOT NULL DEFAULT '' COMMENT '用户uuid',
  `status` tinyint(3)  NOT NULL DEFAULT '0' COMMENT '状态:0正常；1:删除',
  `created` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '创建时间',
  `updated` int(11) unsigned NOT NULL COMMENT '更新时间',
  PRIMARY KEY (`id`),
  KEY `idx_IMUserPositionA_useruuid` (`useruuid`),
  KEY `idx_IMUserPositionA_companyid` (`companyid`),
  KEY `idx_IMUserPositionA_departuuid` (`departuuid`),
  KEY `idx_IMUserPositionA_posuuid` (`posuuid`),
  KEY `idx_IMUserPositionA_updated` (`updated`),
  KEY `idx_IMUserPositionA_status` (`status`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='IM用户岗位表';
#
##user table
DROP TABLE IF EXISTS IMUserA;
CREATE TABLE `IMUserA`
 (
  `id` int(11) unsigned NOT NULL AUTO_INCREMENT COMMENT '用户id',
  `useruuid` varchar(40) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '用户名标识UUID',
  `jobnumber` varchar(20) COLLATE utf8mb4_bin DEFAULT NULL COMMENT '工号',
  `username` varchar(128) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '用户名',
  `name` varchar(160) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '姓名',
  `password` varchar(40) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '密码',
  `origpassword` varchar(40) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '原始密码',
  `histpassword` varchar(1000) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '历史密码',
  `avatar` varchar(200) COLLATE utf8mb4_bin DEFAULT '' COMMENT '自定义用户头像',
  `sex` tinyint(1) unsigned NOT NULL DEFAULT '2' COMMENT '0女 1男 2未知',
  `birthday` date NOT NULL DEFAULT '1970-01-01' COMMENT '生日',
  `phone` varchar(32) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '手机号码',
  `isadmin` tinyint(1) unsigned DEFAULT '0' COMMENT '0:否；1:是',
  `adminstatus` tinyint(1) unsigned NOT NULL DEFAULT '0' COMMENT '0:禁用；1:启用',
  `contactstatus` tinyint(1) unsigned NOT NULL DEFAULT '1' COMMENT '0:否；1:是',
  `status` tinyint(4)  NOT NULL DEFAULT '1' COMMENT '-1:已删除；0:禁用；1:启用',
  `begintime` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'EC用户创建时间',
  `expiredtime` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'EC用户到期时间',
  `updatedtime` int(11) unsigned NOT NULL DEFAULT '0' COMMENT 'EC用户数据更新时间',
  `isiplimit` tinyint(1) unsigned NOT NULL DEFAULT '0' COMMENT '0:不限制；1:限制',
  `lastmodifytime` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '密码最后修改时间',
  `islock` tinyint(1) unsigned NOT NULL DEFAULT '0' COMMENT '0:否；1:锁定',
  `lockip` varchar(15) COLLATE utf8mb4_bin NULL COMMENT '锁定IP',
  `lockstarttime` int(11) NULL  COMMENT '锁定开始时间',
  `lockendtime` int(11)  NULL  COMMENT '锁定结束时间',
  `keyword` varchar(200) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '关键字',
  `remark` varchar(500) COLLATE utf8mb4_bin COMMENT '备注',
  `nick` varchar(32) COLLATE utf8mb4_bin DEFAULT NULL COMMENT '花名,绰号等',
  `signature` varchar(255) COLLATE utf8mb4_bin DEFAULT NULL COMMENT '个性签名',
  #`salt` varchar(4) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '混淆码',
  `buyproduct` varchar(1024) COLLATE utf8mb4_bin DEFAULT NULL COMMENT '个性签名',
  `created` int(11) unsigned NOT NULL DEFAULT 0 COMMENT '创建时间',
  `updated` int(11) unsigned NOT NULL DEFAULT 0 COMMENT '更新时间',
   `companyid` int(11) unsigned NOT NULL DEFAULT '0' COMMENT '公司标识',
   `tmppassword` varchar(40) COLLATE utf8mb4_bin NOT NULL DEFAULT '' COMMENT '密码',
  PRIMARY KEY (`id`),
  KEY `idx_IMUserA_useruuid` (`useruuid`),
  KEY `idx_IMUserA_username` (`username`),
  KEY `idx_IMUserA_status` (`status`),
  KEY `idx_IMUserA_updated` (`updated`),
  KEY `idx_IMUserA_companyid` (`companyid`)
)ENGINE=InnoDB AUTO_INCREMENT=10001 DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='IM用户表';
SELECT AUTO_INCREMENT FROM information_schema.tables WHERE table_name="IMUserA";
#alter table IMUserA  auto_increment = 10724;
#
DROP TABLE IF EXISTS UserAliasTemp;
CREATE TABLE `UserAliasTemp` (
	`id` int(10) unsigned   NOT NULL  auto_increment COMMENT 'id',
  `uuid` varchar(40)  NOT NULL  COMMENT '用户别名uuid',
  `useruuid` varchar(40) NOT NULL  COMMENT '用户uuid',
  `username` varchar(128)  NOT NULL  COMMENT '用户名',
  `aliasname` varchar(128) NOT NULL COMMENT '别名',
  `created` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '创建时间',
  `updated` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '更新时间',
  PRIMARY KEY (`id`),
  KEY `idx_useraliastemp_uuid` (`uuid`),
  KEY `idx_useraliastemp_useruuid` (`useruuid`),
  KEY `idx_useraliastemp_updated` (`updated`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='用户别名表';
#
#
DROP TABLE IF EXISTS UserDetailsTemp;
CREATE TABLE `UserDetailsTemp` (
	`id` int(10) unsigned   NOT NULL  auto_increment COMMENT 'id',
  `uuid` varchar(40)  NOT NULL  COMMENT '用户详情uuid',
  `useruuid` varchar(40) NOT NULL  COMMENT '用户uuid',
  `type` int(3)  NOT NULL DEFAULT '0' COMMENT '详情信息类型，1：邮箱（11：工作邮箱，12：个人邮箱）
2：电话号码（21：手机号码、22：家庭号码、23：工作号码）
3：地址（31：工作地址，32：家庭地址）
4：即时通讯（41：QQ、42：微信、43：MSN、44：Google Talk）
5：传真（51：家庭传真、52：工作传真、53：自定义）
6：自定义',
  `title` varchar(40) COLLATE utf8mb4_bin NOT NULL COMMENT '详情信息标题',
  `content` varchar(1024) COLLATE utf8mb4_bin NOT NULL COMMENT '详情信息内容',
  `created` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '创建时间',
  `updated` int(10) unsigned NOT NULL DEFAULT '0' COMMENT '更新时间',
  PRIMARY KEY (`id`),
  KEY `idx_userdetails_uuid` (`uuid`),
  KEY `idx_userdetails_useruuid` (`useruuid`),
  KEY `idx_userdetails_title` (`title`),
  KEY `idx_userdetails_updated` (`updated`)
)ENGINE=InnoDB DEFAULT CHARSET=utf8mb4 COLLATE=utf8mb4_bin COMMENT='用户详情表';
#
# #initialize work flow
# #insert into IMUser(id,useruuid,username,name,keyword,nick,avatar,created,updated,buyproduct) values(1,'workflow','workflow','审批助手','workflow','审批助手','http://res.im.ioa.cn:12380/data/head/workflow_head_1.jpg',unix_timestamp(),unix_timestamp(),'IM,WORKFLOW');
# #initialize email
# #insert into IMUser(id,useruuid,usename,name,keyword,nick,avatar,created,updated,buyproduct) values(2,'email','email','邮件助手','email','邮件助手','http://res.im.ioa.cn:12380/data/head/email_head_1.jpg',unix_timestamp(),unix_timestamp(),'IM,EMAIL');
# #initialize bulltin
# #insert into IMUser(id,useruuid,usename,name,keyword,nick,avatar,created,updated,buyproduct) values(3,'bulletin','bulletin','公告助手','bulletin','公告助手','http://res.im.ioa.cn:12380/data/head/bulletin_head_1.jpg',unix_timestamp(),unix_timestamp(),'IM,BULLETIN');
# 
 #insert data into IMUserA from IMUser
 insert into IMUserA (id,useruuid,username,name,password,avatar,sex,birthday,phone,status,keyword,buyproduct,created,updated,companyid) 
 select id,userid,email,name,password,avatar,sex,birthday,phone,status,keyword,buyproduct,unix_timestamp(),unix_timestamp(),2 from IMUser where email like '%chinac.com' or id in(1,2,3);
 
 insert into IMUserA (id,useruuid,username,name,password,avatar,sex,birthday,phone,status,keyword,buyproduct,created,updated,companyid) 
 select id,userid,email,name,password,avatar,sex,birthday,phone,status,keyword,buyproduct,unix_timestamp(),unix_timestamp(),3 from IMUser where email like '%demo.com';
 
 insert into IMUserA (id,useruuid,username,name,password,avatar,sex,birthday,phone,status,keyword,buyproduct,created,updated,companyid) 
 select id,userid,email,name,password,avatar,sex,birthday,phone,status,keyword,buyproduct,unix_timestamp(),unix_timestamp(),5 from IMUser where email like '%gw.chinacmail.com';
 
  insert into IMUserA (id,useruuid,username,name,password,avatar,sex,birthday,phone,status,keyword,buyproduct,created,updated,companyid) 
 select id,userid,email,name,password,avatar,sex,birthday,phone,status,keyword,buyproduct,unix_timestamp(),unix_timestamp(),9 from IMUser where email like '%h0.chinacmail.com';
 
  insert into IMUserA (id,useruuid,username,name,password,avatar,sex,birthday,phone,status,keyword,buyproduct,created,updated,companyid) 
 select id,userid,email,name,password,avatar,sex,birthday,phone,status,keyword,buyproduct,unix_timestamp(),unix_timestamp(),1 from IMUser where email like '%huayunyou.com';
 
  insert into IMUserA (id,useruuid,username,name,password,avatar,sex,birthday,phone,status,keyword,buyproduct,created,updated,companyid) 
 select id,userid,email,name,password,avatar,sex,birthday,phone,status,keyword,buyproduct,unix_timestamp(),unix_timestamp(),6 from IMUser where email like '%ipower.cn';
 
  insert into IMUserA (id,useruuid,username,name,password,avatar,sex,birthday,phone,status,keyword,buyproduct,created,updated,companyid) 
 select id,userid,email,name,password,avatar,sex,birthday,phone,status,keyword,buyproduct,unix_timestamp(),unix_timestamp(),8 from IMUser where email like '%services.69dns.com';
 
   insert into IMUserA (id,useruuid,username,name,password,avatar,sex,birthday,phone,status,keyword,buyproduct,created,updated,companyid) 
 select id,userid,email,name,password,avatar,sex,birthday,phone,status,keyword,buyproduct,unix_timestamp(),unix_timestamp(),11 from IMUser where email like '%stat98.com';
 
  insert into IMUserA (id,useruuid,username,name,password,avatar,sex,birthday,phone,status,keyword,buyproduct,created,updated,companyid) 
 select id,userid,email,name,password,avatar,sex,birthday,phone,status,keyword,buyproduct,unix_timestamp(),unix_timestamp(),4 from IMUser where email like '%xinbaotiandi.cn';
 
  insert into IMUserA (id,useruuid,username,name,password,avatar,sex,birthday,phone,status,keyword,buyproduct,created,updated,companyid) 
 select id,userid,email,name,password,avatar,sex,birthday,phone,status,keyword,buyproduct,unix_timestamp(),unix_timestamp(),7 from IMUser where email like '%yunguclub.com';
  
 update IMUserA set contactstatus=1;
 update IMUserA set status=-1 where status=1;
 update IMUserA set status=1 where status = 0;
 update IMUserA set status=0 where status=2;
 
 update IMUserA set sex=3,updated=unix_timestamp() where sex = 2; #female
 update IMUserA set sex=2,updated=unix_timestamp() where sex = 0; #unknown
 update IMUserA set sex=0,updated=unix_timestamp() where sex = 3;
 
 update IMUserA set companyid=0 where id in(1,2,3);
# 
# #insert data into IMDepartA from IMDepart
 insert into IMDepartA (uuid,companyid,departname,priority,parentuuid,status,level,isdepart,departpath,contactstatus,created,updated) select org_id,2,org_name,sort,parent_id,status,level,is_org,path,contacts_status,unix_timestamp(),unix_timestamp() from coa.organization where domain_id='40b30cfa9f2145b1810e02f953a5b27b';
 insert into IMDepartA (uuid,companyid,departname,priority,parentuuid,status,level,isdepart,departpath,contactstatus,created,updated) select org_id,3,org_name,sort,parent_id,status,level,is_org,path,contacts_status,unix_timestamp(),unix_timestamp() from coa.organization where domain_id='5c248d4166a048cc9f03c68381d46a45';
 insert into IMDepartA (uuid,companyid,departname,priority,parentuuid,status,level,isdepart,departpath,contactstatus,created,updated) select org_id,5,org_name,sort,parent_id,status,level,is_org,path,contacts_status,unix_timestamp(),unix_timestamp() from coa.organization where domain_id='7d9313d704cb4acd9f585c4380691be3';
 insert into IMDepartA (uuid,companyid,departname,priority,parentuuid,status,level,isdepart,departpath,contactstatus,created,updated) select org_id,9,org_name,sort,parent_id,status,level,is_org,path,contacts_status,unix_timestamp(),unix_timestamp() from coa.organization where domain_id='c2d23764ab834bd6994102e4811900b6';  
 insert into IMDepartA (uuid,companyid,departname,priority,parentuuid,status,level,isdepart,departpath,contactstatus,created,updated) select org_id,1,org_name,sort,parent_id,status,level,is_org,path,contacts_status,unix_timestamp(),unix_timestamp() from coa.organization where domain_id='28dc6ce2525d461daae7ebf04ba47744';
 insert into IMDepartA (uuid,companyid,departname,priority,parentuuid,status,level,isdepart,departpath,contactstatus,created,updated) select org_id,6,org_name,sort,parent_id,status,level,is_org,path,contacts_status,unix_timestamp(),unix_timestamp() from coa.organization where domain_id='7fa785bf96a24d079dfdecdcdb8abd21';
 insert into IMDepartA (uuid,companyid,departname,priority,parentuuid,status,level,isdepart,departpath,contactstatus,created,updated) select org_id,8,org_name,sort,parent_id,status,level,is_org,path,contacts_status,unix_timestamp(),unix_timestamp() from coa.organization where domain_id='a57dd26190344579bce9beb635e48100';
 insert into IMDepartA (uuid,companyid,departname,priority,parentuuid,status,level,isdepart,departpath,contactstatus,created,updated) select org_id,11,org_name,sort,parent_id,status,level,is_org,path,contacts_status,unix_timestamp(),unix_timestamp() from coa.organization where domain_id='d89b4b2bd0804a6d84545f8173be2695';
 insert into IMDepartA (uuid,companyid,departname,priority,parentuuid,status,level,isdepart,departpath,contactstatus,created,updated) select org_id,4,org_name,sort,parent_id,status,level,is_org,path,contacts_status,unix_timestamp(),unix_timestamp() from coa.organization where domain_id='7516445617f947a8a608bdfbe596e477';
 insert into IMDepartA (uuid,companyid,departname,priority,parentuuid,status,level,isdepart,departpath,contactstatus,created,updated) select org_id,7,org_name,sort,parent_id,status,level,is_org,path,contacts_status,unix_timestamp(),unix_timestamp() from coa.organization where domain_id='95f3cb0d681c4a9fa4a42a55fa0e9092';
        
 #update IMDepartA set created=unix_timestamp(), updated=unix_timestamp() where companyid=2;
 update IMDepartA set parentuuid=NULL where parentuuid='-1';
 delete from IMDepartA  where departname='未分配部门';
# #update IMDepartA a,IMDepartA b set a.status=-1,a.updated=unix_timestamp() where a.parentuuid=b.uuid and b.status=-1;
# 
# #loading data from local file
# # 1:select position_id,position_name,status,type,remark,sort,2 from position where status=0 and domain_id='40b30cfa9f2145b1810e02f953a5b27b' into outfile 'position.txt' character set gbk;
# # 2:iconv -f gbk -t utf8 position.txt -o position.txt.utf8
# # 3:load data local infile '/root/position.txt' into table Position(uuid,posname,status,type,remark,sort,companyid);
# # 4:update position set created=unix_timestamp(),updated=created where companyid=2;
# 
#position
 insert into Position (uuid,companyid,posname,type,status,remark,sort,created,updated) select position_id,2,position_name,type,status,remark,sort,unix_timestamp(),unix_timestamp() from coa.position where domain_id='40b30cfa9f2145b1810e02f953a5b27b';
  insert into Position (uuid,companyid,posname,type,status,remark,sort,created,updated) select position_id,3,position_name,type,status,remark,sort,unix_timestamp(),unix_timestamp() from coa.position where domain_id='5c248d4166a048cc9f03c68381d46a45';
 insert into Position (uuid,companyid,posname,type,status,remark,sort,created,updated) select position_id,5,position_name,type,status,remark,sort,unix_timestamp(),unix_timestamp() from coa.position where domain_id='7d9313d704cb4acd9f585c4380691be3';
 insert into Position (uuid,companyid,posname,type,status,remark,sort,created,updated) select position_id,9,position_name,type,status,remark,sort,unix_timestamp(),unix_timestamp() from coa.position where domain_id='c2d23764ab834bd6994102e4811900b6';
 insert into Position (uuid,companyid,posname,type,status,remark,sort,created,updated) select position_id,1,position_name,type,status,remark,sort,unix_timestamp(),unix_timestamp() from coa.position where domain_id='28dc6ce2525d461daae7ebf04ba47744';
 insert into Position (uuid,companyid,posname,type,status,remark,sort,created,updated) select position_id,6,position_name,type,status,remark,sort,unix_timestamp(),unix_timestamp() from coa.position where domain_id='7fa785bf96a24d079dfdecdcdb8abd21';
 insert into Position (uuid,companyid,posname,type,status,remark,sort,created,updated) select position_id,8,position_name,type,status,remark,sort,unix_timestamp(),unix_timestamp() from coa.position where domain_id='a57dd26190344579bce9beb635e48100';
 insert into Position (uuid,companyid,posname,type,status,remark,sort,created,updated) select position_id,11,position_name,type,status,remark,sort,unix_timestamp(),unix_timestamp() from coa.position where domain_id='d89b4b2bd0804a6d84545f8173be2695';
 insert into Position (uuid,companyid,posname,type,status,remark,sort,created,updated) select position_id,4,position_name,type,status,remark,sort,unix_timestamp(),unix_timestamp() from coa.position where domain_id='7516445617f947a8a608bdfbe596e477';
 insert into Position (uuid,companyid,posname,type,status,remark,sort,created,updated) select position_id,7,position_name,type,status,remark,sort,unix_timestamp(),unix_timestamp() from coa.position where domain_id='95f3cb0d681c4a9fa4a42a55fa0e9092';

 insert into Position (uuid,companyid,sort,created,updated) values('00000000000000000000000000000000',0,1,unix_timestamp(),unix_timestamp());
 
#user position
 insert into IMUserPositionA (uuid,companyid,departuuid,posuuid,useruuid,created,updated) select user_position_id, 2,a.org_id,a.position_id,a.user_id,unix_timestamp(),unix_timestamp() from coa.user_position a,coa.organization b,coa.position c,coa.user d where a.org_id=b.org_id and a.position_id=c.position_id and a.user_id=d.user_id and b.domain_id=c.domain_id and c.domain_id=d.domain_id and d.domain_id='40b30cfa9f2145b1810e02f953a5b27b';
 insert into IMUserPositionA (uuid,companyid,departuuid,posuuid,useruuid,created,updated) select user_position_id, 3,a.org_id,a.position_id,a.user_id,unix_timestamp(),unix_timestamp() from coa.user_position a,coa.organization b,coa.position c,coa.user d where a.org_id=b.org_id and a.position_id=c.position_id and a.user_id=d.user_id and b.domain_id=c.domain_id and c.domain_id=d.domain_id and d.domain_id='5c248d4166a048cc9f03c68381d46a45';
 insert into IMUserPositionA (uuid,companyid,departuuid,posuuid,useruuid,created,updated) select user_position_id, 5,a.org_id,a.position_id,a.user_id,unix_timestamp(),unix_timestamp() from coa.user_position a,coa.organization b,coa.position c,coa.user d where a.org_id=b.org_id and a.position_id=c.position_id and a.user_id=d.user_id and b.domain_id=c.domain_id and c.domain_id=d.domain_id and d.domain_id='7d9313d704cb4acd9f585c4380691be3';
 insert into IMUserPositionA (uuid,companyid,departuuid,posuuid,useruuid,created,updated) select user_position_id, 9,a.org_id,a.position_id,a.user_id,unix_timestamp(),unix_timestamp() from coa.user_position a,coa.organization b,coa.position c,coa.user d where a.org_id=b.org_id and a.position_id=c.position_id and a.user_id=d.user_id and b.domain_id=c.domain_id and c.domain_id=d.domain_id and d.domain_id='c2d23764ab834bd6994102e4811900b6';
 insert into IMUserPositionA (uuid,companyid,departuuid,posuuid,useruuid,created,updated) select user_position_id, 1,a.org_id,a.position_id,a.user_id,unix_timestamp(),unix_timestamp() from coa.user_position a,coa.organization b,coa.position c,coa.user d where a.org_id=b.org_id and a.position_id=c.position_id and a.user_id=d.user_id and b.domain_id=c.domain_id and c.domain_id=d.domain_id and d.domain_id='28dc6ce2525d461daae7ebf04ba47744';
 insert into IMUserPositionA (uuid,companyid,departuuid,posuuid,useruuid,created,updated) select user_position_id, 6,a.org_id,a.position_id,a.user_id,unix_timestamp(),unix_timestamp() from coa.user_position a,coa.organization b,coa.position c,coa.user d where a.org_id=b.org_id and a.position_id=c.position_id and a.user_id=d.user_id and b.domain_id=c.domain_id and c.domain_id=d.domain_id and d.domain_id='7fa785bf96a24d079dfdecdcdb8abd21';
 insert into IMUserPositionA (uuid,companyid,departuuid,posuuid,useruuid,created,updated) select user_position_id, 8,a.org_id,a.position_id,a.user_id,unix_timestamp(),unix_timestamp() from coa.user_position a,coa.organization b,coa.position c,coa.user d where a.org_id=b.org_id and a.position_id=c.position_id and a.user_id=d.user_id and b.domain_id=c.domain_id and c.domain_id=d.domain_id and d.domain_id='a57dd26190344579bce9beb635e48100';
 insert into IMUserPositionA (uuid,companyid,departuuid,posuuid,useruuid,created,updated) select user_position_id, 11,a.org_id,a.position_id,a.user_id,unix_timestamp(),unix_timestamp() from coa.user_position a,coa.organization b,coa.position c,coa.user d where a.org_id=b.org_id and a.position_id=c.position_id and a.user_id=d.user_id and b.domain_id=c.domain_id and c.domain_id=d.domain_id and d.domain_id='d89b4b2bd0804a6d84545f8173be2695';
 insert into IMUserPositionA (uuid,companyid,departuuid,posuuid,useruuid,created,updated) select user_position_id, 4,a.org_id,a.position_id,a.user_id,unix_timestamp(),unix_timestamp() from coa.user_position a,coa.organization b,coa.position c,coa.user d where a.org_id=b.org_id and a.position_id=c.position_id and a.user_id=d.user_id and b.domain_id=c.domain_id and c.domain_id=d.domain_id and d.domain_id='7516445617f947a8a608bdfbe596e477';
 insert into IMUserPositionA (uuid,companyid,departuuid,posuuid,useruuid,created,updated) select user_position_id, 7,a.org_id,a.position_id,a.user_id,unix_timestamp(),unix_timestamp() from coa.user_position a,coa.organization b,coa.position c,coa.user d where a.org_id=b.org_id and a.position_id=c.position_id and a.user_id=d.user_id and b.domain_id=c.domain_id and c.domain_id=d.domain_id and d.domain_id='95f3cb0d681c4a9fa4a42a55fa0e9092';
 
 insert into IMUserPositionA (companyid,departuuid,posuuid,useruuid,created,updated) select 2, a.org_id,'00000000000000000000000000000000',user_id,unix_timestamp(),unix_timestamp() from coa.org_user a, coa.organization b where a.org_id=b.org_id and b.domain_id='40b30cfa9f2145b1810e02f953a5b27b'  and not exists( select 1 from IMUserPositionA c where a.org_id=c.departuuid and a.user_id = c.useruuid); 
 insert into IMUserPositionA (companyid,departuuid,posuuid,useruuid,created,updated) select 3, a.org_id,'00000000000000000000000000000000',user_id,unix_timestamp(),unix_timestamp() from coa.org_user a, coa.organization b where a.org_id=b.org_id and b.domain_id='5c248d4166a048cc9f03c68381d46a45'  and not exists( select 1 from IMUserPositionA c where a.org_id=c.departuuid and a.user_id = c.useruuid);
 insert into IMUserPositionA (companyid,departuuid,posuuid,useruuid,created,updated) select 5, a.org_id,'00000000000000000000000000000000',user_id,unix_timestamp(),unix_timestamp() from coa.org_user a, coa.organization b where a.org_id=b.org_id and b.domain_id='7d9313d704cb4acd9f585c4380691be3'  and not exists( select 1 from IMUserPositionA c where a.org_id=c.departuuid and a.user_id = c.useruuid); 
 insert into IMUserPositionA (companyid,departuuid,posuuid,useruuid,created,updated) select 9, a.org_id,'00000000000000000000000000000000',user_id,unix_timestamp(),unix_timestamp() from coa.org_user a, coa.organization b where a.org_id=b.org_id and b.domain_id='c2d23764ab834bd6994102e4811900b6'  and not exists( select 1 from IMUserPositionA c where a.org_id=c.departuuid and a.user_id = c.useruuid);  
 insert into IMUserPositionA (companyid,departuuid,posuuid,useruuid,created,updated) select 1, a.org_id,'00000000000000000000000000000000',user_id,unix_timestamp(),unix_timestamp() from coa.org_user a, coa.organization b where a.org_id=b.org_id and b.domain_id='28dc6ce2525d461daae7ebf04ba47744'  and not exists( select 1 from IMUserPositionA c where a.org_id=c.departuuid and a.user_id = c.useruuid); 
 insert into IMUserPositionA (companyid,departuuid,posuuid,useruuid,created,updated) select 6, a.org_id,'00000000000000000000000000000000',user_id,unix_timestamp(),unix_timestamp() from coa.org_user a, coa.organization b where a.org_id=b.org_id and b.domain_id='7fa785bf96a24d079dfdecdcdb8abd21'  and not exists( select 1 from IMUserPositionA c where a.org_id=c.departuuid and a.user_id = c.useruuid); 
 insert into IMUserPositionA (companyid,departuuid,posuuid,useruuid,created,updated) select 8, a.org_id,'00000000000000000000000000000000',user_id,unix_timestamp(),unix_timestamp() from coa.org_user a, coa.organization b where a.org_id=b.org_id and b.domain_id='a57dd26190344579bce9beb635e48100'  and not exists( select 1 from IMUserPositionA c where a.org_id=c.departuuid and a.user_id = c.useruuid); 
 insert into IMUserPositionA (companyid,departuuid,posuuid,useruuid,created,updated) select 11,a.org_id,'00000000000000000000000000000000',user_id,unix_timestamp(),unix_timestamp() from coa.org_user a, coa.organization b where a.org_id=b.org_id and b.domain_id='d89b4b2bd0804a6d84545f8173be2695'  and not exists( select 1 from IMUserPositionA c where a.org_id=c.departuuid and a.user_id = c.useruuid); 
 insert into IMUserPositionA (companyid,departuuid,posuuid,useruuid,created,updated) select 4, a.org_id,'00000000000000000000000000000000',user_id,unix_timestamp(),unix_timestamp() from coa.org_user a, coa.organization b where a.org_id=b.org_id and b.domain_id='7516445617f947a8a608bdfbe596e477'  and not exists( select 1 from IMUserPositionA c where a.org_id=c.departuuid and a.user_id = c.useruuid);
 insert into IMUserPositionA (companyid,departuuid,posuuid,useruuid,created,updated) select 7, a.org_id,'00000000000000000000000000000000',user_id,unix_timestamp(),unix_timestamp() from coa.org_user a, coa.organization b where a.org_id=b.org_id and b.domain_id='95f3cb0d681c4a9fa4a42a55fa0e9092'  and not exists( select 1 from IMUserPositionA c where a.org_id=c.departuuid and a.user_id = c.useruuid);
#user details
insert into UserDetails (uuid,useruuid,type,title,content,created,updated) select user_details_id,user_id,type,title,content,unix_timestamp(),unix_timestamp() from coa.user_details;

#user alias
insert into UserAlias (uuid,useruuid,username,aliasname,created,updated) select alias_id,user_id,username,alias_name,unix_timestamp(),unix_timestamp() from coa.user_alias;

#rabbitmq 访问控制问题
#检查msfs动态链接库，如果缺少，则copy后放到/usr/lib64目录下(libtiff.so.5.2.0,libjbig.so.2.0)

 
select a.org_id,user_id from coa.org_user a, coa.organization b where a.org_id=b.org_id and b.domain_id='40b30cfa9f2145b1810e02f953a5b27b' and not  exists( select 1 from IMUserPositionA c where a.org_id=c.departuuid and a.user_id = c.useruuid);


//just for internal development
insert into IMUserA (useruuid,jobnumber,username,name,password,origpassword,histpassword,avatar,sex,birthday,phone,isadmin,adminstatus,contactstatus,status,begintime,expiredtime,updatedtime,isiplimit,lastmodifytime,islock,lockip,lockstarttime,lockendtime,keyword,remark,nick,signature,created,updated,companyid) 
select user_id,user_code,username,full_name,password,original_password,history_password,head_portrait,sex,birthday,phone,is_admin,admin_status,contacts_status,a.status,begin_time,expired_time,update_time,is_ip_limit,last_modfy_time,is_lock,lock_ip,lock_start_time,lock_end_time,keyword,remark,nickname,signature,unix_timestamp(),unix_timestamp(),b.id from coa.user a, IMCompany b where a.domain_id=b.domainid and not exists(select 1 from IMUserA c where a.user_id=c.useruuid);

insert into IMDepartA (uuid,companyid,departname,priority,parentuuid,status,level,isdepart,departpath,contactstatus,created,updated) select org_id,b.id,org_name,sort,parent_id,a.status,level,is_org,path,contacts_status,unix_timestamp(),unix_timestamp() from coa.organization a, IMCompany b  where a.domain_id=b.domainid;

insert into Position (uuid,companyid,posname,type,status,remark,sort,created,updated) select position_id,b.id,position_name,type,a.status,remark,sort,unix_timestamp(),unix_timestamp() from coa.position a, IMCompany b where a.domain_id=b.domainid;

insert into IMUserPositionA (uuid,companyid,departuuid,posuuid,useruuid,created,updated) select user_position_id, c.id,a.org_id,a.position_id,a.user_id,unix_timestamp(),unix_timestamp() from coa.user_position a,coa.organization b ,IMCompany c where a.org_id=b.org_id and b.domain_id=c.domainid;

#insert mail group session
insert into IMRecentSession (userid,peerid,type,status,created,updated) select userid,groupid,2,0,unix_timestamp(),unix_timestamp() from IMGroupMember a where a.status=0 and not exists(select 1 from IMRecentSession b where a.userid=b.userid and a.groupid=b.peerid and b.type=2);     