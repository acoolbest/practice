#ifndef _THREAD_POOL_H_
#define _THREAD_POOL_H_

#include <glib.h>
#include <glib/gerror.h>
#include <glib/gmessages.h>
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <pthread.h>
#include <unistd.h>

typedef void(GFUNC) (gpointer data, gpointer user_data);
typedef void* (START_ROUTINE)(void* arg);

uint32_t init_android_thread_pool(int max_threads, bool exclusive, void *u_date, GFUNC* func);
uint32_t init_ios_thread_pool(int max_threads, bool exclusive, void *u_date, GFUNC* func);

void destory_android_thread_pool();
void destory_ios_thread_pool();

uint32_t set_max_android_threads(uint32_t max_threads);
uint32_t set_max_ios_threads(uint32_t max_threads);

uint32_t push_new_android_to_thread_pool(void *data);
uint32_t push_new_ios_to_thread_pool(void *data);

uint32_t init_thread_count_lock();
uint32_t destory_thread_count_lock();
uint32_t operator_thread_work_count(uint32_t oper_type);
uint32_t get_thread_work_count();

#endif // _THREAD_POOL_H_
