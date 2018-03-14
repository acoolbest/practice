#include <iostream>
#include "public_func/src/def.h"
#include "public_func/src/public_function.h"
#include "thread_pool.h"

GThreadPool * g_android_thread_pool = NULL;
GThreadPool * g_ios_thread_pool = NULL;

static pthread_mutex_t							g_thread_count_mutex;
static uint32_t									g_thread_count = 0;

static bool										g_thread_count_lock_inited = false;

uint32_t init_thread_count_lock()
{
	uint32_t ret = CRG_FAIL;
	g_thread_count_lock_inited = false;

	ret = pthread_mutex_init(&g_thread_count_mutex, NULL) == 0 ? CRG_SUCCESS : CRG_FAIL;
	if (ret == CRG_SUCCESS)
		g_thread_count_lock_inited = true;
	else
		write_log("init mutex for thread count%s%s", PRINT_POINT_STR, PRINT_FAIL_STR);

	return ret;
}

uint32_t destory_thread_count_lock()
{
	if (g_thread_count_lock_inited)
	{
		g_thread_count_lock_inited = false;
		return pthread_mutex_destroy(&g_thread_count_mutex) == 0 ? CRG_SUCCESS : CRG_FAIL;
	}
	return CRG_SUCCESS;
}

static void enter_thread_count_lock()
{
	pthread_mutex_lock(&g_thread_count_mutex);
}

static void leave_thread_count_lock()
{
	pthread_mutex_unlock(&g_thread_count_mutex);
}

uint32_t get_thread_work_count()
{
	uint32_t ret = 0;
	enter_thread_count_lock();
	ret = g_thread_count;
	leave_thread_count_lock();

	return ret;
}

uint32_t operator_thread_work_count(uint32_t oper_type)
{
	uint32_t ret = 0;
	enter_thread_count_lock();
	switch (oper_type)
	{
	case INCREASE_TYPE:
		g_thread_count++;
		break;
	case DECREASE_TYPE:
		if (g_thread_count > 0)
		{
			g_thread_count--;
		}
		break;
	default:
		break;
	}
	ret = g_thread_count;
	leave_thread_count_lock();

	return ret;
}

uint32_t init_android_thread_pool(int max_threads, bool exclusive, void *u_date, GFUNC* func)
{
	GError* error = NULL;
//	gpointer gp = NULL;

	if (!g_thread_supported())
		g_thread_init(NULL);

	g_android_thread_pool = g_thread_pool_new(func, u_date, max_threads, exclusive, &error);

	if (g_android_thread_pool != NULL)
	{
		// int gthreadcount = g_thread_pool_get_num_threads(g_android_thread_pool);
		// printf("%d\n", gthreadcount);
	}
	else
		write_log("%s init thread pool for android%s%s", PRINT_ANDROID_STR, PRINT_POINT_STR, PRINT_FAIL_STR);

	return (error == NULL && g_android_thread_pool != NULL) ? CRG_SUCCESS : CRG_FAIL;
}

uint32_t init_ios_thread_pool(int max_threads, bool exclusive, void *u_date, GFUNC* func)
{
	GError* error = NULL;
	//	gpointer gp = NULL;

	if (!g_thread_supported())
		g_thread_init(NULL);

	g_ios_thread_pool = g_thread_pool_new(func, u_date, max_threads, exclusive, &error);

	if (g_ios_thread_pool != NULL)
	{
		int gthreadcount = g_thread_pool_get_num_threads(g_ios_thread_pool);
	}
	else
		write_log("%s init thread pool for ios%s%s", PRINT_IOS_STR, PRINT_POINT_STR, PRINT_FAIL_STR);

	return (error == NULL && g_ios_thread_pool != NULL) ? CRG_SUCCESS : CRG_FAIL;
}

void destory_android_thread_pool()
{
	if (g_android_thread_pool != NULL)
	{
		g_thread_pool_free(g_android_thread_pool, false, true);
		g_android_thread_pool = NULL;
	}
}

void destory_ios_thread_pool()
{
	if (g_ios_thread_pool != NULL)
	{
		g_thread_pool_free(g_ios_thread_pool, false, true);
		g_ios_thread_pool = NULL;
	}
}

uint32_t push_new_android_to_thread_pool(void *data)
{
	GError* error = NULL;
	g_thread_pool_push(g_android_thread_pool, data, &error);
	return error == NULL ? CRG_SUCCESS : CRG_FAIL;
}

uint32_t push_new_ios_to_thread_pool(void *data)
{
	GError* error = NULL;
	g_thread_pool_push(g_ios_thread_pool, data, &error);
	return error == NULL ? CRG_SUCCESS : CRG_FAIL;
}

uint32_t set_max_android_threads(uint32_t max_threads)
{
	GError* error = NULL;
	g_thread_pool_set_max_threads(g_android_thread_pool, max_threads, &error);

	return error == NULL ? CRG_SUCCESS : CRG_FAIL;
}

uint32_t set_max_ios_threads(uint32_t max_threads)
{
	GError* error = NULL;
	g_thread_pool_set_max_threads(g_ios_thread_pool, max_threads, &error);

	return error == NULL ? CRG_SUCCESS : CRG_FAIL;
}

uint32_t create_thread(pthread_t* out_p_thread_pid, START_ROUTINE* func, int & out_err)
{
	if (out_p_thread_pid == NULL || func == NULL)
		return CRG_INVALID_PARAM;

	out_err = pthread_create(out_p_thread_pid, NULL, func, NULL);
	return out_err == 0 ? CRG_SUCCESS : CRG_FAIL;			
}

uint32_t wait_thread_exit(pthread_t thread_pid, int & out_err)
{
	out_err = pthread_join(thread_pid, NULL);
	return out_err == 0 ? CRG_SUCCESS : CRG_FAIL;
}
