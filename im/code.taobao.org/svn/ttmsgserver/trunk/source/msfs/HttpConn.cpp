/*================================================================
 *   Copyright (C) 2014 All rights reserved.
 *   
 *   文件名称：HttpConn.cpp
 *   创 建 者：Zhang Yuanhao
 *   邮    箱：bluefoxah@gmail.com
 *   创建日期：2014年07月29日
 *   描    述：
 *
 #include "HttpConn.h"
 ================================================================*/
#include "HttpConn.h"
#include "HttpParserWrapper.h"
#include "atomic.h"
#include <Magick++.h>
#include <sys/syscall.h>
using namespace Magick;

static HttpConnMap_t g_http_conn_map;
//#define CUT_PIC_BASE_HEIGHT	160
//#define CUT_PIC_BASE_WIDTH	120
//change from 160,120 to 300,300 on Nov 30th,2015
#define CUT_PIC_BASE_HEIGHT	300
#define CUT_PIC_BASE_WIDTH	300


//just for test, added by george on Oct 20th,2015
typedef struct transduration
{
	time_t m_unStartTime;
	time_t m_unEndTime;
	struct timeval m_struStartTime;
	struct timeval m_struEndTime;
}TRANSDURATION;

typedef map<uint32_t, TRANSDURATION> MAP_TRANSDURATION;
typedef MAP_TRANSDURATION::iterator IT_MAP_TRANSDURATION;

MAP_TRANSDURATION gmapDuration;
pthread_mutex_t mutex_lock = PTHREAD_MUTEX_INITIALIZER;
//end of testing code

// conn_handle 从0开始递增，可以防止因socket handle重用引起的一些冲突
static uint32_t g_conn_handle_generator = 0;
CLock CHttpConn::s_list_lock;
list<Response_t*> CHttpConn::s_response_pdu_list;

CHttpConn* FindHttpConnByHandle(uint32_t conn_handle)
{
    CHttpConn* pConn = NULL;
    HttpConnMap_t::iterator it = g_http_conn_map.find(conn_handle);
    if (it != g_http_conn_map.end())
    {
        pConn = it->second;
    }

    return pConn;
}

void httpconn_callback(void* callback_data, uint8_t msg, uint32_t handle,
        uint32_t uParam, void* pParam)
{
    NOTUSED_ARG(uParam);
    NOTUSED_ARG(pParam);

    // convert void* to uint32_t, oops
    uint32_t conn_handle = *((uint32_t*) (&callback_data));
    CHttpConn* pConn = FindHttpConnByHandle(conn_handle);
    if (!pConn)
    {
        return;
    }

    switch (msg)
    {
    case NETLIB_MSG_READ:
        pConn->OnRead();
        break;
    case NETLIB_MSG_WRITE:
        pConn->OnWrite();
        break;
    case NETLIB_MSG_CLOSE:
        pConn->OnClose();
        break;
    default:
        log("!!!httpconn_callback error msg: %d", msg);
        break;
    }
}

void http_conn_loop_callback(void* callback_data, uint8_t msg, uint32_t handle, void* pParam)
{
    CHttpConn::SendResponsePduList();
}

void http_conn_timer_callback(void* callback_data, uint8_t msg, uint32_t handle,
        void* pParam)
{
    CHttpConn* pConn = NULL;
    HttpConnMap_t::iterator it, it_old;
    uint64_t cur_time = get_tick_count();

    for (it = g_http_conn_map.begin(); it != g_http_conn_map.end();)
    {
        it_old = it;
        it++;

        pConn = it_old->second;
        pConn->OnTimer(cur_time);
    }
}

void init_http_conn()
{
    netlib_register_timer(http_conn_timer_callback, NULL, 1000);
    netlib_add_loop(http_conn_loop_callback, NULL);
	Magick::InitializeMagick(NULL);
}

//////////////////////////

CHttpTask::CHttpTask(Request_t request)
{
    m_ConnHandle = request.conn_handle;
    m_nMethod = request.method;
    m_strUrl = request.strUrl;
    m_strContentType = request.strContentType;
    m_pContent = request.pContent;
    m_nContentLen = request.nContentLen;
    m_strAccessHost = request.strAccessHost;
}

CHttpTask::~CHttpTask()
{
}

void CHttpTask::run()
{
    if(HTTP_GET == m_nMethod)
    {
        OnDownload();
    }
    else if(HTTP_POST == m_nMethod)
    {
		OnUpload();
    }
    else
    {
        char* pContent = new char[strlen(HTTP_RESPONSE_403)];
        snprintf(pContent, strlen(HTTP_RESPONSE_403), HTTP_RESPONSE_403);
        CHttpConn::AddResponsePdu(m_ConnHandle, pContent, strlen(pContent));
    }

    if(m_pContent != NULL)
    {
        delete [] m_pContent;
        m_pContent = NULL;
    }
}

