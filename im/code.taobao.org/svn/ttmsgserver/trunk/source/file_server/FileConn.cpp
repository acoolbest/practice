/*
 * FileConn.cpp
 *
 *  Created on: 2013-12-9
 *      Author: ziteng@mogujie.com
 */

#include "FileConn.h"
#include <sys/stat.h>
#include "IM.File.pb.h"
#include "IM.Server.pb.h"
#include "IM.Other.pb.h"
#include "public_define.h"
using namespace IM::BaseDefine;

static ConnMap_t g_file_conn_map; // connection with others, on connect insert...
static UserMap_t g_file_user_map; // after user login, insert...
/// yunfan add 2014.8.6
static TaskMap_t g_file_task_map;
static pthread_rwlock_t g_file_task_map_lock = PTHREAD_RWLOCK_INITIALIZER;
/// yunfan add end

static char g_current_save_path[BUFSIZ];

/// yunfan add 2014.8.12
static std::list<IM::BaseDefine::IpAddr> g_addr;
uint16_t g_listen_port = 0;
uint32_t g_task_timeout = 3600*24;  //change from 1hour to 24hours
#define SEGMENT_SIZE 65536
/// yunfan add end
uint32_t g_file_valid_period = 30; //30 days,added by george jiang on Aug 18th,2015

void file_conn_timer_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
	uint64_t cur_time = get_tick_count();
	for (ConnMap_t::iterator it = g_file_conn_map.begin(); it != g_file_conn_map.end(); )
	{
		ConnMap_t::iterator it_old = it;
		it++;

		CFileConn* pConn = (CFileConn*)it_old->second;
		pConn->OnTimer(cur_time);
	}
}

void release_task(const char* task_id)
{
    /// should be locked
    if (NULL == task_id) {
        return ;
    }
    
    pthread_rwlock_wrlock(&g_file_task_map_lock);

    TaskMap_t::iterator iter = g_file_task_map.find(task_id); //lock
    if (g_file_task_map.end() == iter)
	{
        pthread_rwlock_unlock(&g_file_task_map_lock);//false unlock
        return ;
    }

	LIST_TRANSFER_TASK listTask = iter->second;
    g_file_task_map.erase(iter);

	//    printf("-------ERASE TASK %s----------\n", t->task_id.c_str());
	for (IT_LIST_TRANSFER_TASK itTask = iter->second.begin(); itTask != iter->second.end(); ++itTask)
	{
		transfer_task_t* t = *itTask;
		t->lock(__LINE__);
		t->release();
		t->unlock(__LINE__);

		delete t;
		t = NULL;
	}

	pthread_rwlock_unlock(&g_file_task_map_lock); //true unlock

    return ;
}

void file_task_timer_callback(void* callback_data, uint8_t msg, uint32_t handle, void* lparam)
{
    pthread_rwlock_wrlock(&g_file_task_map_lock); //lock
    
	for (TaskMap_t::iterator iter = g_file_task_map.begin(); iter != g_file_task_map.end();)
	{
		// check if self-destroy == true
		// then delete task
		//LIST_TRANSFER_TASK listTask = iter->second;

		//int nCount = 0;
		for (IT_LIST_TRANSFER_TASK itTask = iter->second.begin(); itTask != iter->second.end(); )
		{
			transfer_task_t* pCurrentTask = *itTask;
			if (pCurrentTask)
			{
				pCurrentTask->lock(__LINE__); //task lock
				if (true == pCurrentTask->self_destroy)
				{
					if (pCurrentTask->transfered_size == pCurrentTask->upload_packages.size() && pCurrentTask->from_conn == NULL && pCurrentTask->to_conn == NULL)
					{
						pCurrentTask->release();
						pCurrentTask->unlock(__LINE__);
						delete pCurrentTask;
						pCurrentTask = NULL;
						//++nCount;	
						IT_LIST_TRANSFER_TASK itCurrTask = itTask;
						*itTask = NULL;
						++itTask;
						iter->second.erase(itCurrTask);
						log("TASK %s for a connection is DELETED from the connection list..................", iter->first.c_str());
					}
					else
					{
						//log("TASK: %s, TxSIZE: %d, TOTAL UPLOAD SIZE: %d, FROM: %p, TO: %p", iter->first.c_str(), pCurrentTask->transfered_size, pCurrentTask->upload_packages.size(), pCurrentTask->from_conn, pCurrentTask->to_conn);
						pCurrentTask->unlock(__LINE__);
						++itTask;
					}
				}
				else
				{ // self-destroy not true
					// check if timeout
					long esp = time(NULL) - pCurrentTask->create_time;
					//if (esp > g_task_timeout)
					if (esp > 3600 * 24 * (g_file_valid_period + 1))
					{
						// set self_destory true
						// then continue;
						// next round, it will be deleted
						pCurrentTask->self_destroy = true;
						pCurrentTask->transfered_size = pCurrentTask->upload_packages.size();
						log("FILE TRANSMITION SPENDS %d seconds and TIME OUT FOR TASK: %s", esp, iter->first.c_str());
					}
					pCurrentTask->unlock(__LINE__);
					++itTask;
				}
			}
			else
			{
				IT_LIST_TRANSFER_TASK itCurrTask = itTask;
				++itTask;
				iter->second.erase(itCurrTask); //erase deleted task
				//++nCount;
				log("TASK %s for a null connection is DELETED from the connection list..................", iter->first.c_str());
			}
		}

		//log("TASK: %s, REMAINING NODES: %d", iter->first.c_str(), iter->second.size());
		//if (nCount == iter->second.size()) //deleted node is equals to the list size
		if (iter->second.size() == 0) //no nodes for the task
		{
			TaskMap_t::iterator currNode = iter;
			++iter;
			log("TASK %s is DELETED from the task list", currNode->first.c_str());
			g_file_task_map.erase(currNode);
		}
		else
		{
			//log("TASK: %s, DELETED COUNT: %d, REMAIN CONNECTIONS: %d", iter->first.c_str(), nCount, iter->second.size());
			++iter;
		}
    }

	pthread_rwlock_unlock(&g_file_task_map_lock); //lock
   	
    return ;
}

void init_file_conn(std::list<IM::BaseDefine::IpAddr>& q, uint32_t timeout, uint32_t nValidPeriod)
{
    /// yunfan add 2014.8.12
    g_addr = q;
    g_task_timeout = timeout;
    /// yunfan add end
	g_file_valid_period = nValidPeriod;

	char work_path[BUFSIZ];
	if(!getcwd(work_path, BUFSIZ)) {
		log("getcwd failed");
	} else {
		snprintf(g_current_save_path, BUFSIZ, "%s/offline_file", work_path);
	}

	log("save offline files to %s", g_current_save_path);

	int ret = mkdir(g_current_save_path, 0755);
	if ( (ret != 0) && (errno != EEXIST) ) {
		log("!!!mkdir failed to save offline files");
	}

	netlib_register_timer(file_conn_timer_callback, NULL, 1000);
    netlib_register_timer(file_task_timer_callback, NULL, 10000);
}

