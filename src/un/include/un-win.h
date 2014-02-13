#ifndef _NU_WIN_H_
#define _NU_WIN_H_
#include <windows.h>
#include <process.h>
#include "winapi.h"

typedef HANDLE un_thread_t;  //线程句柄
typedef CRITICAL_SECTION un_mutex_t;//互斥
typedef HANDLE  un_sem_t;			//信号量

typedef  union
{
	SRWLOCK rwlock;
	struct
	{
		un_mutex_t read_mutex_;
		un_mutex_t write_mutex_;
		unsigned int number;
	}fall_;

}un_rwlock_t;

typedef union
{
	CONDITION_VARIABLE cond;
	struct
	{
		CRITICAL_SECTION waiters_count_lock_;
		unsigned int   wait_count_;
		HANDLE  signal_event_;
		HANDLE  broadcas_event_;

	}fall_;


}un_cond_t;

typedef struct 
{
	un_mutex_t mutex;
	un_sem_t  sem1;
	un_sem_t sem2;
	unsigned int  wait_count_;
	unsigned int current_wait_;


}un_barrier_t;

typedef struct 
{
	HANDLE event_;
	unsigned char is_init;

}un_once_t;

#define  UN_ONCE_INIT {NULL,0}







#endif