void CHttpTask::OnUpload()
{
	time_t unStartTime = time(NULL), unEndTime = time(NULL);
	struct timeval struStartTime, struEndTime;
	gettimeofday(&struStartTime, NULL);

    //get the file original filename
    char *pContent = NULL;

    int nTmpLen = 0;
    const char* pPos = memfind(m_pContent, m_nContentLen, CONTENT_DISPOSITION, strlen(CONTENT_DISPOSITION));
    if (pPos != NULL)
    {
        nTmpLen = pPos - m_pContent;
        const char* pPos2 = memfind(pPos, m_nContentLen - nTmpLen, "filename=", strlen("filename="));
        if (pPos2 != NULL)
        {
            pPos = pPos2 + strlen("filename=") + 1;
            const char * pPosQuotes = memfind(pPos, m_nContentLen - nTmpLen, "\"", strlen("\""));
            int nFileNameLen = pPosQuotes - pPos;
                
            char szFileName[256];
            if(nFileNameLen <= 255)
            {
                memcpy(szFileName,  pPos, nFileNameLen);
                szFileName[nFileNameLen] = 0;

                    
                const char* pPosType = memfind(szFileName, nFileNameLen, ".", 1, false);
                if(pPosType != NULL)
                {
                    char szType[16];
                    int nTypeLen = nFileNameLen - (pPosType + 1 - szFileName);
                    if(nTypeLen <=15)
                    {
                        memcpy(szType, pPosType + 1, nTypeLen);
                        szType[nTypeLen] = 0;
                        log("upload file, file name:%s", szFileName);
                        char szExtend[32];
						//george added the followng 2 line on Sep 25th,2015
						bool bCutPic = false;
						char szExtend1[32];
						int nHeight1 = 0, nWidth1 = 0;
						int nTmpHeight = 0, nTmpWidth = 0;
                        const char* pPosExtend = memfind(szFileName, nFileNameLen, "_", 1, false);
                        if(pPosExtend != NULL)
                        {
                            const char* pPosTmp = memfind(pPosExtend, nFileNameLen - (pPosExtend + 1 - szFileName), "x", 1);
                            if(pPosTmp != NULL)
                            {
                                int nWidthLen = pPosTmp - pPosExtend - 1;
                                int nHeightLen = pPosType - pPosTmp - 1;
                                if(nWidthLen >= 0 && nHeightLen >= 0)
                                {
                                    int nWidth = 0;
                                    int nHeight = 0;
                                 /*   char szWidth[5], szHeight[5];
                                    if(nWidthLen <=4 && nHeightLen <=4)*/
									//change both width and height length from 4 to 16
									char szWidth[16], szHeight[16];
									if (nWidthLen <= 16 && nHeightLen <= 16)
                                    {
                                        memcpy(szWidth, pPosExtend + 1, nWidthLen);
                                        szWidth[nWidthLen] = 0;
                                        memcpy(szHeight, pPosTmp + 1, nHeightLen );
                                        szHeight[nHeightLen] = 0;
                                        nWidth = atoi(szWidth);
                                        nHeight = atoi(szHeight);

										//to judge if we should compress the picture.added by george on Sep 25th,2015
										if (nHeight == 0 || nWidth == 0)
										{
											szExtend[0] = 0;
										}
										else
										{
											if (nHeight <= CUT_PIC_BASE_HEIGHT && nWidth <= CUT_PIC_BASE_WIDTH)
											{
												snprintf(szExtend, sizeof(szExtend), "%dx%d.%s", nWidth, nHeight, szType);
											}
											else
											{
												bCutPic = true;
												//added by george on Dec 1st ,2015. for cropping
												if (nHeight / nWidth > 3)
												{
													nTmpWidth = nWidth;
													nTmpHeight = nWidth * 3;
												}
												else if (nWidth / nHeight > 3)
												{
													nTmpHeight = nHeight;
													nTmpWidth = nHeight * 3;
												}

												int nHeight2 = 0, nWidth2 = 0;
												if (nTmpHeight != 0 && nTmpWidth != 0)
												{
													nHeight2 = nTmpHeight;
													nWidth2 = nTmpWidth;
												}
												else
												{
													nHeight2 = nHeight;
													nWidth2 = nWidth;
												}

												float fRatio1 = nHeight2*1.0f / CUT_PIC_BASE_HEIGHT;
												float fRatio2 = nWidth2*1.0f / CUT_PIC_BASE_WIDTH;
												if (fRatio1 > fRatio2)
												{
													nHeight1 = (int)(nHeight2 / fRatio1);
													nWidth1 = (int)(nWidth2 / fRatio1);
												}
												else
												{
													nWidth1 = (int)(nWidth2 / fRatio2);
													nHeight1 = (int)(nHeight2 / fRatio2);
												}
												//end of adding

											/*	float fRatio1 = nHeight*1.0f / CUT_PIC_BASE_HEIGHT;
												float fRatio2 = nWidth*1.0f / CUT_PIC_BASE_WIDTH;
												if (fRatio1 > fRatio2)
												{
													nHeight1 = (int)(nHeight / fRatio1);
													nWidth1 = (int)(nWidth / fRatio1);
												}
												else
												{
													nWidth1 = (int)(nWidth / fRatio2);
													nHeight1 = (int)(nHeight / fRatio2);
												}
*/
												//original file format is such as original size_adjusted size
												//compressed file format is such as original size_adjusted size_s
												memset(szExtend, 0, sizeof(szExtend));
												snprintf(szExtend, sizeof(szExtend), "%dx%d_%dx%d.%s", nWidth, nHeight, nWidth1, nHeight1, szType);
													
												//snprintf(szExtend1, sizeof(szExtend1), "%dx%d_%dx%d_s.%s", nWidth, nHeight, nWidth1, nHeight1, szType);
											}

											memset(szExtend1, 0, sizeof(szExtend1));
											snprintf(szExtend1, sizeof(szExtend1), "s.%s", szType);
										}
                                    }
									else
                                    {
                                        szExtend[0] = 0;
                                    }
                                }
                                else
                                {
                                    szExtend[0] = 0;
                                }
                            }
                            else{
                                szExtend[0] = 0;
                            }
                        }
                        else
                        {
                            szExtend[0] = 0;
                        }

                        //get the file content
                        size_t nPos = m_strContentType.find(BOUNDARY_MARK);
                        if (nPos != m_strContentType.npos)
                        {
                            const  char* pBoundary = m_strContentType.c_str() + nPos + strlen(BOUNDARY_MARK);
                            int nBoundaryLen = m_strContentType.length() - nPos - strlen(BOUNDARY_MARK);

                            pPos = memfind(m_pContent, m_nContentLen, pBoundary, nBoundaryLen);
                            if (NULL != pPos)
                            {
                                nTmpLen = pPos - m_pContent;
                                pPos = memfind(m_pContent + nTmpLen, m_nContentLen - nTmpLen, CONTENT_TYPE, strlen(CONTENT_TYPE));
                                if (NULL != pPos)
                                {
                                    nTmpLen = pPos - m_pContent;
                                    pPos = memfind(m_pContent + nTmpLen, m_nContentLen - nTmpLen, HTTP_END_MARK, strlen(HTTP_END_MARK));
                                    if (NULL != pPos)
                                    {
                                        nTmpLen = pPos - m_pContent;
                                        const char* pFileStart = pPos + strlen(HTTP_END_MARK);
                                        pPos2 = memfind(m_pContent + nTmpLen, m_nContentLen - nTmpLen, pBoundary, nBoundaryLen);
                                        if (NULL != pPos2)
                                        {
                                            int64_t nFileSize = pPos2 - strlen(HTTP_END_MARK) - pFileStart;
											if (nFileSize <= HTTP_UPLOAD_MAX)
											{
                                                char filePath[512] ={ 0 };
												char szTempFilePath[512] = { 0 };
                                                if(strlen(szExtend) != 0)
                                                {
                                                    g_fileManager->uploadFile(szType, pFileStart, nFileSize, filePath, NULL, szExtend); //upload original file
													log("FILE PATH %s", filePath);

													strcpy(szTempFilePath, filePath);
													char *pPosition = strchr(szTempFilePath, '.');
													if (pPosition)
													{
														*pPosition = 0;
														pPosition = NULL;
													}

													if (bCutPic) //we should compress the file
													{
														/*strcpy(szTempFilePath, filePath);
														char *pPosition = strchr(szTempFilePath, '.');
														if (pPosition)
														{
															*pPosition = 0;
															pPosition = NULL;
														}*/

														//create a temp file and write original data
														try
														{
															struct timeval struCurrTime;
															gettimeofday(&struCurrTime, NULL);
															string strTempFile = longlong2string((long long)pthread_self()) + "_" + longlong2string((long long)(struCurrTime.tv_sec * 1000 * 1000 + struCurrTime.tv_usec)) + "_" + string("temp1.") + szType;
															File tmpFile = File(strTempFile.c_str());

															tmpFile.create();
															tmpFile.write(0, nFileSize, pFileStart);
															tmpFile.close();


															log("FILE1: %s", strTempFile.c_str());

															 
															if (nTmpWidth != 0 && nTmpHeight != 0) //cropping file
															{
																Image CropFile;
																CropFile.read(strTempFile.c_str());
																CropFile.crop(Geometry(nTmpWidth, nTmpHeight, 0, 0));
																strTempFile = longlong2string((long long)pthread_self()) + "_" + longlong2string((long long)(struCurrTime.tv_sec * 1000 * 1000 + struCurrTime.tv_usec)) + "_" + string("temp3.") + szType;
																CropFile.write(strTempFile.c_str());
																log("Cropping File: %s", strTempFile.c_str());
															}

															//compressing the temp file to a new size file
															//Magick::InitializeMagick(NULL);
															Image master(strTempFile.c_str());
															Image second = master;

															char szSize[16];
															memset(szSize, 0, sizeof(szSize));
															snprintf(szSize, sizeof(szSize), "%dx%d", nWidth1, nHeight1);
															second.zoom(szSize); //change size
															log("NEW SIZE: %s", szSize);
															remove(strTempFile.c_str());//remove the temp file

															strTempFile = longlong2string((long long)pthread_self()) + "_" + longlong2string((long long)(struCurrTime.tv_sec*1000*1000+struCurrTime.tv_usec)) + "_" + string("temp2_") + szSize + "." + szType;
															second.write(strTempFile.c_str()); //new size file
															
															tmpFile.remove(); //remove temp file1
														
															//open zipped file and read data 
															File tmpFile2 = File(strTempFile.c_str());
															int nTmpSize = 0;
															nTmpSize = File::getFileSize((char*)strTempFile.c_str());
															log("FILE2: %s, SIZE: %d", strTempFile.c_str(), nTmpSize);

															char *pszData = NULL;
															if (nTmpSize > 0)
															{
																pszData = new char[nTmpSize];
																if (pszData)
																{
																	memset(pszData, 0, nTmpSize);
																	tmpFile2.open();
																	tmpFile2.read(0, nTmpSize, pszData);
																	tmpFile2.close();

																	log("TEMP PATH: %s, EXTEND2: %s", szTempFilePath, szExtend1);
																	char szCutFilePath[512];
																	memset(szCutFilePath, 0, sizeof(szCutFilePath));
																	g_fileManager->uploadFile(szType, pszData, nTmpSize, szCutFilePath, szTempFilePath, szExtend1); //upload compressed file
																	log("FILE PATH1: %s", szCutFilePath);
																	delete[]pszData;
																	pszData = NULL;
																}
															}
															tmpFile2.remove();
														}
														catch (exception &e)
														{
															log("An exception occurred: %s", e.what());

														}
													}
													else //whatever,we should copy original file to a new one
													{
														char szNewFilePath[512];
														memset(szNewFilePath, 0, sizeof(szNewFilePath));
														g_fileManager->uploadFile(szType, pFileStart, nFileSize, szNewFilePath, szTempFilePath, szExtend1);
														log("SAME FILE and FILE PATH1: %s", szNewFilePath);
													}
                                                }
                                                else
												{
                                                    g_fileManager->uploadFile(szType, pFileStart, nFileSize, filePath, NULL);
													log("NO EXTEND,FILE PATH: %s", filePath);
                                                }
                                                char url[1024];
                                                snprintf(url, sizeof(url), "{\"error_code\":0,\"error_msg\": \"成功\",\"path\":\"%s\",\"url\":\"http://%s/%s\"}", filePath,m_strAccessHost.c_str(), filePath);
                                                uint32_t content_length = strlen(url);
                                                pContent = new char[HTTP_RESPONSE_HTML_MAX];
                                                snprintf(pContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, content_length,url);
                                                CHttpConn::AddResponsePdu(m_ConnHandle, pContent, strlen(pContent));
												log("URL: %s", url);
                                            }
                                        }
                                        else
                                        {
                                            char url[128];
                                            snprintf(url, sizeof(url), "{\"error_code\":8,\"error_msg\": \"格式错误\",\"path\":\"\",\"url\":\"\"}");
                                            log("%s",url);
                                            uint32_t content_length = strlen(url);
                                            pContent = new char[HTTP_RESPONSE_HTML_MAX];
                                            snprintf(pContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, content_length,url);
                                            CHttpConn::AddResponsePdu(m_ConnHandle, pContent, strlen(pContent));
                                        }
                                    }
                                    else
                                    {
                                        char url[128];
                                        snprintf(url, sizeof(url), "{\"error_code\":7,\"error_msg\": \"格式错误\",\"path\":\"\",\"url\":\"\"}");
                                        log("%s",url);
                                        uint32_t content_length = strlen(url);
                                        pContent = new char[HTTP_RESPONSE_HTML_MAX];
                                        snprintf(pContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, content_length,url);
                                        CHttpConn::AddResponsePdu(m_ConnHandle, pContent, strlen(pContent));
                                    }

                                }
                                else
                                {
                                    char url[128];
                                    snprintf(url, sizeof(url), "{\"error_code\":6,\"error_msg\": \"格式错误\",\"path\":\"\",\"url\":\"\"}");
                                    log("%s",url);
                                    uint32_t content_length = strlen(url);
                                    pContent = new char[HTTP_RESPONSE_HTML_MAX];
                                    snprintf(pContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, content_length,url);
                                    CHttpConn::AddResponsePdu(m_ConnHandle, pContent, strlen(pContent));
                                }
                            }
                            else
                            {
                                char url[128];
                                snprintf(url, sizeof(url), "{\"error_code\":5,\"error_msg\": \"格式错误\",\"path\":\"\",\"url\":\"\"}");
                                log("%s",url);
                                uint32_t content_length = strlen(url);
                                pContent = new char[HTTP_RESPONSE_HTML_MAX];
                                snprintf(pContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, content_length,url);
                                CHttpConn::AddResponsePdu(m_ConnHandle, pContent, strlen(pContent));
                            }
                        }
                        else
                        {
                            char url[128];
                            snprintf(url, sizeof(url), "{\"error_code\":4,\"error_msg\": \"格式错误\",\"path\":\"\",\"url\":\"\"}");
                            log("%s",url);
                            uint32_t content_length = strlen(url);
                            pContent = new char[HTTP_RESPONSE_HTML_MAX];
                            snprintf(pContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, content_length,url);
                            CHttpConn::AddResponsePdu(m_ConnHandle, pContent, strlen(pContent));
                        }
                    }
                    else{
                        char url[128];
                        snprintf(url, sizeof(url), "{\"error_code\":9,\"error_msg\": \"格式错误\",\"path\":\"\",\"url\":\"\"}");
                        log("%s",url);
                        uint32_t content_length = strlen(url);
                        pContent = new char[HTTP_RESPONSE_HTML_MAX];
                        snprintf(pContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, content_length,url);
                        CHttpConn::AddResponsePdu(m_ConnHandle, pContent, strlen(pContent));
                    }
                }
                else{
                    char url[128];
                    snprintf(url, sizeof(url), "{\"error_code\":10,\"error_msg\": \"格式错误\",\"path\":\"\",\"url\":\"\"}");
                    log("%s",url);
                    uint32_t content_length = strlen(url);
                    pContent = new char[HTTP_RESPONSE_HTML_MAX];
                    snprintf(pContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, content_length,url);
                    CHttpConn::AddResponsePdu(m_ConnHandle, pContent, strlen(pContent));
                }
            }else
            {
                char url[128];
                snprintf(url, sizeof(url), "{\"error_code\":11,\"error_msg\": \"格式错误\",\"path\":\"\",\"url\":\"\"}");
                log("%s",url);
                uint32_t content_length = strlen(url);
                pContent = new char[HTTP_RESPONSE_HTML_MAX];
                snprintf(pContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, content_length,url);
                CHttpConn::AddResponsePdu(m_ConnHandle, pContent, strlen(pContent));
            }
        }
        else
        {
            char url[128];
            snprintf(url, sizeof(url), "{\"error_code\":3,\"error_msg\": \"格式错误\",\"path\":\"\",\"url\":\"\"}");
            log("%s",url);
            uint32_t content_length = strlen(url);
            pContent = new char[HTTP_RESPONSE_HTML_MAX];
            snprintf(pContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, content_length,url);
            CHttpConn::AddResponsePdu(m_ConnHandle, pContent, strlen(pContent));
        }
    }
    else
    {
        char url[128];
        snprintf(url, sizeof(url), "{\"error_code\":2,\"error_msg\": \"格式错误\",\"path\":\"\",\"url\":\"\"}");
        log("%s",url);
        uint32_t content_length = strlen(url);
        pContent = new char[HTTP_RESPONSE_HTML_MAX];
        snprintf(pContent, HTTP_RESPONSE_HTML_MAX, HTTP_RESPONSE_HTML, content_length,url);
        CHttpConn::AddResponsePdu(m_ConnHandle, pContent, strlen(pContent));
    }

	/*unEndTime = time(NULL);
	gettimeofday(&struEndTime, NULL);
	unsigned int unDeficit = unEndTime - unStartTime;
	char szStartTime[32], szEndTime[32];
	memset(szStartTime, 0, sizeof(szStartTime));
	memset(szEndTime, 0, sizeof(szEndTime));
	strftime(szStartTime, sizeof(szStartTime)-1, "%Y/%m/%d %H:%M:%S", localtime(&unStartTime));
	strftime(szEndTime, sizeof(szEndTime)-1, "%Y/%m/%d %H:%M:%S", localtime(&unEndTime));
	if (unDeficit > 0)
	{
		log("UPLOADING START TIME: %s\tEND TIME: %s\t, DEFICIT: %d seconds", szStartTime, szEndTime, unDeficit);
	}
	else
	{
		float fDeficit = (struEndTime.tv_sec - struStartTime.tv_sec)*1000 + (struEndTime.tv_usec - struStartTime.tv_usec)*1.0f/1000;
		log("UPLOADING START TIME: %s\tEND TIME: %s\t, DEFICIT: %f milliseconds", szStartTime, szEndTime, fDeficit);
	}*/
	
}