/// yunfan add 2014.8.7
/// offline file upload
/// file-svr will send pull-data-req to sender
/// then wait for sender's rsp
void* _DoUpload(void* lparam)
{
	pthread_detach(pthread_self());

    if (NULL == lparam) {
        return NULL;
    }
    transfer_task_t* t = reinterpret_cast<transfer_task_t*>(lparam);
    
	if (!t)
	{
		return NULL;
	}

	
	t->lock(__LINE__);

	//log("Locking when uploading a file for task: %s....", t->task_id.c_str());
    t->create_time = time(NULL);
    
    // at begin
    // send 10 data-pull-req
    for (uint32_t cnt = 0; cnt < 1; ++cnt)
	{
        std::map<uint32_t, upload_package_t*>::iterator iter = t->upload_packages.begin();
        if (t->upload_packages.end() != iter)
		{
            IM::File::IMFilePullDataReq msg;
            msg.set_task_id(t->task_id);
            msg.set_user_id(t->from_user_id);
            msg.set_trans_mode(::IM::BaseDefine::FILE_TYPE_OFFLINE);
            msg.set_offset(iter->second->offset);
            msg.set_data_size(iter->second->size);
			msg.set_base_peer_id(0); //upload data, base peer id is set 0 by default
            CImPdu pdu;
            pdu.SetPBMsg(&msg);
            pdu.SetServiceId(SID_FILE);
            pdu.SetCommandId(CID_FILE_PULL_DATA_REQ);
            CFileConn* pConn = (CFileConn*)t->GetConn(t->from_user_id);
            pConn->SendPdu(&pdu);
            log("Pull Data Req: task id:%s, user id: %d, offset: %d, data size: %d", t->task_id.c_str(), t->from_user_id, iter->second->offset, iter->second->size);
			++iter;
        }
        //++iter;
    }
    
    // what if there is no rsp?
    // still send req?
    // no!
    // at last, the user will cancel
    // next ver, do change

	//while (t->transfered_size != t->upload_packages.size())
	//{
	//	if (t->self_destroy)
	//	{
	//		log("timeout, exit thread, task %s", t->task_id.c_str());
	//		return NULL;
	//	}
	//	sleep(1);
	//}

	// write head
	if (NULL == t->file_head)
	{
		t->file_head = new file_header_t;
	}
	if (NULL == t->file_head)
	{
		log("create file header failed %s", t->task_id.c_str());
		// beacuse all data in mem
		// it has to be released

		/*
		UserMap_t::iterator ator = g_file_user_map.find(t->from_user_id);
		if (g_file_user_map.end() != ator) {
		CFileConn* pConn = (CFileConn*)ator->second;
		pConn->Close();
		}
		*/
		CFileConn* pConn = (CFileConn*)t->GetConn(t->from_user_id);
		t->self_destroy = true;
		t->unlock(__LINE__);
		log("Unlocking when failing to create file header for task in 4....... ");
		pConn->Close();

		return NULL;
	}
	t->file_head->set_create_time(time(NULL));
	t->file_head->set_task_id(t->task_id.c_str());
	t->file_head->set_from_user_id(t->from_user_id);
	t->file_head->set_to_user_id(t->to_user_id);
	t->file_head->set_file_name("");
	t->file_head->set_file_size(t->file_size);
	try
	{
		if (t->fp)
		{
			fwrite(t->file_head, 1, sizeof(file_header_t), t->fp);
		}
		else
		{
			t->lock(__LINE__);
			log("invalid pointer to a file");
			return NULL;
		}
	}
	catch (exception& Exception)
	{
		log("An exception occurs when writing file header: %s........", Exception.what());
		;
	}

	std::map<uint32_t, upload_package_t*>::iterator itor = t->upload_packages.begin();

	bool bContinue = true;
	while (bContinue)
	{
		if (t->self_destroy)
		{
			log("timeout, exit thread, task %s", t->task_id.c_str());
			t->transfered_size = t->upload_packages.size(); //cancelling the task when uploading,current transfered block is set to total blocks
			t->unlock(__LINE__); //unlock
			bContinue = false;
			//log("Unlocking when uploading a file in 1...........");
			return NULL;
		}
		else if (t->from_conn == NULL && t->to_conn == NULL) //client disconnects when uploading a file
		{
			t->self_destroy = true;
			log("%d DISCONNECTS WHEN UPLOADING A FILE AND MAKE THE TASK %s DESTROYED", t->from_user_id, t->task_id.c_str());
		}
		else 
		{
			//write data.added on Dec 7th, 2015
			while(itor != t->upload_packages.end())
			{
				if (itor->second->data)
				{
					try
					{
						fwrite(itor->second->data, 1, itor->second->size, t->fp);
						log("WRIE DATA SIZE: %d", itor->second->size);
					}
					catch (exception& Exception)
					{
						log("an exception occurs when writing file data: %s.......", Exception.what());
						;
					}
					delete[]itor->second->data;
					itor->second->data = NULL;
					++itor;
				}
				else
				{
					break;
				}
			}

			if (t->transfered_size == t->upload_packages.size())
			{
				log("all data successfully received and ready to write to file.......TASK ID: %s", t->task_id.c_str());
				bContinue = false;
			}
			else
			{
				t->unlock(__LINE__); //unlock
				/*log("Unlocking when uploading a file in 2...........");
				log("TASK: %s\tCURRENT TRANSFERED SIZE: %d\tUPLOAD PACKAGE SIZE: %d", t->task_id.c_str(), t->transfered_size, t->upload_packages.size());*/
				sleep(1);
				t->lock(__LINE__);
			}
		}
	}


	////maybe conditional lock is better 
	//bool bContinue = true;
	//while (bContinue)
	//{
	//	if (t->self_destroy)
	//	{
	//		log("timeout, exit thread, task %s", t->task_id.c_str());
	//		t->transfered_size = t->upload_packages.size(); //cancelling the task when uploading,current transfered block is set to total blocks
	//		t->unlock(__LINE__); //unlock
	//		bContinue = false;	
	//		//log("Unlocking when uploading a file in 1...........");
	//		return NULL;
	//	}
	//	else if (t->transfered_size != t->upload_packages.size())
	//	{
	//		if (t->from_conn == NULL && t->to_conn == NULL) //client disconnects when uploading a file
	//		{
	//			t->self_destroy = true;
	//			log("%d DISCONNECTS WHEN UPLOADING A FILE AND MAKE THE TASK %s DESTROYED", t->from_user_id, t->task_id.c_str());			
	//		}

	//		//added on Dec 7th, 2015
	//		if(itor != t->upload_packages.end())
	//		{
	//			if (itor->second->data)
	//			{
	//				try
	//				{
	//					fwrite(itor->second->data, 1, itor->second->size, t->fp);
	//					log("WRIE DATA SIZE: %d", itor->second->size);
	//				}
	//				catch (exception& Exception)
	//				{
	//					log("an exception occurs when writing file data: %s.......", Exception.what());
	//					;
	//				}
	//				delete[]itor->second->data;
	//				itor->second->data = NULL;
	//				++itor;
	//			}				
	//		}

	//		t->unlock(__LINE__); //unlock
	//		/*log("Unlocking when uploading a file in 2...........");
	//		log("TASK: %s\tCURRENT TRANSFERED SIZE: %d\tUPLOAD PACKAGE SIZE: %d", t->task_id.c_str(), t->transfered_size, t->upload_packages.size());*/
	//		sleep(1);
	//		t->lock(__LINE__);
	//		/*log("Relocking a again when uploading a file for task: %s in 3.....", t->task_id.c_str());*/
	//	}
	//	else
	//	{
	//		log("all data successfully received and ready to write to file.......TASK ID: %s", t->task_id.c_str());
	//		bContinue = false;
	//	}	
	//}

    
 //   // write head
 //   if (NULL == t->file_head) {
 //       t->file_head = new file_header_t;
 //   }
 //   if (NULL == t->file_head) {
 //       log("create file header failed %s", t->task_id.c_str());
 //       // beacuse all data in mem
 //       // it has to be released
 //       
 //       /*
 //       UserMap_t::iterator ator = g_file_user_map.find(t->from_user_id);
 //       if (g_file_user_map.end() != ator) {
 //           CFileConn* pConn = (CFileConn*)ator->second;
 //           pConn->Close();
 //       }
 //        */
 //       CFileConn* pConn = (CFileConn*)t->GetConn(t->from_user_id);
	//	t->self_destroy = true;
 //       t->unlock(__LINE__);
	//	log("Unlocking when failing to create file header for task in 4....... ");
 //       pConn->Close(); 
	//	
 //       return NULL;  
 //   }
 //   t->file_head->set_create_time(time(NULL));
 //   t->file_head->set_task_id(t->task_id.c_str());
 //   t->file_head->set_from_user_id(t->from_user_id);
 //   t->file_head->set_to_user_id(t->to_user_id);
 //   t->file_head->set_file_name("");
 //   t->file_head->set_file_size(t->file_size);
	//try
	//{
	//	fwrite(t->file_head, 1, sizeof(file_header_t), t->fp);
	//}
	//catch (exception& Exception)
	//{
	//	log("An exception occurs when writing file header: %s........",Exception.what());
	//	;
	//}
    
  /*  std::map<uint32_t, upload_package_t*>::iterator itor = t->upload_packages.begin();
    for ( ; itor != t->upload_packages.end(); ++itor)
	{
		try
		{
			fwrite(itor->second->data, 1, itor->second->size, t->fp);
		}
		catch (exception& Exception)
		{
			log("an exception occurs when writing file data: %s.......", Exception.what());
			;
		}
    }*/
    
	//added by george on Dec 7th,2015
	try
	{
		fflush(t->fp);
		if (t->fp)
		{
			fclose(t->fp);
			t->fp = NULL;
		}
	}
	catch (exception& Exception)
	{
		log("an exception occurs when writing data/removing file: %s", Exception.what());
	}

	log("SUCCEEDING IN WRITING DATA FOR TASK: %s...........", t->task_id.c_str());
    t->unlock(__LINE__);
	//log("Unlocking when uploading a file successfully in 5..............");
    return NULL;
}


