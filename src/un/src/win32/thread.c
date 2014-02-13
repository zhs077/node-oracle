#include "un.h"
#include "internal.h"

#define  HAVA_RWLOCK_API()  (pInitializeSRWLock != NULL)
#define  HAVA_COND_API()	(pInitializeConditionVariable == NULL)


static un_once_t once_t=UN_ONCE_INIT;
static void un_init(void)//初始化环境
{
	/* Tell Windows that we will handle critical errors. */
	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX |
		SEM_NOOPENFILEERRORBOX);
	init_winapi();

}


void uv__once_init(void) 
{
	un_once(&once_t, un_init);
}


void un_once(un_once_t* once,void(*callback)(void))
{
	if(once->is_init)
		return;
	else
	{
		int result;
		HANDLE exit_event = NULL;
		HANDLE create_event = NULL;
		create_event = CreateEvent(NULL,TRUE,FALSE,NULL);
		if(create_event == NULL)
		{
			un_get_error(GetLastError(), "CreateEvent");
		}
		//InterlockedCompareExchange是把目标操作数（第1参数所指向的内存中的数）与一个值（第3参数）比较，
		//如果相等，则用另一个值（第2参数）与目标操作数（第1参数所指向的内存中的数）交换；InterlockedExchange是不比较直接交换。
		//整个操作过程是锁定内存的，其它处理器不会同时访问内存，从而实现多处理器环境下的线程互斥。
		//返回第一个参数的值
		exit_event =InterlockedCompareExchangePointer(&once->event_,&create_event,NULL);
		if(exit_event == NULL) //赢得该race
		{
			callback();
			result = SetEvent(create_event);
			assert(result);
			once->is_init = 1;

		}

		else
		{
			CloseHandle(create_event);
			result = WaitForSingleObject(exit_event,INFINITE);
			assert(result == WAIT_OBJECT_0);

			
		}

	}


}
static UINT _stdcall un_thread_start(void* arg)
{
	thread_ctx* tmp = (thread_ctx*)arg;
	thread_ctx ctx;
	ctx = *tmp;
	free(tmp);
	ctx.entery(ctx.arg);
	
	return 0;
}
int un_thread_create(un_thread_t* tid,void(*entery)(void* arg),void* arg)
{
	thread_ctx* ctx = (thread_ctx*)malloc(sizeof(thread_ctx));
	int r;
	if (ctx == NULL)
	{
		return UN_ENOMEM;
	}
	ctx->entery = entery;
	ctx->arg = arg;
	*tid= (HANDLE) _beginthreadex(NULL,0,un_thread_start,ctx,0,NULL);
	r = *tid ? 0:errno;
	if (r)
	{
		free(ctx);
		
	}
	return r;
}

int un_thread_join(un_thread_t* tid)
{
	if(WaitForSingleObject(*tid,INFINITE))
		return GetLastError();
	else
	{
		CloseHandle(*tid);
		return 0;

	}
}

int un_mutex_init(un_mutex_t* mutex)
{
	InitializeCriticalSection(mutex);
	return 0;
}
void un_mutex_destroy(un_mutex_t* mutex)
{
	DeleteCriticalSection(mutex);
}
void un_mutex_lock(un_mutex_t* mutex)
{
	EnterCriticalSection(mutex);
}

void un_mutex_unlock(un_mutex_t* mutex)
{
	LeaveCriticalSection(mutex);
}

int un_mutex_trylock(un_mutex_t* mutex)
{
	if(TryEnterCriticalSection(mutex))
		return 0;
	else
		return UN_EAGAIN;

}
int un_rwlock_init(un_rwlock_t* rwlock)
{
	uv__once_init();
	if(HAVA_RWLOCK_API())
	{

		pInitializeSRWLock(&rwlock->rwlock);

	}
	else
	{

		un_mutex_init(&rwlock->fall_.read_mutex_);
		un_mutex_init(&rwlock->fall_.write_mutex_);
		rwlock->fall_.number = 0;

	}
	return 0;


}
void un_rwlock_destroy(un_rwlock_t* rwlock)
{
	if(HAVA_RWLOCK_API())
	{

		(void)rwlock->rwlock ;
	}
	else
	{
		un_mutex_destroy(&rwlock->fall_.read_mutex_);
		un_mutex_destroy(&rwlock->fall_.write_mutex_);
		rwlock->fall_.number = 0;

	}
}
void un_rwlock_rdlock(un_rwlock_t* rwlock)
{

	if(HAVA_RWLOCK_API())
	{

		pAcquireSRWLockShared(&rwlock->rwlock);
	}
	else
	{
		un_mutex_lock(&rwlock->fall_.read_mutex_);
		if(++rwlock->fall_.number == 1)
			un_mutex_lock(&rwlock->fall_.write_mutex_);

		un_mutex_unlock(&rwlock->fall_.read_mutex_);

	}
}
int un_rwlock_tryrdlock(un_rwlock_t* rwlock)
{
	if (HAVA_RWLOCK_API())
	{
		if( pTryAcquireSRWLockShared(&rwlock->rwlock))
			return 0;
		else
		return UV_EBUSY;
	}
	else
	{
		int r;
		r = un_mutex_trylock(&rwlock->fall_.read_mutex_);
		if (r)
		{
			goto error2;

		}
		if (rwlock->fall_.number == 0)
		{
			r = un_mutex_trylock(&rwlock->fall_.write_mutex_);

		}
		if (r == 0)
		{
			rwlock->fall_.number++;
		}


		un_mutex_unlock(&rwlock->fall_.read_mutex_);
error2:
		return r;
	}


}
void un_rwlock_rdunlock(un_rwlock_t* rwlock)
{

	if(HAVA_RWLOCK_API())
	{

		pReleaseSRWLockShared(&rwlock->rwlock);
	}
	else
	{
		un_mutex_lock(&rwlock->fall_.read_mutex_);
		if(--rwlock->fall_.number == 0)
			un_mutex_unlock(&rwlock->fall_.write_mutex_);

		un_mutex_unlock(&rwlock->fall_.read_mutex_);

	}
}

