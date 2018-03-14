/*
************************************************************************
* CTimer.h
*
* Created on: 2015-4-15
* Author: wuxian

* This class provide a timer to finish some works.
* Call SetTimer() to set the timer_interval and enable it
* and call KillTimer() to stop it.
* The work you want to do should be written on OnTimer
* function.
*************************************************************************
*/

#ifndef CTIMER_H_
#define CTIMER_H_

#include <pthread.h>
#include <sys/time.h>
#include <vector>
void *thread_proc(void *arg);

typedef struct _TimeInfo{
	int EventID;
	unsigned long second;
	unsigned long microsecond;
}TimeInfo;
  
class CEvent
{
public:
    CEvent(){};
    virtual ~CEvent(){};
	
    pthread_t threadid;
    TimeInfo  Newtimer;
private:

};

class CTimer
{
private:
    pthread_t thread_timer;
    unsigned long m_second;
	unsigned long m_microsecond;

public:
    CTimer();
    CTimer(unsigned long second, 
		   unsigned long microsecond);
		   
    virtual ~CTimer();
	
    void SetTimer(unsigned long second,
				  unsigned long microsecond,
				  int EventID); 
				  
    void KillTimer(int EventID);
	static void OnTimer(int EventID);
};

#endif 