int generate_id(char* id)
{
    if (NULL == id) {
        return  -1; // invalid param
    }
    
    uuid_t uid = {0};
    uuid_generate(uid);
    if (uuid_is_null(uid)) {
        id = NULL;
        return -2; // uuid generate failed
    }
    uuid_unparse(uid, id);
    
    return 0;
}
/// yunfan add end

CFileConn::CFileConn()
{
	//log("CFileConn\n");
	m_bAuth = false;
	m_user_id = 0;
    m_conntype = CONN_TYPE_MSG_SERVER;
}

CFileConn::~CFileConn()
{
	log("~CFileConn, user_id=%u", m_user_id);

	for (FileMap_t::iterator it = m_save_file_map.begin(); it != m_save_file_map.end(); it++) {
		file_stat_t* file = it->second;
		fclose(file->fp);
		delete file;
	}
	m_save_file_map.clear();
}

void CFileConn::Close()
{
    log("close client, handle %d", m_handle);
    
    m_bAuth = false;
    
	if (m_handle != NETLIB_INVALID_HANDLE) {
		netlib_close(m_handle);
		g_file_conn_map.erase(m_handle);
	}

	if (m_user_id > 0) {
		g_file_user_map.erase(m_user_id);
        m_user_id = 0;
	}

	//george jiang inserted the following bracket on July 24th, 2015
	//connection is reset to NULL when close the connection
	
	pthread_rwlock_wrlock(&g_file_task_map_lock); //lock

	//log("rwlock locking....................................");
	for (TaskMap_t::iterator iter = g_file_task_map.begin(); iter != g_file_task_map.end(); ++iter)
	{
		//LIST_TRANSFER_TASK listTask = iter->second;
		for (IT_LIST_TRANSFER_TASK itTask = iter->second.begin(); itTask != iter->second.end(); ++itTask)
		{
			transfer_task_t* t = *itTask;
			if (t)
			{	
				t->lock(__LINE__);
				if (t->from_conn == this) //uploading files to others
				{
					log("from connection: %p for the task: %s is closed", t->from_conn, t->task_id.c_str());
					t->from_conn = NULL;
				}

				if (t->to_conn == this) //downloading files from others
				{
					log("to connection: %p for the task: %s is closed", t->to_conn, t->task_id.c_str());
					t->to_conn = NULL;
				}
				t->unlock(__LINE__);
			}
		}
	}

	pthread_rwlock_unlock(&g_file_task_map_lock); //unlock
	//log("rwlock unlocking....................................");
	//end of inserting

	ReleaseRef();
}

void CFileConn::OnConnect(net_handle_t handle)
{
	/// yunfan modify 2014.8.7
    m_handle = handle;

	g_file_conn_map.insert(make_pair(handle, this));    
	netlib_option(handle, NETLIB_OPT_SET_CALLBACK, (void*)imconn_callback);
	netlib_option(handle, NETLIB_OPT_SET_CALLBACK_DATA, (void*)&g_file_conn_map);

	uint32_t socket_buf_size = NETLIB_MAX_SOCKET_BUF_SIZE;
	netlib_option(handle, NETLIB_OPT_SET_SEND_BUF_SIZE, &socket_buf_size);
	netlib_option(handle, NETLIB_OPT_SET_RECV_BUF_SIZE, &socket_buf_size);
    /// yunfan modify end
}

void CFileConn::OnClose()
{
	log("client onclose: handle=%d", m_handle);
	Close();
}

void CFileConn::OnTimer(uint64_t curr_tick)
{
	//30 seconds
    if (m_conntype == CONN_TYPE_MSG_SERVER) {
        if (curr_tick > m_last_recv_tick + SERVER_TIMEOUT) {
            log("msg server timeout");
            Close();
        }
    }
    else
    {
		//2 minutes
        if (curr_tick > m_last_recv_tick + CLIENT_TIMEOUT) {
            log("client timeout, user_id=%u", m_user_id);
            Close();
        }
    }
	
}

void CFileConn::OnWrite()
{
	CImConn::OnWrite();
}

void CFileConn::HandlePdu(CImPdu* pPdu)
{
	switch (pPdu->GetCommandId()) {
        case CID_OTHER_HEARTBEAT:
            _HandleHeartBeat(pPdu);
            break;
        case CID_FILE_LOGIN_REQ:
            _HandleClientFileLoginReq(pPdu);
            break;
        
        case CID_FILE_STATE:
            _HandleClientFileStates(pPdu);
            break ;
        case CID_FILE_PULL_DATA_REQ:
            _HandleClientFilePullFileReq(pPdu);
            break ;
        case CID_FILE_PULL_DATA_RSP:
            _HandleClientFilePullFileRsp( pPdu);
            break ;
        case CID_OTHER_FILE_TRANSFER_REQ:
            _HandleMsgFileTransferReq(pPdu);
            break ;
        case CID_OTHER_FILE_SERVER_IP_REQ:
            _HandleGetServerAddressReq(pPdu);
            break;
        default: 
            log("no such cmd id: %u", pPdu->GetCommandId());
            break;
	}
}

void CFileConn::_HandleHeartBeat(CImPdu *pPdu)
{
    SendPdu(pPdu);
}

