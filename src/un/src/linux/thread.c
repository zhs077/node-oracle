#include "un.h"
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <unistd.h>
/*typedef pthread_t un_thread_t;
typedef pthread_mutex_t un_mutex_t;
typedef pthread_rwlock_t un_rwlock_t;
typedef pthread_once_t	un_once_t;
typedef pthread_cond_t  un_cond_t;
typedef pthread_barrier_t un_barrier_t;
typedef sem_t			un_sem_t;
*/

//typedef struct
//{
//	void(*entry)(void*arg);
//	void* arg;

//}thread_cxx;

#undef NANOSEC
#define NANOSEC ((uint64_t) 1e9)
void* un_thread_start(void* arg)
{

    thread_ctx* cxx = (thread_ctx*)arg;
    thread_ctx cxx_p = *cxx;
	free(cxx);
    cxx_p.entery(cxx_p.arg);

	return 0;
}
int un_thread_create(un_thread_t* tid,void(*entry)(void*arg),void* arg)
{
    thread_ctx* cxx = (thread_ctx*)malloc(sizeof(thread_ctx));
	if (cxx == NULL)
	{
		return UN_ENOMEM;
	}
    cxx->arg = arg;
    cxx->entery = entry;

	int err = pthread_create(tid,NULL,un_thread_start,cxx);
	if (err)
	{
		free(cxx);
	}
	 return err ? -1 : 0;

}


int un_thread_join(un_thread_t* tid )
{

	return -pthread_join(*tid,NULL);
	
}

int un_mutex_init(un_mutex_t* mutex)
{
#if defined (NDEBUG) || !defined(PTHREAD_MUTEX_ERRORCHECK)
	return -pthread_mutex_init(mutex,NULL);
#else
	pthread_mutexattr_t mattr;
	
	if(pthread_mutexattr_init(&mattr))
		abort();
	if(pthread_mutexattr_settype(&mattr,PTHREAD_MUTEX_ERRORCHECK))
		abort();
	int err;
	err = pthread_mutex_init(mutex, &mattr);
	if(pthread_mutexattr_destroy(&mattr))
		abort();
    return -err;
		
#endif

	
}

void un_mutex_destroy(un_mutex_t* mutex)
{
	if (pthread_mutex_destroy(mutex))
		abort();

}
void un_mutex_lock(un_mutex_t* mutex)
{
	if (pthread_mutex_lock(mutex))
		abort();
	
}
int un_mutex_trylock(un_mutex_t* mutex)
{
	int err = pthread_mutex_trylock(mutex);
	if (err && err != EBUSY && err != EAGAIN)
		abort();
	return -err;

}
void un_mutex_unlock(un_mutex_t* mutex)
{
	if (pthread_mutex_unlock(mutex))
		abort();

}
int un_rwlock_init(un_rwlock_t* rwlock)
{
	return -pthread_rwlock_init(rwlock,NULL);
}
void un_rwlock_destroy(un_rwlock_t* rwlock) 
{
	if (pthread_rwlock_destroy(rwlock))
		abort();
}

void un_rwlock_rdlock(un_rwlock_t* rwlock) 
{
	if (pthread_rwlock_rdlock(rwlock))
		abort();
}
void un_rwlock_rdunlock(un_rwlock_t* rwlock) 
{
	if (pthread_rwlock_unlock(rwlock))
		abort();
}
int un_rwlock_tryrdlock(un_rwlock_t* rwlock)
{
	int err = pthread_rwlock_tryrdlock(rwlock);
	if (err && err != EBUSY && err != EAGAIN)
		abort();
	return -err;

}

void un_rwlock_wrlock(un_rwlock_t*rwlock)
{
	if(pthread_rwlock_wrlock(rwlock))
		abort();
}
int un_rwlock_trywrlock(un_rwlock_t* rwlock)
{
	int err = pthread_rwlock_trywrlock(rwlock);
	if (err && err != EBUSY && err != EAGAIN)
		abort();
	return -err;

}
void un_rwlock_wrunlock(un_rwlock_t*rwlock)
{
	if(pthread_rwlock_unlock(rwlock))
		abort();
}
void un_once(un_once_t* guard,void(*callback)(void))
{
	if(pthread_once(guard,callback))
		abort();

}

int un_cond_init(un_cond_t* cond)
{
	pthread_condattr_t attr;
	int err = pthread_condattr_init(&attr);
	if (err)
		return -err;
	err = pthread_condattr_setclock(&attr, CLOCK_MONOTONIC);
	if (err)
		goto error2;
	err = pthread_cond_init(cond, &attr);
	if (err)
		goto error2;
	err = pthread_condattr_destroy(&attr);
	if (err)
		goto error;
    return 0;
error:
  pthread_cond_destroy(cond);
error2:
	pthread_condattr_destroy(&attr);
	return -err;

}

void un_cond_destroy(un_cond_t* cond)
{
	if(pthread_cond_destroy(cond))
		abort();
}
void un_cond_signal(un_cond_t* cond)
{
	if (pthread_cond_signal(cond))
		abort();
}
void un_cond_broadcast(un_cond_t* cond)
{
	if (pthread_cond_broadcast(cond))
		abort();
}
void un_cond_wait(un_cond_t* cond,un_mutex_t* mutext)
{
	if(pthread_cond_wait(cond,mutext))
		abort();
}
int un_cond_timedwait(un_cond_t* cond,un_mutex_t* mutex, uint64_t timeout)
{

	struct timespec ts;
	clock_gettime(CLOCK_MONOTONIC,&ts);
	timeout+=(ts.tv_sec * NANOSEC + ts.tv_nsec);
	ts.tv_sec = timeout / NANOSEC;
	ts.tv_nsec = timeout % NANOSEC;
	
	int err = pthread_cond_timedwait(cond,mutex,&ts);
	if (err == 0) 
		return 0;

	if (err == ETIMEDOUT)
		return -ETIMEDOUT;

	abort();
	return -1;  /* Satisfy the compiler. */
}
int un_sem_init(un_sem_t* sem,unsigned int value)
{
	if (sem_init(sem,0,value))
		return -errno;
	return 0;
	

}
void un_sem_destroy(un_sem_t* sem)
{
	if (sem_destroy(sem))
		abort();
	
}
void un_sem_post(un_sem_t* sem)
{
	if(sem_post(sem))
		abort();
}
void un_sem_wait(un_sem_t* sem)
{
	int r;
	do 
	{
        r = sem_wait(sem);
	} while (r == -1 && errno == EINTR);
	if (r)
	{
		abort();
	}

}

int un_sem_trywait(un_sem_t* sem)
{
	int r;
	do 
	{
        r = sem_trywait(sem);
	} while (r == -1 && errno == EINTR);
	if (r)
	{
		if (r == EAGAIN)
			return -EAGAIN;
		abort();
		
	}
	return 0;

}

int un_barrier_init(un_barrier_t* barrier,unsigned int count)
{
	return -pthread_barrier_init(barrier,NULL,count);


}
void un_barrier_destroy(un_barrier_t* barrier)
{
	if(pthread_barrier_destroy(barrier))
		abort();

}
void un_barrier_wait(un_barrier_t* barrier)
{
	int r = pthread_barrier_wait(barrier);
	if (r && r !=PTHREAD_BARRIER_SERIAL_THREAD)
        abort();
	
}
unsigned long un_thread_self()
{
    return pthread_self();
}

void un_sleep(uint64_t millseconds)
{
    usleep(millseconds * 1000); //linux sleep(s )
}