void  CHttpTask::OnDownload()
{
	//just for test
	TRANSDURATION struDownloadTime;
	memset(&struDownloadTime, 0, sizeof(struDownloadTime));
	struDownloadTime.m_unStartTime = time(NULL);
	gettimeofday(&struDownloadTime.m_struStartTime, NULL);
	//end of test

    uint32_t  nFileSize = 0;
    int32_t nTmpSize = 0;
    string strPath;
    if(g_fileManager->getAbsPathByUrl(m_strUrl, strPath ) == 0)
    {
		log("URL: %s Path:%s", m_strUrl.c_str(), strPath.c_str());
        nTmpSize = File::getFileSize((char*)strPath.c_str());
		if (nTmpSize == -1) //do not find compressed file
		{
			size_t nPos = m_strUrl.find("_s");  //find key for compressed file
			if (nPos != string::npos)
			{
				//m_strUrl = m_strUrl.substr(0, nPos) + m_strUrl.substr(nPos + 2, m_strUrl.length() - nPos - 2);
				m_strUrl = m_strUrl.substr(0, nPos) + m_strUrl.substr(nPos + 2, m_strUrl.length() - nPos - 2);
				nPos = strPath.find("_s");
				string strTmpUrl;
				if (nPos != string::npos)
				{
					strTmpUrl = strPath.substr(0, nPos) + strPath.substr(nPos + 2, strPath.length() - nPos - 2);
					nTmpSize = File::getFileSize((char*)strTmpUrl.c_str()); //find original file
					log("%s NO CUT PICS,GET ORIGINAL PATH: %s", strPath.c_str(), strTmpUrl.c_str());
				}
			}
		}

        if(nTmpSize != -1)
        {
            char szResponseHeader[1024];
            size_t nPos = strPath.find_last_of(".");
            string strType = strPath.substr(nPos + 1, strPath.length() - nPos);
            if(strType == "jpg" || strType == "JPG" || strType == "jpeg" || strType == "JPEG" || strType == "png" || strType == "PNG" || strType == "gif" || strType == "GIF")
            {
                snprintf(szResponseHeader, sizeof(szResponseHeader), HTTP_RESPONSE_IMAGE, nTmpSize, strType.c_str());
            }
            else
            {
                snprintf(szResponseHeader,sizeof(szResponseHeader), HTTP_RESPONSE_EXTEND, nTmpSize);
            }
            int nLen = strlen(szResponseHeader);
            char* pContent = new char[nLen + nTmpSize];
            memcpy(pContent, szResponseHeader, nLen);
            g_fileManager->downloadFileByUrl((char*)m_strUrl.c_str(), pContent + nLen, &nFileSize);
            int nTotalLen = nLen + nFileSize;
            CHttpConn::AddResponsePdu(m_ConnHandle, pContent, nTotalLen);

			//just for test
			pthread_mutex_lock(&mutex_lock);
			gmapDuration[m_ConnHandle] = struDownloadTime;
			pthread_mutex_unlock(&mutex_lock);
        }
        else
        {
            int nTotalLen = strlen(HTTP_RESPONSE_404);
            char* pContent = new char[nTotalLen];
            snprintf(pContent, nTotalLen, HTTP_RESPONSE_404);
            CHttpConn::AddResponsePdu(m_ConnHandle, pContent, nTotalLen);
            log("File size is invalied\n");
                
        }
    }
    else
    {
        int nTotalLen = strlen(HTTP_RESPONSE_500);
		char* pContent = new char[nTotalLen];
		snprintf(pContent, nTotalLen, HTTP_RESPONSE_500);
		CHttpConn::AddResponsePdu(m_ConnHandle, pContent, nTotalLen);
    }

}