void CFileConn::_HandleClientFileLoginReq(CImPdu* pPdu)
{
    // if can not find uuid
    // return invalid uuid
    // if invalid user_id
    // return invalid user
    
    // if ready_to_recv or offline / mobile task
    //   return can_send
    // return ok

	IM::File::IMFileLoginReq msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));
    uint32_t user_id = msg.user_id();
    string task_id = msg.task_id();
    IM::BaseDefine::ClientFileRole mode = msg.file_role();
	string file_id = msg.file_id(); //added on Dec 7th,2015

	m_user_id = user_id;
	log("client login, user_id=%u, handle %d, task id: %s,file id: %s", m_user_id, m_handle, task_id.c_str(), file_id.c_str());

    // auth done
    m_bAuth = true;
    m_conntype = CONN_TYPE_CLIENT;
    
    IM::File::IMFileLoginRsp msg2;
    msg2.set_result_code(1);
    msg2.set_task_id(task_id);
    CImPdu pdu;
    pdu.SetPBMsg(&msg2);
    pdu.SetServiceId(SID_FILE);
    pdu.SetCommandId(CID_FILE_LOGIN_RES);
    pdu.SetSeqNum(pPdu->GetSeqNum());

    // create task for offline download
	if (IM::BaseDefine::CLIENT_OFFLINE_DOWNLOAD == mode) 
	{
        // create a thread // insert into a queue, multi-threads handle the queue
        // find file
        // send file
        transfer_task_t* t = new transfer_task_t;
        if (NULL == t)
		{
            SendPdu(&pdu);
            Close();
            log("create task failed for task id %s, user %d", task_id.c_str(), m_user_id);
            return; // create task failed
        }
        t->task_id = task_id;
        t->to_user_id = m_user_id;
        t->create_time = time(NULL);
		t->file_id = file_id;

        pthread_rwlock_wrlock(&g_file_task_map_lock);  //lock

		TaskMap_t::iterator iter = g_file_task_map.find(task_id.c_str());
		//LIST_TRANSFER_TASK listTask;
		if (g_file_task_map.end() == iter) //task no found(maybe the side for uploading is closed or file server restarts)
		{
			LIST_TRANSFER_TASK listTask;
			listTask.push_back(t);
			g_file_task_map.insert(std::make_pair(task_id.c_str(), listTask)); // insert a new task for downloading (from client side)
			log("create a new task: %s for user: %d  when downloading", task_id.c_str(), t->to_user_id);
		}
		else // the task has already existed
		{
			//listTask = iter->second;
			IT_LIST_TRANSFER_TASK itTask = iter->second.begin();
			for (; itTask != iter->second.end(); ++itTask)
			{
				transfer_task_t* pPreviousTask = *itTask;
				if (pPreviousTask) 
				{
					pPreviousTask->lock(__LINE__);
					if (pPreviousTask->self_destroy) //destroy the task
					{
						if (t->transfered_size != t->upload_packages.size())  //downloading mode
						{
							t->transfered_size = t->upload_packages.size(); //mark to be deleted
						}
					}
					else if(pPreviousTask->to_user_id == m_user_id) //duplicated task
					{
						pPreviousTask->file_id = file_id;
						delete t;
						t = NULL;
						pPreviousTask->unlock(__LINE__);
						log("user %d 's task is already exists", m_user_id, iter->first.c_str());
						break;
					}
					pPreviousTask->unlock(__LINE__);
					//log("mark the previous node to be deleted when downloading a file, task_id :%s", iter->first.c_str());
				}
			}

			if (itTask == iter->second.end()) //new client connection
			{
				iter->second.push_back(t);
				log("new connection for task: %s, userid: %d", t->task_id.c_str(), m_user_id);
			}
		}

        pthread_rwlock_unlock(&g_file_task_map_lock); //unlock
    }
    

    // check task
    pthread_rwlock_wrlock(&g_file_task_map_lock);  //lock

    TaskMap_t::iterator iter = g_file_task_map.find(task_id.c_str());
    if (g_file_task_map.end() == iter) {
        // failed to find task
        // return invaild task id
        pthread_rwlock_unlock(&g_file_task_map_lock); //false unlock
        SendPdu(&pdu);
        Close(); // invalid user for task
        log("check task id failed, user_id = %u, request taks id %s", m_user_id, task_id.c_str());
        return;
    }
    //transfer_task_t* t = iter->second;
	//LIST_TRANSFER_TASK listTask = iter->second;
       
	bool bValidUser = false;
	transfer_task_t* t = NULL, *t1 = NULL;
	int nCount = 0; //for test
	for (IT_LIST_TRANSFER_TASK itTask = iter->second.begin(); itTask != iter->second.end(); ++itTask)
	{
		++nCount; // for test
		t = *itTask;
		//log("get user from the task list................");
		if (t)
		{
			t->lock(__LINE__);
			if (!t->self_destroy)
			{
				if (t->from_user_id != m_user_id && t->to_user_id != m_user_id)
				{
					t->unlock(__LINE__);
					continue;
				}
				else
				{
					bValidUser = true;
					//check valid period for the file
					time_t nCurrentTime = time(NULL);
					if (nCurrentTime - t->create_time > 3600 * 24 * (g_file_valid_period + 1))
					{
						t->self_destroy = true;
						t->transfered_size = t->upload_packages.size();
						t->unlock(__LINE__);

						SendPdu(&pdu);
						Close();
						log("invalid task: %s, current time: %lu, create time: %lu, deficit: %lu seconds", task_id.c_str(), nCurrentTime, t->create_time, nCurrentTime - t->create_time);
						return;
					}

					t1 = t; 
					if (t->from_user_id == m_user_id)
					{
						t->from_conn = this;
					}

					if (t->to_user_id == m_user_id)
					{
						t->to_conn = this;
					}

					if (m_user_id == t->from_user_id)
					{
						t->ready_to_send = true;
					}

					if (m_user_id == t->to_user_id)
					{
						t->ready_to_recv = true;
					}

					log("INDEX: %d, TASK %s from CONNECTION: %p to %p, from USER: %d to %d", nCount, t->task_id.c_str(), t->from_conn, t->to_conn, t->from_user_id, t->to_user_id);

					t->unlock(__LINE__);
					//break; commented by george on March 4th,2016
				}
			}
			else
			{
				t->unlock(__LINE__);
			}
		}
	}

	pthread_rwlock_unlock(&g_file_task_map_lock); //unlock

    // check user
   	if (!bValidUser)
	{
        // invalid user
        // return error
        SendPdu(&pdu);
        Close();
        log("invalid user %u for task %s", m_user_id, task_id.c_str());
        return;
    }
    
	// prepare for offline upload
	if (IM::BaseDefine::CLIENT_OFFLINE_UPLOAD == mode)
	{
		int iret = _PreUpload(task_id.c_str());
		if (0 > iret) {
			SendPdu(&pdu);
			Close();
			log("preload faild for task %s, err %d", task_id.c_str(), iret);
			return;
		}
	}

    // send result
    msg2.set_result_code(0);
    CImPdu pdu2;
    pdu2.SetPBMsg(&msg2);
    pdu2.SetServiceId(SID_FILE);
    pdu2.SetCommandId(CID_FILE_LOGIN_RES);
    pdu2.SetSeqNum(pPdu->GetSeqNum());
	SendPdu(&pdu2); // login succeed
	//log("sending a packet for file login response to the client..........");
   

	//george jiang commented the following bracket on July 21st,2015
    //// notify that the peet is ready
    //if ( (m_user_id == t->to_user_id && \
    //      t->ready_to_send) || \
    //    (m_user_id == t->from_user_id && \
    //     t->ready_to_recv)){
    //    // send peer-ready state to recver
    //    _StatesNotify(IM::BaseDefine::CLIENT_FILE_PEER_READY, task_id.c_str(), m_user_id, t->GetConn(m_user_id));
    //    log("nofity recver %d task %s can recv", m_user_id, task_id.c_str());
    //}
    //end of commenting

    // create a thread // insert into a queue, multi-threads handle the queue
    // send to client PULLDATA msg
    // recv and write file
    if (IM::BaseDefine::CLIENT_OFFLINE_UPLOAD == mode)
	{
        // check thread id
		t = t1;
        pthread_create(&t->worker, NULL, _DoUpload, t);

        log("create thread for offline upload task %s user %d thread id %lu", task_id.c_str(), m_user_id, t->worker);
    }
    /// yunfan add end

    return;
}

