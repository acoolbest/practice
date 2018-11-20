#just for internal development
#loading data
insert into IMCompany (companyuuid,companyno,companyname,companyfullname,industrytype,status,created,updated) select primary_org_id,org_code,org_name,org_full_name,industry,status,unix_timestamp(),unix_timestamp() from coa.org_info;
#update IMCompany a, coa.org_password b set a.validitystatus=b.validity_status,a.validity=b.validity,a.isforcepassword=b.is_force_password where a.companyuuid=b.primary_org_id;
#update IMCompany a, coa.org_set b set a.isjoined = b.is_search,a.isinvite = b.is_invite,a.updated=unix_timestamp() where a.companyuuid=b.primary_org_id;

#update IMCompany a, coa.domain_info b set a.companyname=b.company where a.domainid=b.domain_id;

#just for internal development
insert into IMUser (useruuid,username,name,password,origpassword,histpassword,avatar,sex,birthday,phone,status,lastmodifiedtime,islock,lockip,lockstarttime,lockendtime,keyword,remark,nick,signature,created,updated) 
select user_id,username,full_name,password,original_password,history_password,head_portrait,sex,birthday,phone,status,last_modified_time,is_lock,lock_ip,lock_start_time,lock_end_time,keyword,remark,nickname,signature,unix_timestamp(),unix_timestamp() from coa.user;

insert into IMDepart (uuid,companyid,departname,priority,parentuuid,status,level,isdepart,departpath,contactstatus,created,updated) select org_id,b.id,org_name,sort,parent_id,a.status,level,is_org,path,contacts_status,unix_timestamp(),unix_timestamp() from coa.organization a, IMCompany b  where a.primary_org_id=b.companyuuid;

insert into IMPosition (uuid,companyid,posname,type,status,remark,sort,created,updated) select position_id,b.id,position_name,type,a.status,remark,sort,unix_timestamp(),unix_timestamp() from coa.position a, IMCompany b where a.primary_org_id=b.companyuuid;

insert into IMUserPosition (uuid,companyid,departuuid,posuuid,useruuid,created,updated) select staff_position_id, c.id,a.org_id,a.position_id,a.staff_id,unix_timestamp(),unix_timestamp() from coa.staff_position a,coa.organization b ,IMCompany c where a.org_id=b.org_id and b.primary_org_id=c.companyuuid;

insert into IMUserPosition (uuid,companyid,departuuid,useruuid,posuuid,created,updated) select org_staff_id,c.id,a.org_id,staff_id,'00000000000000000000000000000000', unix_timestamp(),unix_timestamp() from coa.org_staff a, coa.organization b,IMCompany c where a.org_id=b.org_id and b.primary_org_id=c.companyuuid;
#insert into IMRecentSession (userid,peerid,type,status,created,updated) select userid,groupid,2,0,unix_timestamp(),unix_timestamp() from IMGroupMember a where a.status=0 and not exists(select 1 from IMRecentSession b where a.userid=b.userid and a.groupid=b.peerid and b.type=2);     
# #initialize work flow
#insert into IMUserA (id,status,contactstatus,useruuid,username,name,keyword,nick,avatar,created,updated,buyproduct) values(1,1,1,'workflow','workflow','审批助手','workflow','审批助手','http://res.im.ioa.cn:12380/data/head/workflow_head_1.jpg',unix_timestamp(),unix_timestamp(),'IM,WORKFLOW');
# #initialize email
#insert into IMUserA (id,status,contactstatus,useruuid,username,name,keyword,nick,avatar,created,updated,buyproduct) values(2,1,1,'email','email','邮件助手','email','邮件助手','http://res.im.ioa.cn:12380/data/head/email_head_1.jpg',unix_timestamp(),unix_timestamp(),'IM,EMAIL');
# #initialize bulltin
#insert into IMUserA (id,status,contactstatus,useruuid,username,name,keyword,nick,avatar,created,updated,buyproduct) values(3,1,1,'bulletin','bulletin','公告助手','bulletin','公告助手','http://res.im.ioa.cn:12380/data/head/bulletin_head_1.jpg',unix_timestamp(),unix_timestamp(),'IM,BULLETIN');

 #user details
insert into UserDetails (uuid,useruuid,type,title,content,created,updated) select user_details_id,user_id,type,title,content,unix_timestamp(),unix_timestamp() from coa.user_details a where not exists(select 1 from UserDetails b where a.user_details_id=b.uuid);

#user alias
insert into UserAlias (uuid,useruuid,aliasname,created,updated) select alias_id,user_id,alias_name,unix_timestamp(),unix_timestamp() from coa.user_alias a where not exists(select 1 from UserAlias b where a.alias_id=b.uuid); 

#update department
update IMDepart set parentuuid=NULL where parentuuid='-1';
#delete from IMDepartA  where departname='未分配部门';
#end of loading