CHttpConn::CHttpConn()
{
    m_busy = false;
    m_sock_handle = NETLIB_INVALID_HANDLE;
    m_state = CONN_STATE_IDLE;

    m_last_send_tick = m_last_recv_tick = get_tick_count();
    m_conn_handle = ++g_conn_handle_generator;
    if (m_conn_handle == 0)
    {
        m_conn_handle = ++g_conn_handle_generator;
    }

    log("CHttpConn, handle=%u", m_conn_handle);
}

CHttpConn::~CHttpConn()
{
    log("~CHttpConn, handle=%u", m_conn_handle);
}

int CHttpConn::Send(void* data, int len)
{
    m_last_send_tick = get_tick_count();

    if (m_busy)
    {
        m_out_buf.Write(data, len);
        return len;
    }

    int ret = netlib_send(m_sock_handle, data, len);
    if (ret < 0)
        ret = 0;

    if (ret < len)
    {
        m_out_buf.Write((char*) data + ret, len - ret);
        m_busy = true;
        log("not send all, remain=%d", m_out_buf.GetWriteOffset());
    }
    else
    {
        OnSendComplete();
    }

    return len;
}

void CHttpConn::Close()
{
    m_state = CONN_STATE_CLOSED;

    g_http_conn_map.erase(m_conn_handle);
    netlib_close(m_sock_handle);

    ReleaseRef();
}