/// yunfan add 2014.8.6
//only for uploading
void CFileConn::_HandleMsgFileTransferReq(CImPdu* pPdu)
{
    // if realtime transfer
    // new realtime_task
    // generate uuid
    // copy userid_1 userid_2
    // time_t = time(null);
    // return uuid
    // else new offline_task
    // generate uuid
    // copy user_1
    // copy file_size
    // time_t = time(null)
    // return uuid
    
    // create task for:
    // realtime transfer and offline upload

    IM::Server::IMFileTransferReq msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));

    
    uint32_t from_id = msg.from_user_id();
    uint32_t to_id = msg.to_user_id();
	string strFileID = "";
    
    IM::Server::IMFileTransferRsp msg2;
    msg2.set_result_code(1);
    msg2.set_from_user_id(from_id);
    msg2.set_to_user_id(to_id);
    msg2.set_file_name(msg.file_name());
    msg2.set_file_size(msg.file_size());
    msg2.set_task_id(msg.task_id());
    msg2.set_trans_mode(msg.trans_mode());
    msg2.set_attach_data(msg.attach_data());
	msg2.set_file_id(""); //added on Dec 7th,2015
    CImPdu pdu;
    pdu.SetPBMsg(&msg2);
    pdu.SetServiceId(SID_OTHER);
    pdu.SetCommandId(CID_OTHER_FILE_TRANSFER_RSP);
    pdu.SetSeqNum(pPdu->GetSeqNum());

	//task id comes from client,commented on Aug 18th,2015
    //char task_id[64] = {0};
    //int iret = generate_id(task_id);
    //if (0 > iret || NULL == task_id) {
    //    SendPdu(&pdu);
    //    log("create task id failed");
    //    return; // errno create task id failed 1
    //}
    //

	//added by george on Dec 7th,2015
	char file_id[64] = {0};
	int iret = generate_id(file_id); //get file id
	if (0 > iret || NULL == file_id)
	{
	    SendPdu(&pdu);
	    log("create file id failed");
	    return; // errno create task id failed 1
	}
	
    // new task and add to task_map
    transfer_task_t* task = new transfer_task_t;
    if (NULL == task)
	{
        // log new failed
        // return error
        SendPdu(&pdu);
        Close(); // close connection with msg svr
        log("create task failed");
        return; // create task failed
    }
    
    task->transfer_mode = msg.trans_mode();
    //task->task_id = task_id;
	task->task_id = msg.task_id();
    task->from_user_id = from_id;
    task->to_user_id = to_id;
    task->file_size = msg.file_size();
   // task->create_time = time_t(NULL);
	task->create_time = time(NULL);
	task->file_id = file_id; //added by george on Dec 7th,2015

    // read cfg file
    msg2.set_result_code(0);
	//msg2.set_task_id(task_id);
	msg2.set_task_id(msg.task_id());
	msg2.set_task_create_time(task->create_time);
	msg2.set_valid_period(g_file_valid_period);
	msg2.set_file_id(file_id);

    pdu.SetPBMsg(&msg2);
	pdu.SetServiceId(SID_OTHER);
	pdu.SetCommandId(CID_OTHER_FILE_TRANSFER_RSP);
	pdu.SetSeqNum(pPdu->GetSeqNum());
	SendPdu(&pdu);
    
    //task->create_time = time(NULL);
	//LIST_TRANSFER_TASK listTask;
    pthread_rwlock_wrlock(&g_file_task_map_lock); //lock

	TaskMap_t::iterator iter = g_file_task_map.find(task->task_id.c_str()); 
	if (g_file_task_map.end() == iter)
	{
		LIST_TRANSFER_TASK listTask;
		listTask.push_back(task);
		g_file_task_map.insert(make_pair((char*)task->task_id.c_str(), listTask));
		log("TASK %s NO FOUND, INSERTING!", task->task_id.c_str());
	}
	else //cancelling a task before and recarrying out a task(retransferring a file)
	{
		log("TASK %s FOUND, CURRENT LIST NODE: %d.....................", iter->first.c_str(), iter->second.size());
		//listTask = iter->second;
		//log("NODE: %d", iter->second.size());

		//for (IT_LIST_TRANSFER_TASK itTask = iter->second.begin(); itTask != iter->second.end(); ++itTask)
		//{
		//	transfer_task_t* pPreviousTask = *itTask;
		//	if (pPreviousTask)
		//	{
		//		pPreviousTask->lock(__LINE__);
		//		//if (pPreviousTask->from_user_id == m_user_id) //task already exists
		//		{
		//			pPreviousTask->self_destroy = true;
		//			//pPreviousTask->transfered_size = pPreviousTask->upload_packages.size();
		//			log("TASK %s for USER: %d already EXISTS, MARK NODES TO BE DELETED: %d", pPreviousTask->task_id.c_str(), pPreviousTask->from_user_id, iter->second.size());
		//		}
		//		pPreviousTask->unlock(__LINE__);
		//	}
		//}
		iter->second.push_back(task); //append a new node for the task
		log("AFTER INSERTING, TASK LIST NODES: %d", iter->second.size(), iter->second.size());
		/*iter->second.push_back(task);
		log("LIST NODE: %d, TASK NODE2: %d", iter->second.size(), iter->second.size());*/
	}

    pthread_rwlock_unlock(&g_file_task_map_lock); //unlock
    
    log("create task succeed, task id %s, task type %d, from user %d, to user %d, file size:%lu", task->task_id.c_str(),task->transfer_mode, task->from_user_id, task->to_user_id, task->file_size);
    
    return;
}

void CFileConn::_HandleClientFileStates(CImPdu* pPdu)
{
    // switch state
    // case ready_to_recv
    //     if sender_on
    //       tell sender can_send
    //     else update state
    // case cancel
    //      notify ohter node cancel
    //      close socket
    // case dnoe
    //      notify recver done
    //      close socket

    if (!_IsAuth()) {
		return;
	}
    
    IM::File::IMFileState msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));

    
    string task_id = msg.task_id();
    uint32_t user_id = msg.user_id();
    uint32_t file_stat = msg.state();

    pthread_rwlock_wrlock(&g_file_task_map_lock); //lock

	//log("Locking for getting status in 1.............");
    TaskMap_t::iterator iter = g_file_task_map.find((char*)task_id.c_str());
    if (g_file_task_map.end() == iter) {
		//log("Unlocking for getting status in 2.............");
        pthread_rwlock_unlock(&g_file_task_map_lock); //false unlock
        return; // invalid task id
    }

	//LIST_TRANSFER_TASK listTask = iter->second;
	transfer_task_t* t = NULL, *t1= NULL;
	for (IT_LIST_TRANSFER_TASK itTask = iter->second.begin(); itTask != iter->second.end(); ++itTask)
	{
		t = *itTask;
		if (t)
		{
			t->lock(__LINE__);
			//log("Locking a task in status 1");
			if (!t->self_destroy && (t->from_user_id == user_id || t->to_user_id == user_id))
			{
				t->unlock(__LINE__);
				//log("Unlocking a task in status 2");
				//break;
				t1 = t;
			}
			else
			{
				t->unlock(__LINE__);
				//log("Unlocking a task in status 3");
				t = NULL;
			}
		}
	}

    pthread_rwlock_unlock(&g_file_task_map_lock); //unlock
	//log("Unlocking for getting status in 3");

	t = t1;
	if (t)
	{
		log("File State Request:  task id: %s, from use id: %d, to user id: %d, file state;%d, current user: %d", task_id.c_str(), t->from_user_id, t->to_user_id, file_stat, user_id);
		t->lock(__LINE__);
		if (t->from_user_id != user_id && t->to_user_id != user_id)
		{
			log("invalid user_id %d for task %s", user_id, task_id.c_str());
			t->unlock(__LINE__);
			return;
		}
    
		switch (file_stat)
		{
			//george jiang changed the bracket on July 16th,2015
			case IM::BaseDefine::CLIENT_FILE_CANCEL:
			case IM::BaseDefine::CLIENT_FILE_FAILD:
				t->self_destroy = true;
				log("Canceling a task: %s by user_id: %d.....................", task_id.c_str(), user_id)
				; //do nothing
				break;
			//case IM::BaseDefine::CLIENT_FILE_UPLOAD_DONE:
			case IM::BaseDefine::CLIENT_FILE_DOWNLOAD_DONE:
			case IM::BaseDefine::CLIENT_FILE_DONE:
			case IM::BaseDefine::CLIENT_FILE_REFUSE:
			{
				// notify other client
				CFileConn *pConn = (CFileConn*)t->GetOpponentConn(user_id);
		
				if (pConn)
				{
					pConn->SendPdu(pPdu);
				}
				else
				{
					("invalid other side");
				}

				// release	
				t->self_destroy = true;
				log("task %s %d by user_id %d notify %d, erased", task_id.c_str(), file_stat, user_id,  t->GetOpponent(user_id));
			
				break;
			}
            
			default:
				break;
		}

		t->unlock(__LINE__);
		return;
	}
}

