/************************************************************************/
/* 一些共有的数据结构                                                        */
/************************************************************************/


#ifndef _UN_COMMON_H_
#define _UN_COMMON_H_
typedef struct 
{
	void(*entery)(void* arg);
	void* arg;

}thread_ctx;
#endif
