/************************************************************************/
/* 该头文件用来获取一些跟操作系统相关的API，有的系统不支持            */
/************************************************************************/
//  [9/6/2013 zhs]


#ifndef _WINAPI_H_
#define _WINAPI_H_

#include "un-win.h"
//读写锁
/************************************************************************/
/* Minimum supported client
Windows Vista [desktop apps | Windows Store apps]
Minimum supported server
Windows Server 2008 [desktop apps | Windows Store apps]                                                                     */
/************************************************************************/
typedef VOID (WINAPI *sInitializeSRWLock)(PSRWLOCK SRWLock);
typedef VOID (WINAPI *sAcquireSRWLockExclusive)(PSRWLOCK SRWLock);
typedef VOID (WINAPI *sAcquireSRWLockShared)(PSRWLOCK SRWLock);
typedef VOID (WINAPI *sReleaseSRWLockExclusive)(PSRWLOCK SRWLock);
typedef VOID (WINAPI *sReleaseSRWLockShared)(PSRWLOCK SRWLock);
typedef BOOL (WINAPI *sSleepConditionVariableSRW)(PSRWLOCK SRWLock);

//win7 server2008 R2
typedef BOOLEAN  ( *sTryAcquireSRWLockExclusive)(PSRWLOCK SRWLock);
typedef BOOLEAN  ( *sTryAcquireSRWLockShared)(PSRWLOCK SRWLock);

extern sInitializeSRWLock pInitializeSRWLock;
extern sAcquireSRWLockExclusive pAcquireSRWLockExclusive;
extern sAcquireSRWLockShared	pAcquireSRWLockShared;
extern sReleaseSRWLockShared	pReleaseSRWLockShared;
extern sReleaseSRWLockExclusive pReleaseSRWLockExclusive;
extern sSleepConditionVariableSRW pSleepConditionVariableSRW;
extern sTryAcquireSRWLockExclusive pTryAcquireSRWLockExclusive;
extern sTryAcquireSRWLockShared	pTryAcquireSRWLockShared;

//条件变量
typedef VOID (WINAPI* sInitializeConditionVariable)(PCONDITION_VARIABLE ConditionVariable);
typedef BOOL (WINAPI* sSleepConditionVariableCS)(PCONDITION_VARIABLE ConditionVariable,PCRITICAL_SECTION CriticalSection,DWORD dwMilliseconds);
typedef VOID (WINAPI* sWakeConditionVariable)(PCONDITION_VARIABLE ConditionVariable);
typedef VOID (WINAPI* sWakeAllConditionVariable)(PCONDITION_VARIABLE ConditionVariable);
extern sInitializeConditionVariable pInitializeConditionVariable;
extern sSleepConditionVariableCS pSleepConditionVariableCS;
extern sWakeConditionVariable pWakeConditionVariable;
extern sWakeAllConditionVariable pWakeAllConditionVariable;

//
typedef BOOL (WINAPI*sGetQueuedCompletionStatusEx)
	(
	HANDLE CompletionPort,
	LPOVERLAPPED_ENTRY lpCompletionPortEntries,
	 ULONG ulCount,
	 PULONG ulNumEntriesRemoved,
	DWORD dwMilliseconds,
	 BOOL fAlertable
	);
extern sGetQueuedCompletionStatusEx pGetQueuedCompletionStatusEx;






#endif