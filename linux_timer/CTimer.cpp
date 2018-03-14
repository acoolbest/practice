#include "CTimer.h"
#include <sys/select.h>
#include <time.h>
#include <pthread.h>
#include <iostream>
using namespace std;

std::vector<CEvent>  g_EventList;
extern CTimer *g_Timer;

CTimer::CTimer(): m_second(0), m_microsecond(0){}

CTimer::CTimer(unsigned long second, unsigned long microsecond) :
				m_second(second), m_microsecond(microsecond)
{}

CTimer::~CTimer(){
    m_second     = 0;
    m_microsecond = 0;
}

void CTimer::SetTimer(unsigned long second, unsigned long microsecond,int EventID){
    std::vector<CEvent>::iterator it = g_EventList.begin();
    for(;it!= g_EventList.end(); it++){
        if((*it).Newtimer.EventID == EventID){
            KillTimer(EventID);
            break;
        }
    }

    m_second = second;      
    m_microsecond = microsecond;

    TimeInfo *newtime   = new TimeInfo;
    newtime->EventID    = EventID;
    newtime->second     = m_second;
    newtime->microsecond = m_microsecond;

    pthread_create(&thread_timer, NULL, thread_proc, (void*)newtime);
	
    CEvent *p_event = new CEvent;
    p_event->threadid = thread_timer;   
    p_event->Newtimer.EventID = newtime->EventID;
    p_event->Newtimer.second = newtime->second;
    p_event->Newtimer.microsecond = newtime->microsecond;
    g_EventList.push_back(*p_event);
    delete p_event;
}

void CTimer::KillTimer(int EventID){
    pthread_t  threadid;
	bool flag = false;
	
    std::vector<CEvent>::iterator it = g_EventList.begin();
    for(;it!=g_EventList.end(); it++){
        if((*it).Newtimer.EventID == EventID){
            flag = true;
            threadid = it->threadid;
            break;
        }
    }
    if(flag){
        m_second = 0;
        m_microsecond = 0;
        g_EventList.erase(it);
        pthread_cancel(threadid);
        pthread_join(threadid, NULL); 
    }
}

void CTimer::OnTimer(int EventID){
	cout <<"event "<<EventID<<": is on time"<<endl;
}

void  *thread_proc(void *arg)
{
	TimeInfo pa = *(TimeInfo*)arg;
	delete (TimeInfo*)arg;

	while (true){
		struct timeval tempval;
		tempval.tv_sec  = pa.second;
		tempval.tv_usec = pa.microsecond;
		select(0, NULL, NULL, NULL, &tempval);
		g_Timer->OnTimer(pa.EventID);
		pthread_testcancel();
	}
}