// data handler async
// if uuid not found
// return invalid uuid and close socket
// if offline or mobile task
// check if file size too large, write data and ++size
// if realtime task
// if transfer data

void CFileConn::_HandleClientFilePullFileReq(CImPdu *pPdu)
{
    if (!_IsAuth()) {
		return;
	}
    
    IM::File::IMFilePullDataReq msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));

	uint32_t user_id = msg.user_id();
    string task_id = msg.task_id();
    uint32_t mode = msg.trans_mode();
	uint32_t offset = msg.offset();
	uint32_t datasize = msg.data_size();
	uint32_t base_peer_id = msg.base_peer_id();
    
    IM::File::IMFilePullDataRsp msg2;
    msg2.set_result_code(1);
    msg2.set_task_id(task_id);
    msg2.set_user_id(user_id);
    msg2.set_offset(offset);
    msg2.set_data("");
    CImPdu pdu;
    pdu.SetPBMsg(&msg2);
    pdu.SetServiceId(SID_FILE);
    pdu.SetCommandId(CID_FILE_PULL_DATA_RSP);
    pdu.SetSeqNum(pPdu->GetSeqNum());

    // since the task had been created when the recver logged-in
    // we can find task in g_file_task_map here
	
    pthread_rwlock_wrlock(&g_file_task_map_lock); //lock

    TaskMap_t::iterator iter = g_file_task_map.find(task_id.c_str());
    if (g_file_task_map.end() == iter) {
        // invalid task id
        pthread_rwlock_unlock(&g_file_task_map_lock); //false unlock
        log("invalid task id %s:%d ", task_id.c_str(),task_id.length());
        SendPdu(&pdu);
        return;
    }

	//LIST_TRANSFER_TASK listTask = iter->second;
	transfer_task_t* t = NULL, *t1= NULL;
	for (IT_LIST_TRANSFER_TASK itTask = iter->second.begin(); itTask != iter->second.end(); ++itTask)
	{
		t = *itTask;
		if (t)
		{
			t->lock(__LINE__);
			if (!t->self_destroy && (t->to_user_id == user_id || t->from_user_id == user_id))
			{
				t->unlock(__LINE__);
				t1 = t;
				//break;
			}
			else
			{
				//log("TASK %s is destroyed when pulling data............", task_id.c_str());
				t->unlock(__LINE__);
				t = NULL;
			}
		}
	}

    pthread_rwlock_unlock(&g_file_task_map_lock); //unlock
    
	t = t1;
	if (t == NULL)
	{
		/*SendPdu(&pdu);
		log("illieage user %d for task %s", user_id, task_id.c_str());*/
		return;
	}

    t->lock(__LINE__);
	//log("start locking............");
 //   
 //   t->create_time = time(NULL);
 //   
 //   if (t->from_user_id != user_id /*for the realtime recver*/ && t->to_user_id != user_id /*for the offline download*/) {
 //       // invalid user
 //       log("illieage user %d for task %s", user_id, task_id.c_str());
 //       SendPdu(&pdu);
 //       
 //       t->unlock(__LINE__);
	//	log("unloking 1.............");
 //       return;
 //   }
    
    switch (mode)
	{
        case IM::BaseDefine::FILE_TYPE_ONLINE: // transfer request to sender
        {
            CFileConn* pConn = (CFileConn*)t->GetOpponentConn(user_id);
            pConn->SendPdu(pPdu);
            break;
        }
        case IM::BaseDefine::FILE_TYPE_OFFLINE: // for the offline mode
        {
            // find file use task id
            // send header info to user
            // send data
            // save path manager not used
            
            // save transfered info into task
            // like FILE*
            // transfered size
            
            
            // haven't been opened
            size_t size = 0;
            if (NULL == t->fp) {
                char save_path[BUFSIZ] = {0};
                //snprintf(save_path, BUFSIZ, "%s/%d", g_current_save_path, user_id); // those who can only get files under their user_id-dir
				//read a file from the directory where it placed(creating when uploading a file)
				//std::string str_user_id = idtourl(user_id);
				std::string str_user_id = idtourl(base_peer_id); //maybe the receiver forwards the file,user id is replaced with base_peer_id(file's original receiver ) updated by George Jinag on Sep 16th,2015
				snprintf(save_path, BUFSIZ, "%s/%s", g_current_save_path, str_user_id.c_str());
                
                int ret = mkdir(save_path, 0755);
                if ( (ret != 0) && (errno != EEXIST) ) {
                    log("mkdir failed for path: %s", save_path);
                    SendPdu(&pdu);
                    t->unlock(__LINE__);
					log("unlocking 2.................");
                    return;
                }
                
                strncat(save_path, "/", BUFSIZ);
               // strncat(save_path, task_id.c_str(), BUFSIZ); // use task_id as file name, in case of same-name file
				strncat(save_path, t->file_id.c_str(), BUFSIZ); // added on Dec 7th,2015
                
                // open at first time

				log("download file: %s", save_path);

                t->fp = fopen(save_path, "rb");  // save fp
                if (!t->fp) {
                    log("can not open file");
                    SendPdu(&pdu);
                    t->unlock(__LINE__);
					//log("unlocking 3....................");
                    return;
                }
                
                // read head at open
                if (NULL == t->file_head) {
                    t->file_head = new file_header_t;
                    if (NULL == t->file_head) {
                        // close to ensure next time will new file-header again
                        log("read file head failed.");

                        fclose(t->fp);
                        SendPdu(&pdu);
                        
                        t->unlock(__LINE__);
						//log("unlocking 4.................");
                        return;
                    }
                }
                
                size = fread(t->file_head, 1, sizeof(file_header_t), t->fp); // read header
                if (sizeof(file_header_t) > size) {
                    // close to ensure next time will read again
                    log("read file head failed.");
                    fclose(t->fp); // error to get header
					t->fp = NULL;
                    SendPdu(&pdu);
                    
                    t->unlock(__LINE__);
					//log("unlocking 5.......................");
                    return;
                } // the header won't be sent to recver, because the msg svr had already notified it.
                // if the recver needs to check it, it could be helpful
                // or sometime later, the recver needs it in some way.
            }
            
            // read data and send based on offset and datasize.
            char* tmpbuf = new char[datasize];
            if (NULL == tmpbuf) {
                // alloc mem failed
                log("alloc mem failed.");
                SendPdu(&pdu);
                t->unlock(__LINE__);
				//log("unlocking 6..........................");
                return;
            }
            memset(tmpbuf, 0, datasize);
            
            // offset file_header_t
            int iret = fseek(t->fp, sizeof(file_header_t) + offset, SEEK_SET); // read after file_header_t
            if (0 != iret) {
                log("seek offset failed.");
                SendPdu(&pdu);
                delete[] tmpbuf;
                t->unlock(__LINE__);
				//log("unlocking 7..........................");
                return;
                // offset failed
            }
			
			//george jiang inserted the bracket on Nov 17th,2015
			for (int i = 0; i < 10; i++)
			{
				//george jiang update the following on July 21st ,2015
				size = fread(tmpbuf, 1, datasize, t->fp);
				//log("SIZE= %d, DATASIZE: %d", size, datasize);
				if (0 != size)  //read required data
				{
					if (size > 0)
					{
						msg2.set_data(tmpbuf, size);
						msg2.set_result_code(0);
					}
					else
					{
						msg2.set_data("error", 5);
						msg2.set_result_code(1);
					}

					CImPdu pdu2;
					pdu2.SetPBMsg(&msg2);
					pdu2.SetServiceId(SID_FILE);
					pdu2.SetCommandId(CID_FILE_PULL_DATA_RSP);
					pdu2.SetSeqNum(pPdu->GetSeqNum());
					//pdu2.SetSeqNum(pPdu->GetSeqNum()); comment the line by george.jiang on June,26th,2015
					SendPdu(&pdu2);
					//t->transfered_size += size; // record transfered size for next time offset
					log("Pull Data Response: task id:%s, user id: %d, bytes sent;%d", task_id.c_str(), user_id, size);

					//after reading completely,we should make the pointer points to the end, so have to read followed 2 bytes,
					size = fread(tmpbuf, 1, 2, t->fp);
					delete[] tmpbuf;
					break;
				}
				else
				{
					t->unlock(__LINE__);
					sleep(2);
					t->lock(__LINE__);
				}
			}
			//else //failed to read or end of file
			if (feof(t->fp))
			{
				log("end of pulling data...........................");
				//delete[] tmpbuf;
                fclose(t->fp);
                t->fp = NULL;
             
                //_StatesNotify(CLIENT_FILE_DONE, task_id.c_str(), user_id, this);
				_StatesNotify(CLIENT_FILE_DOWNLOAD_DONE, task_id.c_str(), user_id, this);
                //Close(); //george commented the line on July 16th,2015
				//george jiang inserted the following lines on July 21st,2015.After receiving successfully, notify the sender immediately
				// notify the other side client
				CFileConn *pOpponentConn = (CFileConn*)t->GetOpponentConn(user_id);
				if (pOpponentConn)
				{
					//_StatesNotify(CLIENT_FILE_DONE, task_id.c_str(), t->GetOpponent(user_id), pOpponentConn);
					_StatesNotify(CLIENT_FILE_DOWNLOAD_DONE, task_id.c_str(), t->GetOpponent(user_id), pOpponentConn);
				}
				//end of inserting on July 21st,2015
                
                t->self_destroy = true;
                t->unlock(__LINE__);
				//log("unlocking after completely pulling data to client..................... ");
                return;
            }
            
            break;
        }
        default:
            break;
    }
  
    t->unlock(__LINE__); //just for non-offline mode
	//log("unlocking 8................................");
    return;
}