void CHttpConn::OnConnect(net_handle_t handle)
{
    printf("OnConnect, handle=%d", handle);
    m_sock_handle = handle;
    m_state = CONN_STATE_CONNECTED;
    g_http_conn_map.insert(make_pair(m_conn_handle, this));

    netlib_option(handle, NETLIB_OPT_SET_CALLBACK, (void*) httpconn_callback);
    netlib_option(handle, NETLIB_OPT_SET_CALLBACK_DATA,
            reinterpret_cast<void *>(m_conn_handle));
    netlib_option(handle, NETLIB_OPT_GET_REMOTE_IP, (void*) &m_peer_ip);
}

void CHttpConn::OnRead()
{
	for (;;)
    {
        uint32_t free_buf_len = m_in_buf.GetAllocSize()
                - m_in_buf.GetWriteOffset();
        if (free_buf_len < READ_BUF_SIZE + 1)
            m_in_buf.Extend(READ_BUF_SIZE + 1);

        int ret = netlib_recv(m_sock_handle,
                m_in_buf.GetBuffer() + m_in_buf.GetWriteOffset(),
                READ_BUF_SIZE);
        if (ret <= 0)
            break;

        m_in_buf.IncWriteOffset(ret);

        m_last_recv_tick = get_tick_count();
    }

    // 每次请求对应一个HTTP连接，所以读完数据后，不用在同一个连接里面准备读取下个请求
    char* in_buf = (char*) m_in_buf.GetBuffer();
    uint32_t buf_len = m_in_buf.GetWriteOffset();
    in_buf[buf_len] = '\0';

    //log("OnRead, buf_len=%u, conn_handle=%u", buf_len, m_conn_handle); // for debug


    m_HttpParser.ParseHttpContent(in_buf, buf_len);

    if (m_HttpParser.IsReadAll())
    {
        string strUrl = m_HttpParser.GetUrl();
        log("IP:%s access:%s", m_peer_ip.c_str(), strUrl.c_str());
        if (strUrl.find("..") != strUrl.npos) {
            Close();
            return;
        }
        m_access_host = m_HttpParser.GetHost();
        if (m_HttpParser.GetContentLen() > HTTP_UPLOAD_MAX)
        {
            // file is too big
            log("content  is too big");
            char url[128];
            snprintf(url, sizeof(url), "{\"error_code\":1,\"error_msg\": \"上传文件过大\",\"url\":\"\"}");
            log("%s",url);
            uint32_t content_length = strlen(url);
            char pContent[1024];
            snprintf(pContent, sizeof(pContent), HTTP_RESPONSE_HTML, content_length,url);
            Send(pContent, strlen(pContent));
            return;
        }

        int nContentLen = m_HttpParser.GetContentLen();
        char* pContent = NULL;
        if(nContentLen != 0)
        {
            try {
                pContent =new char[nContentLen];
                memcpy(pContent, m_HttpParser.GetBodyContent(), nContentLen);
            }
            catch(...)
            {
                log("not enough memory");
                char szResponse[HTTP_RESPONSE_500_LEN + 1];
                snprintf(szResponse, HTTP_RESPONSE_500_LEN, "%s", HTTP_RESPONSE_500);
                Send(szResponse, HTTP_RESPONSE_500_LEN);
                return;
            }
        }
        Request_t request;
        request.conn_handle = m_conn_handle;
        request.method = m_HttpParser.GetMethod();;
        request.nContentLen = nContentLen;
        request.pContent = pContent;
        request.strAccessHost = m_HttpParser.GetHost();
        request.strContentType = m_HttpParser.GetContentType();
        request.strUrl = m_HttpParser.GetUrl() + 1;
        CHttpTask* pTask = new CHttpTask(request);

        if(HTTP_GET == m_HttpParser.GetMethod())
        {
        	g_GetThreadPool.AddTask(pTask);
			log("GETTING DATA................");
        }
        else
        {
        	g_PostThreadPool.AddTask(pTask);
        }
    }
}

