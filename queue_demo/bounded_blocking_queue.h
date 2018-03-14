#ifndef _BOUNDED_BLOCKING_QUEUE_H_
#define _BOUNDED_BLOCKING_QUEUE_H_
#include <pthread.h>
#include <list>
#include <iostream>
#include <errno.h>
#include <string.h>
#include <vector>
using namespace std;

//the following are UBUNTU/LINUX ONLY terminal color codes.
#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */
#define BOLDBLACK   "\033[1m\033[30m"      /* Bold Black */
#define BOLDRED     "\033[1m\033[31m"      /* Bold Red */
#define BOLDGREEN   "\033[1m\033[32m"      /* Bold Green */
#define BOLDYELLOW  "\033[1m\033[33m"      /* Bold Yellow */
#define BOLDBLUE    "\033[1m\033[34m"      /* Bold Blue */
#define BOLDMAGENTA "\033[1m\033[35m"      /* Bold Magenta */
#define BOLDCYAN    "\033[1m\033[36m"      /* Bold Cyan */
#define BOLDWHITE   "\033[1m\033[37m"      /* Bold White */

class client_recv_stru
{
public:
	client_recv_stru(uint32_t id = 0, string data = "") : client_data(id), recv_buf(data){ }
	~client_recv_stru() { }
	
	uint32_t client_data;
	string recv_buf;

	bool operator == (const client_recv_stru &rhs) const
	{
		return (client_data == rhs.client_data);
	}
};

template <typename T>
class bounded_blocking_queue
{
	public:
		bounded_blocking_queue(unsigned int size)
		{
			_max_size = (size < _list.max_size() ? size : _list.max_size());
			pthread_mutex_init(&_rlock, NULL);
			pthread_mutex_init(&_wlock, NULL);
			pthread_cond_init(&_rcond, NULL);
			pthread_cond_init(&_wcond, NULL);
		}
		~bounded_blocking_queue()
		{
			pthread_mutex_destroy(&_rlock);
			pthread_mutex_destroy(&_wlock);
			pthread_cond_destroy(&_rcond);
			pthread_cond_destroy(&_wcond);
		}
		//void push(const T& data);
		void push(const T& data, const int seconds);
		//bool pop(T& data);
		bool pop(T& data, bool & was_empty, const int seconds); // time-out pop
		void erase(const T& data);
	private:
		list<T> _list;
		unsigned int _max_size;
		pthread_mutex_t _rlock, _wlock;
		pthread_cond_t _rcond, _wcond;
};

#if 0
template <typename T>
void bounded_blocking_queue<T>::push(const T& data)
{
	pthread_mutex_lock(&_wlock);
	const bool was_empty = _list.empty();
	_list.push_back(data);
	pthread_mutex_unlock(&_wlock);
	if (was_empty) 
		pthread_cond_broadcast(&_cond);
}

template <typename T>
bool bounded_blocking_queue<T>::pop(T& data)
{
	pthread_mutex_lock(&_rlock);
	while (_list.empty())
	{
		pthread_cond_wait(&_cond, &_rlock);
	}
	data = _list.front();
	_list.pop_front();
	pthread_mutex_unlock(&_rlock);
	return true;
}
#endif

static string get_current_time()
{
	time_t timep;
	struct tm *p = NULL;
	char s[64] = {0};
	time(&timep);
	p = localtime(&timep);
	strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S\t", p);
	return string(s);
}