void CFileConn::_HandleClientFilePullFileRsp(CImPdu *pPdu)
{
	//log("prepare for handling data pulling response 1..............");
    if (!_IsAuth()) {
		return;
	}
    
	//log("prepare for handling data pulling response 2..............");
    IM::File::IMFilePullDataRsp msg;
    CHECK_PB_PARSE_MSG(msg.ParseFromArray(pPdu->GetBodyData(), pPdu->GetBodyLength()));

	uint32_t user_id = msg.user_id();
    string task_id = msg.task_id();
	uint32_t offset = msg.offset();
	uint32_t data_size = msg.data().length();
    
	//log("prepare for handling data pulling response 3..............");
	//LIST_TRANSFER_TASK listTask;
    pthread_rwlock_wrlock(&g_file_task_map_lock); //lock

	//log("Locking the task.................");
    TaskMap_t::iterator iter = g_file_task_map.find(task_id.c_str());
    if (g_file_task_map.end() == iter) {
        // invalid task id
		//log("Unlocking the task in 1.................");
        pthread_rwlock_unlock(&g_file_task_map_lock); //false unlock
        return;
    }
	
	//listTask = iter->second;
	transfer_task_t* t = NULL, *t1 = NULL;
	int nCount = 0;
	for (IT_LIST_TRANSFER_TASK itTask = iter->second.begin(); itTask != iter->second.end(); ++itTask)
	{
		++nCount;
		t = *itTask;
		if (t)
		{
			t->lock(__LINE__);
			if (!t->self_destroy && (t->from_user_id == user_id || t->to_user_id == user_id))
			{
				t->unlock(__LINE__);
				t1 = t;
				log("INDEX: %d, FROM USER: %d, TO USER: %d, CURRENT USER: %d", nCount, t->from_user_id, t->to_user_id, user_id);
				//break;
			}
			else
			{
				t->unlock(__LINE__);
				t = NULL;
			}
		}
	}

    pthread_rwlock_unlock(&g_file_task_map_lock); //unlock
	//log("Unlocking the task in 2.................");
    
	t = t1;
	if (t == NULL)
	{
		return;
	}

	
    t->lock(__LINE__);
	//log("Locking for pulling data response in 1....... ");
 //   t->create_time = time(NULL);
 //   
 //   if (t->from_user_id != user_id && t->to_user_id != user_id) {
 //       // invalid user
 //       t->unlock(__LINE__);
	//	log("Unlocking for pulling data response in 2...........");
 //       return;
 //   }
    
    switch (t->transfer_mode)
	{
        case FILE_TYPE_ONLINE: // transfer request to sender
        {
            CFileConn* pConn = (CFileConn*)t->GetOpponentConn(user_id);
            pConn->SendPdu(pPdu); /// send to recver
            break;
        }
        case FILE_TYPE_OFFLINE: /// this is the response to the server pull-data-req
        {
            if (t->upload_packages.size() <= 0) {
                log("FATAL ERROR");
                t->unlock(__LINE__);
				//log("Unlocking for pulling data response in 3...........");
                return;
            }
            
            // check if data size ok
            std::map<uint32_t, upload_package_t*>::iterator itPacks = t->upload_packages.find(offset);
            if (t->upload_packages.end() != itPacks)
			{ // offset right
                
                // check if data size ok
                if (data_size != itPacks->second->size) {
                    // the rsp's data size is different from req's
                    // refuse it or dynamic adjust
                    // do not adjust now, maybe later
                    uint32_t offset = itPacks->second->offset;
                    uint32_t size = itPacks->second->size;
                    // resend req
                    IM::File::IMFilePullDataReq msg2;
                    msg2.set_task_id(task_id);
                    msg2.set_user_id(user_id);
                    msg2.set_trans_mode((::IM::BaseDefine::FileType)t->transfer_mode);
                    msg2.set_offset(offset);
                    msg2.set_data_size(size);
					msg2.set_base_peer_id(0); //upload data, base peer id is set 0 by default
                    CImPdu pdu;
                    pdu.SetPBMsg(&msg2);
                    pdu.SetServiceId(SID_FILE);
                    pdu.SetCommandId(CID_FILE_PULL_DATA_REQ);
                    SendPdu(&pdu);
                    log("size not match, required size: %d, actual size: %d", itPacks->second->size, data_size);

                    t->unlock(__LINE__);
					//log("Unlocking for pulling data response in 4...........");
                    return;
                }
                
                // check if data-ptr OK
                if (NULL == itPacks->second->data)
				{
                    itPacks->second->data = new char[itPacks->second->size];
                    if (NULL == itPacks->second->data) {
                        uint32_t offset = itPacks->second->offset;
                        uint32_t size = itPacks->second->size;
                        
                        log("alloc mem failed");
                        // resend req
                        IM::File::IMFilePullDataReq msg2;
                        msg2.set_task_id(task_id);
                        msg2.set_user_id(user_id);
                        msg2.set_trans_mode((::IM::BaseDefine::FileType)t->transfer_mode);
                        msg2.set_offset(offset);
                        msg2.set_data_size(size);
						msg2.set_base_peer_id(0); //upload data, base peer id is set 0 by default
						CImPdu pdu;
                        pdu.SetPBMsg(&msg2);
                        pdu.SetServiceId(SID_FILE);
                        pdu.SetCommandId(CID_FILE_PULL_DATA_REQ);
                        SendPdu(&pdu);
                        t->unlock(__LINE__);
						//log("Unlocking for pulling data response in 5...........");
                        return;
                    }
                }
                
                // copy data
                memset(itPacks->second->data, 0,  itPacks->second->size);
                memcpy(itPacks->second->data, msg.data().c_str(), msg.data().length());
                ++t->transfered_size;
				log("CURRENT TRANSFERED SIZE: %d OFFSET: %d for TASK: %s", t->transfered_size, itPacks->second->offset, task_id.c_str());
            }
            
            // find which segment hasn't got data yet
            std::map<uint32_t, upload_package_t*>::iterator itor = t->upload_packages.begin();
			bool bFound = false;
			advance(itor, t->transfered_size);
			if (itor != t->upload_packages.end())
			{	
				bFound = true;
			}
           /* for ( ; itor != t->upload_packages.end(); ++itor)
			{
                if (NULL == itor->second->data)
				{
                    bFound = true;
                    break;
                }
            }*/
            if (!bFound)
			{
                // all packages recved
                //_StatesNotify(IM::BaseDefine::CLIENT_FILE_UPLOAD_DONE, task_id.c_str(), user_id, t->GetConn(user_id));
				//_StatesNotify(IM::BaseDefine::CLIENT_FILE_DONE, task_id.c_str(), user_id, t->GetConn(user_id));

                //Close();  george jiang commented the line on July 16th,2015
				log("successfully transferring data for task: %s", task_id.c_str());
                t->unlock(__LINE__);
				//log("Unlocking for pulling data response in 6...........");
                return;
            }
            // prepare to send req for this segment
            uint32_t next_offset = itor->second->offset;
            uint32_t next_size = itor->second->size;
            // send pull-data-req
            IM::File::IMFilePullDataReq msg2;
            msg2.set_task_id(task_id);
            msg2.set_user_id(user_id);
            msg2.set_trans_mode((::IM::BaseDefine::FileType)t->transfer_mode);
            msg2.set_offset(next_offset);
            msg2.set_data_size(next_size);
			msg2.set_base_peer_id(0); //upload data, base peer id is set 0 by default
            CImPdu pdu;
            pdu.SetPBMsg(&msg2);
            pdu.SetServiceId(SID_FILE);
            pdu.SetCommandId(CID_FILE_PULL_DATA_REQ);
            SendPdu(&pdu);
			//log("NEXT OFFSET: %d, NEXT SIZE: %d, TASK ID: %s", next_offset, next_size, task_id.c_str());
			//log("Sending a request for pulling data to client for TASK: %s", task_id.c_str());
            break;
        }
        default:
            break;
    }
    
    t->unlock(__LINE__);
	//log("Unlocking for pulling data response in 7...........");
    return;
}

