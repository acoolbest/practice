/**
*	g++ list.cpp -lpthread
**/
#include <pthread.h>
#include <list>
#include <iostream>
#include <errno.h>
#include <string.h>
#include <vector>
using namespace std;
//-------------------------------
#if 0
template <typename T>
class Queue 
{ 
	public: 
		Queue( ) 
		{ 
			pthread_mutex_init(&_lock, NULL); 
		} 
		~Queue( ) 
		{ 
			pthread_mutex_destroy(&_lock);
		} 
		void push(const T& data);
		T pop( ); 
	private: 
		list<T> _list; 
		pthread_mutex_t _lock;
};

template <typename T>
void Queue<T>::push(const T& value ) 
{ 
	pthread_mutex_lock(&_lock);
	_list.push_back(value);
	pthread_mutex_unlock(&_lock);
}

template <typename T>
T Queue<T>::pop( ) 
{ 
	if (_list.empty( )) 
	{
		throw "element not found";
	}
	pthread_mutex_lock(&_lock); 
	T _temp = _list.front( );
	_list.pop_front( );
	pthread_mutex_unlock(&_lock);
	return _temp;
}
#endif
//-------------------------------
#if 0
template <typename T>
class Queue 
{ 
public: 
    Queue( ) 
    { 
        pthread_mutex_init(&_rlock, NULL); 
        pthread_mutex_init(&_wlock, NULL);
    } 
    ~Queue( ) 
    { 
        pthread_mutex_destroy(&_rlock);
        pthread_mutex_destroy(&_wlock);
    } 
    void push(const T& data);
    T pop( ); 
private: 
    list<T> _list; 
    pthread_mutex_t _rlock, _wlock;
};

template <typename T>
void Queue<T>::push(const T& value ) 
{ 
    pthread_mutex_lock(&_wlock);
    _list.push_back(value);
    pthread_mutex_unlock(&_wlock);
}
template <typename T>
T Queue<T>::pop( ) 
{ 
    if (_list.empty( )) 
    { 
        throw "element not found";
    }
    pthread_mutex_lock(&_rlock);
    T _temp = _list.front( );
    _list.pop_front( );
    pthread_mutex_unlock(&_rlock);
    return _temp;
}
#endif
//-------------------------------
#if 1
template <typename T>
class BlockingQueue 
{ 
public: 
    BlockingQueue ( ) 
    { 
        pthread_mutexattr_init(&_attr); 
        // set lock recursive
        pthread_mutexattr_settype(&_attr,PTHREAD_MUTEX_RECURSIVE_NP); 
        pthread_mutex_init(&_lock,&_attr);
        pthread_cond_init(&_cond, NULL);
    } 
    ~BlockingQueue ( ) 
    { 
        pthread_mutex_destroy(&_lock);
        pthread_cond_destroy(&_cond);
    } 
    void push(const T& data);
    void push(const T& data, const int seconds); //time-out push
    T pop( );
    bool pop(T & out_data, const int seconds); // time-out pop
private: 
    list<T> _list; 
    pthread_mutex_t _lock;
    pthread_mutexattr_t _attr;
    pthread_cond_t _cond;
};
template <typename T>
T BlockingQueue<T>::pop( ) 
{ 
    pthread_mutex_lock(&_lock);
    while (_list.empty( )) 
    { 
        pthread_cond_wait(&_cond, &_lock);
    }
    T _temp = _list.front( );
    _list.pop_front( );
    pthread_mutex_unlock(&_lock);
    return _temp;
}
template <typename T>
void BlockingQueue <T>::push(const T& data ) 
{ 
    pthread_mutex_lock(&_lock);
    const bool was_empty = _list.empty( );
    _list.push_back(data);
    pthread_mutex_unlock(&_lock);
    if (was_empty) 
        pthread_cond_broadcast(&_cond);
}