void un_rwlock_wrlock(un_rwlock_t* rwlock)
{

	if(HAVA_RWLOCK_API())
	{

		pAcquireSRWLockExclusive(&rwlock->rwlock);
	}
	else
	{
		un_mutex_lock(&rwlock->fall_.write_mutex_);

	}
}
int un_rwlock_trywrlock(un_rwlock_t* rwlock)
{
	if(HAVA_RWLOCK_API())
	{
		if(pTryAcquireSRWLockExclusive(&rwlock->rwlock))
			return 0;
		else
			return UV_EBUSY;
	}
	else
	{
		return un_mutex_trylock(&rwlock->fall_.write_mutex_);


	}
	

}
void un_rwlock_wrunlock(un_rwlock_t* rwlock)
{

	if(HAVA_RWLOCK_API())
	{

		pReleaseSRWLockExclusive(&rwlock->rwlock);
	}
	else
	{
		un_mutex_unlock(&rwlock->fall_.write_mutex_);

	}
}



int un_sem_init(un_sem_t* sem, unsigned int count)
{
	*sem = CreateSemaphore(NULL,count,INT_MAX,NULL);
	if (sem == NULL)
	{
		return GetLastError();
	}
	return 0;
}

void un_sem_destroy(un_sem_t* sem)
{
	if (!CloseHandle(*sem))
		abort();
	
}
void un_sem_post(un_sem_t* sem)
{
	if(!ReleaseSemaphore(*sem,1,NULL))
		abort();
}
void un_sem_wait(un_sem_t* sem)
{
	if(WaitForSingleObject(*sem,INFINITE)  != WAIT_OBJECT_0)
		abort();
}

int  un_sem_trywait(un_sem_t* sem)
{
	DWORD r;
	r = WaitForSingleObject(*sem,0);
	if (WAIT_OBJECT_0 == r)
		return 0;
	if(WAIT_TIMEOUT == r)
		return UN_EAGAIN;
	abort();
	return -1;
	
}

//参考 http://www.cs.wustl.edu/~schmidt/win32-cv-1.html

int un_cond_init(un_cond_t* cond)
{
	uv__once_init();
	if (HAVA_COND_API())
	{
		pInitializeConditionVariable(&cond->cond);
		return 0;
	
	}
	else
	{
		int err_no;
		cond->fall_.wait_count_ = 0;
		InitializeCriticalSection(&cond->fall_.waiters_count_lock_);
		//创建自动重置的事件
		cond->fall_.signal_event_ = CreateEvent(NULL,FALSE,FALSE,NULL);
		if (cond->fall_.signal_event_ == NULL)
		{
			  err_no = GetLastError();
			goto err2;
		}
		//创建人工重置的事件

		cond->fall_.broadcas_event_ = CreateEvent(NULL,TRUE,FALSE,NULL);
		if (cond->fall_.broadcas_event_ == NULL)
		{
			 err_no = GetLastError();
			goto err;
		}
		return 0;
err:	
		CloseHandle(cond->fall_.signal_event_);

err2:
		DeleteCriticalSection(&cond->fall_.waiters_count_lock_);

		 return  err_no;

	}
		
}

void un_cond_destroy(un_cond_t* cond)
{
	if (HAVA_COND_API())
	{
		//nothing
	}
	else
	{
		if(!CloseHandle(cond->fall_.signal_event_))
			abort();
		if(!CloseHandle(cond->fall_.broadcas_event_))
			abort();
		DeleteCriticalSection(&cond->fall_.waiters_count_lock_);
	}
}