void CHttpConn::OnWrite()
{
    if (!m_busy)
        return;

    int ret = netlib_send(m_sock_handle, m_out_buf.GetBuffer(),
            m_out_buf.GetWriteOffset());
    if (ret < 0)
        ret = 0;

    int out_buf_size = (int) m_out_buf.GetWriteOffset();

    m_out_buf.Read(NULL, ret);

    if (ret < out_buf_size)
    {
        m_busy = true;
//        log("not send all, remain=%d", m_out_buf.GetWriteOffset());
    } else
    {
        m_busy = false;
        OnSendComplete();
    }
}

void CHttpConn::OnClose()
{
    Close();
}

void CHttpConn::OnTimer(uint64_t curr_tick)
{
    if (curr_tick > m_last_recv_tick + HTTP_CONN_TIMEOUT)
    {
        log("HttpConn timeout, handle=%d", m_conn_handle);
        Close();
    }
}

void CHttpConn::AddResponsePdu(uint32_t conn_handle, char* pContent, int nLen)
{
    Response_t* pResp = new Response_t;
    pResp->conn_handle = conn_handle;
    pResp->pContent = pContent;
    pResp->content_len = nLen;

    s_list_lock.lock();
    s_response_pdu_list.push_back(pResp);
    s_list_lock.unlock();
}