//-------------------------------
template <typename T>
void BlockingQueue <T>::push(const T& data, const int seconds) 
{
    struct timespec ts1, ts2;
    bool was_empty = _list.empty( );
    clock_gettime(CLOCK_REALTIME, &ts1);
    pthread_mutex_lock(&_lock);
    clock_gettime(CLOCK_REALTIME, &ts2);
    if ((ts2.tv_sec - ts1.tv_sec) < seconds) 
    {
        was_empty = _list.empty( );
        _list.push_back(data);
    }
    pthread_mutex_unlock(&_lock);
    if (was_empty) 
        pthread_cond_broadcast(&_cond);
}
template <typename T>
bool BlockingQueue <T>::pop(T & out_data, const int seconds)
{
	int result = 0;
    struct timespec ts1, ts2; 
    clock_gettime(CLOCK_REALTIME, &ts1); 
    pthread_mutex_lock(&_lock);
    clock_gettime(CLOCK_REALTIME, &ts2);
    // First Check: if time out when get the _lock 
    if ((ts1.tv_sec - ts2.tv_sec) < seconds) 
    { 
        ts2.tv_sec += seconds; // specify wake up time
        while(_list.empty( ) && (result == 0)) 
        { 
            result = pthread_cond_timedwait(&_cond, &_lock, &ts2) ;
        }
        if (result == 0) // Second Check: if time out when timedwait  
        {
            out_data = _list.front( );
            _list.pop_front( );
            pthread_mutex_unlock(&_lock);
            return true;
        }
		else
		{
			if(result == ETIMEDOUT)
				cout << "ETIMEDOUT" << endl;
			else
				cout << "OTHER: " << strerror(errno) << ", result: " << result << endl;
		}
    }
    pthread_mutex_unlock(&_lock);
	return false;
    //throw exception("timeout happened");
}
#endif
//-------------------------------
#if 0
template <typename T>
class BoundedBlockingQueue 
{ 
public: 
    BoundedBlockingQueue (unsigned int size) : maxSize(size) 
    { 
        pthread_mutex_init(&_lock, NULL); 
        pthread_cond_init(&_rcond, NULL);
        pthread_cond_init(&_wcond, NULL);
        _array.reserve(maxSize);
    } 
    ~BoundedBlockingQueue ( ) 
    { 
        pthread_mutex_destroy(&_lock);
        pthread_cond_destroy(&_rcond);
        pthread_cond_destroy(&_wcond);
    } 
    void push(const T& data);
    T pop( ); 
private: 
    vector<T> _array; // or T* _array if you so prefer
    unsigned int maxSize;
    pthread_mutex_t _lock;
    pthread_cond_t _rcond, _wcond;
};
template <typename T>
void BoundedBlockingQueue <T>::push(const T& value ) 
{ 
    pthread_mutex_lock(&_lock);
    const bool was_empty = _array.empty( );
    while (_array.size( ) == maxSize) 
    { 
        pthread_cond_wait(&_wcond, &_lock);
    } 
    _array.push_back(value);
    pthread_mutex_unlock(&_lock);
    if (was_empty) 
        pthread_cond_broadcast(&_rcond);
}
template <typename T>
T BoundedBlockingQueue<T>::pop( ) 
{ 
    pthread_mutex_lock(&_lock);
    const bool was_full = (_array.size( ) == maxSize);
    while(_array.empty( )) 
    { 
        pthread_cond_wait(&_rcond, &_lock) ;
    }
    T _temp = _array.front( );
    _array.erase( _array.begin( ));
    pthread_mutex_unlock(&_lock);
    if (was_full)
        pthread_cond_broadcast(&_wcond);
    return _temp;
}
#endif
//-------------------------------
//-------------------------------
//-------------------------------
//-------------------------------
struct client_recv_stru
{
	int client_data;
	string recv_buf;
};

int main(int argc, char *argv[])
{
	#if 0
	BoundedBlockingQueue<client_recv_stru> q(5);
	client_recv_stru stru_tmp;
	
	//stru_tmp = q.pop();
	//cout << stru_tmp.recv_buf << endl;
	
	stru_tmp.client_data = 1;
	stru_tmp.recv_buf = "123, ";
	q.push(stru_tmp);
	stru_tmp.client_data = 2;
	stru_tmp.recv_buf = "456, ";
	q.push(stru_tmp);
	
	while(1)
	{
		try
		{
			stru_tmp = q.pop();
			cout << stru_tmp.recv_buf << stru_tmp.client_data << endl;
		}
		catch (exception ex)
		{
			cout << "pop err: " << ex.what() << endl;
		}
	}
	
	#else
	BlockingQueue<client_recv_stru> q;
	client_recv_stru stru_tmp;

	stru_tmp.client_data = 1;
	stru_tmp.recv_buf = "123, ";
	q.push(stru_tmp, 1);
	stru_tmp.client_data = 2;
	stru_tmp.recv_buf = "456, ";
	q.push(stru_tmp, 1);
	while(1)
	{
		if(q.pop(stru_tmp, 1))
			cout << stru_tmp.recv_buf << stru_tmp.client_data << endl;
		else
			cout << "pop err ~~~~~~~~~" << endl;
	}
	
	#endif
	return 0;
}