template <typename T>
void bounded_blocking_queue<T>::push(const T& data, const int seconds)
{
	int result = 0;
	struct timespec ts1, ts2;
	bool was_empty = _list.empty();
	clock_gettime(CLOCK_REALTIME, &ts1);
	pthread_mutex_lock(&_wlock);
	clock_gettime(CLOCK_REALTIME, &ts2);
	if ((ts2.tv_sec - ts1.tv_sec) < seconds)
	{
		ts2.tv_sec += seconds;
		was_empty = _list.empty();
		while (_list.size() == _max_size && result == 0) 
		{
			cout << BOLDYELLOW << get_current_time() << "push: before pthread_cond_timedwait" << RESET << endl;
			result = pthread_cond_timedwait(&_wcond, &_wlock, &ts2);
			cout << BOLDYELLOW << get_current_time() << "push: after pthread_cond_timedwait" << RESET << endl;
		}
		cout << BOLDYELLOW << get_current_time() << "before push: result is " << result << ", list.size is " << _list.size() << RESET << endl;
		if(result == 0)
			_list.push_back(data);
		cout << BOLDYELLOW << get_current_time() << "after push: result is " << result << ", list.size is " << _list.size() << RESET << endl;
	}
	else
	{
		cout << BOLDYELLOW << get_current_time() << "push: (ts2.tv_sec - ts1.tv_sec) < seconds" << RESET << endl;
	}
	pthread_mutex_unlock(&_wlock);
	if (was_empty)
	{
		cout << BOLDYELLOW << get_current_time() << "push: pthread_cond_broadcast(&_rcond)" << RESET << endl;
		pthread_cond_broadcast(&_rcond);
	}
	else
	{
		cout << BOLDYELLOW << get_current_time() << "push: No pthread_cond_broadcast(&_rcond)" << RESET << endl;
	}
}

template <typename T>
bool bounded_blocking_queue<T>::pop(T& data, bool & was_empty, const int seconds)
{
	bool ret = false;
	int result = 0;
	struct timespec ts;
	pthread_mutex_lock(&_rlock);
	const bool was_full = (_list.size() == _max_size);
	cout << BOLDBLUE << get_current_time() << "was_full: " << was_full << " list.size is "<< _list.size() << " max_size is "<< _max_size << RESET << endl;
	clock_gettime(CLOCK_REALTIME, &ts);
	ts.tv_sec += seconds; // specify wake up time
	while (_list.empty() && result == 0)
	{
		cout << BOLDBLUE << get_current_time() << "pop: before pthread_cond_timedwait" << RESET << endl;
		result = pthread_cond_timedwait(&_rcond, &_rlock, &ts);
		cout << BOLDBLUE << get_current_time() << "pop: after pthread_cond_timedwait" << RESET << endl;
	}
	cout << BOLDBLUE << get_current_time() << "before pop: result is " << result << ", list.size is " << _list.size() << RESET << endl;
	if(result == 0)
	{
		data = _list.front();
		_list.pop_front();
		ret = true;
	}
	cout << BOLDBLUE << get_current_time() << "after pop: result is " << result << ", list.size is " << _list.size() << RESET << endl;
	was_empty = _list.empty();
	pthread_mutex_unlock(&_rlock);
	if (was_full)
	{
		cout << BOLDBLUE << get_current_time() << "pop: pthread_cond_broadcast(_wcond)" << RESET << endl;
		pthread_cond_broadcast(&_wcond);
	}
	else
	{
		cout << BOLDBLUE << get_current_time() << "pop: No pthread_cond_broadcast(_wcond)" << RESET << endl;
	}

	return ret;
}

template <typename T>
void bounded_blocking_queue<T>::erase(const T& data)
{
	int count = 0;
	pthread_mutex_lock(&_rlock);
	cout << BOLDRED << get_current_time() << "erase: we will found id is " << data.client_data << RESET << endl;
	while(1)
	{
		auto iter = find(_list.begin(), _list.end(), data);
		if (iter != _list.end())
		{
			count++;
			cout << BOLDRED << get_current_time() << "erase: found " << iter->client_data << " " << iter->recv_buf<< count << RESET << endl;
			_list.erase(iter);
		}
		else
		{
			cout << BOLDRED << get_current_time() << "erase: No found " << count << RESET << endl;
			break;
		}
	}
	pthread_mutex_unlock(&_rlock);
}
#endif //_BOUNDED_BLOCKING_QUEUE_H_