void CHttpConn::SendResponsePduList()
{
    s_list_lock.lock();
    while (!s_response_pdu_list.empty())
	{
        Response_t* pResp = s_response_pdu_list.front();
        s_response_pdu_list.pop_front();
        s_list_lock.unlock();

        CHttpConn* pConn = FindHttpConnByHandle(pResp->conn_handle);
		if (pConn)
		{
			int nBytes = pConn->Send(pResp->pContent, pResp->content_len);
			log("DATA LEN: %d, SOCKET: %d, BYTES SENT: %d", pResp->content_len, pResp->conn_handle, nBytes);

			////start test
			//pthread_mutex_lock(&mutex_lock);
			//IT_MAP_TRANSDURATION iter = gmapDuration.find(pResp->conn_handle);
			//if (iter != gmapDuration.end())
			//{
			//	iter->second.m_unEndTime = time(NULL);
			//	gettimeofday(&iter->second.m_struEndTime, NULL);
			//	unsigned int unDeficit = iter->second.m_unEndTime - iter->second.m_unStartTime;
			//	char szStartTime[32], szEndTime[32];
			//	memset(szStartTime, 0, sizeof(szStartTime));
			//	memset(szEndTime, 0, sizeof(szEndTime));
			//	strftime(szStartTime, sizeof(szStartTime)-1, "%Y/%m/%d %H:%M:%S", localtime(&iter->second.m_unStartTime));
			//	strftime(szEndTime, sizeof(szEndTime)-1, "%Y/%m/%d %H:%M:%S", localtime(&iter->second.m_unEndTime));
			//	/*	if (unDeficit > 0)
			//		{
			//		log("DOWNLOADING START TIME: %s\tEND TIME: %s\t, DEFICIT: %d seconds", szStartTime, szEndTime, unDeficit);
			//		}
			//		else*/
			//	{
			//		float fDeficit = ((iter->second.m_struEndTime.tv_sec - iter->second.m_struStartTime.tv_sec) * 1000 + (iter->second.m_struEndTime.tv_usec - iter->second.m_struStartTime.tv_usec)*1.0f / 1000) / 1000;
			//		log("DOWNLOADING START TIME: %s\tEND TIME: %s\t, DEFICIT: %f seconds", szStartTime, szEndTime, fDeficit);
			//	}

			//}
			//pthread_mutex_unlock(&mutex_lock);
			////end test

		}


        if(pResp->pContent != NULL)
        {
            delete [] pResp->pContent;
            pResp->pContent = NULL;
        }
        if(pResp != NULL)
        {
            delete pResp;
            pResp = NULL;
        }

        s_list_lock.lock();
    }

	s_list_lock.unlock();
}

void CHttpConn::OnSendComplete()
{
    Close();
}