##data migration
##IMCompany table migration
update IMCompany a, coa.org_info b set a.companyname=b.org_name,a.companyno=b.org_code,a.companyfullname=b.org_full_name,a.industrytype=b.industry,a.status=b.status where a.companyuuid=b.primary_org_id;
update IMCompany a, coa.org_password b set a.validitystatus=b.validity_status,a.validity=b.validity,a.isforcepassword=b.is_force_password where a.companyuuid=b.primary_org_id;
update IMCompany a, coa.org_set b set a.isjoined = b.is_search,a.isinvite = b.is_invite,a.updated=unix_timestamp() where a.companyuuid=b.primary_org_id;
#
##IMUser table migration
update IMUser a, coa.user b set a.username=b.username ,a.name=b.full_name ,a.avatar=b.head_portrait ,a.phone=b.phone ,a.status=b.status,a.lastmodifiedtime=b.last_modified_time where a.useruuid=b.user_id;
update IMUser a, coa.user b set a.islock=b.is_lock ,a.lockip=b.lock_ip ,a.keyword=b.keyword,a.remark=b.remark,a.account=b.account,a.password=b.password where a.useruuid=b.user_id;
update IMUser a, coa.user b set a.signature=b.signature,a.isinitpassword=b.is_init_password ,a.email=b.email,a.salt=b.salt,a.isactived=b.is_activate,a.updated=unix_timestamp() where a.useruuid=b.user_id;
update IMUser set avatar='assistant_workflow',updated=unix_timestamp() where id=1;
update IMUser set avatar='assistant_mail',updated=unix_timestamp() where id=2;
update IMUser set avatar='assistant_bulletin',updated=unix_timestamp() where id=3;
#
#
##IMUserPosition table migration
update IMUserPosition a, coa.org_staff b set a.jobgrade=b.relation,a.updated=unix_timestamp() where a.useruuid=b.staff_id and a.departuuid=b.org_id and a.posuuid='00000000000000000000000000000000';
update IMUserPosition a, (select useruuid,departuuid from IMUserPosition where status=0 group by useruuid,departuuid) as b set a.status=0,updated=unix_timestamp() where a.useruuid=b.useruuid and a.departuuid=b.departuuid and a.posuuid='00000000000000000000000000000000' and a.status <> 0;
#
##IMStaff table migration
insert into IMStaff (uuid,useruuid,userid,jobnumber,mobilephone,staffname,isadmin,adminstatus,issuperadmin,isactived,contactstatus,status,companyid,keyword,remark,account,email,username,defaultemail,created,updated)
select staff_id,a.user_id,b.id,a.staff_code,a.phone,a.full_name,a.is_admin,a.admin_status,a.is_super_admin,a.is_activate,a.contacts_status,a.status,c.id,a.keyword,a.remark,a.account,a.email,a.username,a.default_email,unix_timestamp(),unix_timestamp() from coa.staff a, IMUser b,IMCompany c where a.user_id=b.useruuid and a.primary_org_id=c.companyuuid;
update IMStaff set buyproduct='HR,BULLETIN,DOC,CLOUDDISK,EC,WORKFLOW,IM,CONTACTS,MAIL';

#staff details
insert into StaffDetails (uuid,staffuuid,type,title,content,created,updated) select staff_details_id,staff_id,type,title,content,unix_timestamp(),unix_timestamp() from coa.staff_details a where not exists(select 1 from StaffDetails b where a.staff_details_id=b.uuid);

#set companyid for group
update IMGroup a, (select userid,min(companyid) as mincompanyid from IMStaff group by userid) as b set a.companyid=b.mincompanyid,a.updated=unix_timestamp(),a.version=a.version+1 where a.creator=b.userid and a.companyid = 0;

#we should change operation time to now()
update IMMessage_0 set status=1 where (fromid >0 and fromid < 4) and from_unixtime(created) < now();
update IMMessage_1 set status=1 where (fromid >0 and fromid < 4) and from_unixtime(created) < now();
update IMMessage_2 set status=1 where (fromid >0 and fromid < 4) and from_unixtime(created) < now();
update IMMessage_3 set status=1 where (fromid >0 and fromid < 4) and from_unixtime(created) < now();
update IMMessage_4 set status=1 where (fromid >0 and fromid < 4) and from_unixtime(created) < now();
update IMMessage_5 set status=1 where (fromid >0 and fromid < 4) and from_unixtime(created) < now();
update IMMessage_6 set status=1 where (fromid >0 and fromid < 4) and from_unixtime(created) < now();
update IMMessage_7 set status=1 where (fromid >0 and fromid < 4) and from_unixtime(created) < now();

#update recentsessio,operaton time should be change to now()
update IMRecentSession set status=1,updated=unix_timestamp() where ((userid>0 and userid<4) or (peerid >0 and peerid < 4)) and type=1 and from_unixtime(updated) < now() and status=0;

#remove session for abnormal user
update IMRecentSession a, (select id from IMUser where status <> 1) as b set a.status=1,a.updated=unix_timestamp() where (a.userid=b.id or (a.peerid=b.id and type=1)) and status=0;