int CFileConn::_StatesNotify(int state, const char* task_id, uint32_t user_id, CImConn* conn)
{
    CFileConn* pConn = (CFileConn*)conn;
    IM::File::IMFileState msg;
    msg.set_state((::IM::BaseDefine::ClientFileState)state);
    msg.set_task_id(task_id);
    msg.set_user_id(user_id);
    CImPdu pdu;
    pdu.SetPBMsg(&msg);
	pdu.SetServiceId(SID_FILE);
	pdu.SetCommandId(CID_FILE_STATE);
    
    pConn->SendPdu(&pdu);
    log("notify to user %d state %d task %s", user_id, state, task_id);
    return 0;
}

int CFileConn::_PreUpload(const char* task_id)
{
    pthread_rwlock_wrlock(&g_file_task_map_lock); //lock

    TaskMap_t::iterator iter = g_file_task_map.find(task_id);
    if (g_file_task_map.end() == iter) {
        pthread_rwlock_unlock(&g_file_task_map_lock); //false unlock
        log("failed to find task %s in task map", task_id);
        return -1;
    }

    transfer_task_t* t = NULL;

	//LIST_TRANSFER_TASK listTask = iter->second;
	int nCount = 0;
	for (IT_LIST_TRANSFER_TASK itTask = iter->second.begin(); itTask != iter->second.end(); ++itTask)
	{
		t = *itTask;  //the last node is the latest
		++nCount;
	}
	log("CURRENT NODE: %d, TOTAL NODES:%d", nCount, iter->second.size());
    pthread_rwlock_unlock(&g_file_task_map_lock); //unlock

	   
	if (t)
	{
		t->lock(__LINE__);

		if (t->self_destroy)
		{
			t->unlock(__LINE__);
			return -1;
		}
	}
	else
	{
		return -1;
	}

    char save_path[BUFSIZ];
    std::string str_user_id = idtourl(t->to_user_id);
    snprintf(save_path, BUFSIZ, "%s/%s", g_current_save_path, str_user_id.c_str());
    int ret = mkdir(save_path, 0755);
    if ( (ret != 0) && (errno != EEXIST) ) {
        log("mkdir failed for path: %s", save_path);
        
        t->self_destroy = true;
		t->unlock(__LINE__);
        return -2;
    }
    
    // save as g_current_save_path/to_id_url/task_id
    strncat(save_path, "/", BUFSIZ);
    //strncat(save_path, t->task_id.c_str(), BUFSIZ);
	strncat(save_path, t->file_id.c_str(), BUFSIZ); //change on Dec 7th, 2015
    
	log("upload file: %s", save_path);

	if (access(save_path, F_OK) == 0)
	{
		remove(save_path);
		log("file exists,deleting it first before reuploading..............");
	}

    t->fp = fopen(save_path, "ab+");
    if (!t->fp) {
        log("open file for write failed");
        
        t->self_destroy = true;
		t->unlock(__LINE__);
        return -3;
    }
    
	if (t->upload_packages.size() == 0)
	{
		uint32_t total_packages = t->file_size / SEGMENT_SIZE;
		for (uint32_t cnt = 0; cnt < total_packages; ++cnt) {
			upload_package_t* package = new upload_package_t(cnt, cnt * SEGMENT_SIZE, SEGMENT_SIZE);
			if (NULL == t) {
				log("create upload packages failed");

				t->self_destroy = true;
				t->unlock(__LINE__);
				return -4;
			}
			t->upload_packages.insert(std::make_pair(package->offset, package));
		}

		uint32_t last_piece = t->file_size % SEGMENT_SIZE;
		if (last_piece) {
			total_packages += 1;
			upload_package_t* package = new upload_package_t(t->upload_packages.size(), t->file_size - last_piece, last_piece);
			if (NULL == package) {
				log("create upload package failed");

				t->self_destroy = true;
				t->unlock(__LINE__);
				return -5;
			}
			t->upload_packages.insert(std::make_pair(package->offset, package));
		}
		log("NEW TASK: %s, PACKAGE SIZE: %d", task_id, t->upload_packages.size());
	}

	t->unlock(__LINE__);
    return 0;
}

void CFileConn::_HandleGetServerAddressReq(CImPdu* pPdu)
{
    IM::Server::IMFileServerIPRsp msg;
    for (auto ip_addr_tmp : g_addr)
    {
        auto ip_addr = msg.add_ip_addr_list();
        *ip_addr = ip_addr_tmp;
    }
    CImPdu pdu;
    pdu.SetPBMsg(&msg);
    pdu.SetServiceId(SID_OTHER);
    pdu.SetCommandId(CID_OTHER_FILE_SERVER_IP_RSP);
    pdu.SetSeqNum(pPdu->GetSeqNum());
	SendPdu(&pdu);
	return;
}
/// yunfan add end