int un_cond_wait_helper(un_cond_t* cond, un_mutex_t* mutex,DWORD dwMilliseconds)
{
	HANDLE handles[2] = 
	{
		cond->fall_.signal_event_,
		cond->fall_.broadcas_event_
	};

	int result ;
	int last_waiter;
	EnterCriticalSection (&cond->fall_.waiters_count_lock_);
	cond->fall_.wait_count_++;
	LeaveCriticalSection (&cond->fall_.waiters_count_lock_);
	// It's ok to release the <external_mutex> here since Win32
	// manual-reset events maintain state when used with
	// <SetEvent>.  This avoids the "lost wakeup" bug...
	un_mutex_unlock(mutex);
	/* Wait for either event to become signaled due to <uv_cond_signal> being */
	/* called or <uv_cond_broadcast> being called. */
	result = WaitForMultipleObjects(2, handles, FALSE, dwMilliseconds);
	EnterCriticalSection (&cond->fall_.waiters_count_lock_);
	cond->fall_.wait_count_--;
	last_waiter = (result == WAIT_OBJECT_0 + 1) && (cond->fall_.wait_count_ == 0);
	LeaveCriticalSection (&cond->fall_.waiters_count_lock_);
	// Some thread called <pthread_cond_broadcast>.
	if (last_waiter)
	{
		// We're the last waiter to be notified or to stop waiting, so
		// reset the manual event. 
		ResetEvent (cond->fall_.broadcas_event_); 

	}

	un_mutex_lock(mutex);
	if (result == WAIT_OBJECT_0 || result == WAIT_OBJECT_0 + 1)
		return 0;


	if (result == WAIT_TIMEOUT)
		return UN_ETIMEDOUT ;

	abort();
	return -1 ; /* Satisfy the compiler. */

}
void un_cond_wait(un_cond_t* cond,un_mutex_t* mutex)
{
	if (HAVA_COND_API())
	{
		if(!pSleepConditionVariableCS(&cond->cond,mutex,INFINITE))
		abort();
	}
	else
	{
		// Avoid race conditions.
		un_cond_wait_helper(cond,mutex,INFINITE);
	
	}

}
int un_cond_timedwait(un_cond_t* cond,un_mutex_t* mutex, uint64_t timeout)
{
	if(HAVA_COND_API())
	{
		if(pSleepConditionVariableCS(&cond->cond,mutex,(DWORD)timeout/1e6))
			return 0;
		if (GetLastError() != ERROR_TIMEOUT)
			abort();
		return UN_ETIMEDOUT;
	}
	else
	{
		return un_cond_wait_helper(cond,mutex,(DWORD)timeout/1e6);

	}

}

void un_cond_signal(un_cond_t* cond)
{
	if(HAVA_COND_API())
	{
		pWakeConditionVariable(&cond->cond);
	}
	else
	{
		int have_waits;
		EnterCriticalSection(&cond->fall_.waiters_count_lock_);
		have_waits = cond->fall_.wait_count_ > 0;
		LeaveCriticalSection(&cond->fall_.waiters_count_lock_);
		if (have_waits)
			SetEvent(cond->fall_.signal_event_);
		
	}
	
}

void un_cond_broadcast (un_cond_t* cond)
{
	if(HAVA_COND_API())
	{
		pWakeAllConditionVariable(&cond->cond);
	}
	else
	{
		int have_waits;
		EnterCriticalSection(&cond->fall_.waiters_count_lock_);
		have_waits = cond->fall_.wait_count_ > 0;
		LeaveCriticalSection(&cond->fall_.waiters_count_lock_);
		if (have_waits)
			SetEvent(cond->fall_.broadcas_event_);
	}
	
}

int un_barrier_init(un_barrier_t* barrier,unsigned int wait_count_)
{
	int err;
	barrier->wait_count_ = wait_count_;
	barrier->current_wait_ = 0;
	err = un_mutex_init(&barrier->mutex);
	if(err)
		return err;
	err = un_sem_init(&barrier->sem1,0);
	if(err)
		goto error2;
	err = un_sem_init(&barrier->sem2,1);
	if(err)
		goto error;
	return 0;
	
error:
	 un_sem_destroy(&barrier->sem1);
error2:
	 un_mutex_destroy(&barrier->mutex);


	 return err;

	
}
void un_barrier_destroy(un_barrier_t* barrier)
{
	un_sem_destroy(&barrier->sem2);
	un_sem_destroy(&barrier->sem1);
	un_mutex_destroy((&barrier->mutex));
}
void un_barrier_wait(un_barrier_t* barrier)
{
	un_mutex_lock(&barrier->mutex);
	if(++barrier->current_wait_ == barrier->wait_count_)
	{
		un_sem_wait(&barrier->sem2);
		un_sem_post(&barrier->sem1);
	}
	un_mutex_unlock(&barrier->mutex);

	un_sem_wait(&barrier->sem1);
	un_sem_post(&barrier->sem1);

	un_mutex_lock(&barrier->mutex);
	if(--barrier->current_wait_ == 0)
	{
		un_sem_wait(&barrier->sem1);
		un_sem_post(&barrier->sem2);
	}
	un_mutex_unlock(&barrier->mutex);
	un_sem_wait(&barrier->sem2);
	un_sem_post(&barrier->sem2);

}

unsigned long  un_thread_self()
{
	return (unsigned long)GetCurrentThreadId();
}

void un_sleep(uint64_t millseconds)
{
	Sleep(millseconds);
}