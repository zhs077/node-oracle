#include "winapi.h"
#include"un.h"
#include "internal.h"


sInitializeSRWLock pInitializeSRWLock;
sAcquireSRWLockExclusive pAcquireSRWLockExclusive;
sAcquireSRWLockShared	pAcquireSRWLockShared;
sReleaseSRWLockShared	pReleaseSRWLockShared;
sReleaseSRWLockExclusive pReleaseSRWLockExclusive;
sSleepConditionVariableSRW pSleepConditionVariableSRW;
sTryAcquireSRWLockExclusive pTryAcquireSRWLockExclusive;
sTryAcquireSRWLockShared	pTryAcquireSRWLockShared;
//  [9/6/2013 zhs] 条换变量
sInitializeConditionVariable pInitializeConditionVariable;
sSleepConditionVariableCS pSleepConditionVariableCS;
sWakeConditionVariable pWakeConditionVariable;
sWakeAllConditionVariable pWakeAllConditionVariable;
//
sGetQueuedCompletionStatusEx pGetQueuedCompletionStatusEx;

void init_winapi()
{
	HMODULE kernel32_module;
	kernel32_module = GetModuleHandleA("Kernel32.dll");
	if (kernel32_module == NULL)
	{
		 un_get_error(GetLastError(), "GetModuleHandleA");
	}
	//读写锁
	pInitializeSRWLock = (sInitializeSRWLock)GetProcAddress(kernel32_module,"InitializeSRWLock");
	pAcquireSRWLockExclusive = (sAcquireSRWLockExclusive)GetProcAddress(kernel32_module,"AcquireSRWLockExclusive");
	pAcquireSRWLockShared = (sAcquireSRWLockShared)GetProcAddress(kernel32_module,"AcquireSRWLockShared");
	pReleaseSRWLockShared = (sReleaseSRWLockShared)GetProcAddress(kernel32_module,"ReleaseSRWLockShared");
	pReleaseSRWLockExclusive = (sReleaseSRWLockExclusive)GetProcAddress(kernel32_module,"ReleaseSRWLockExclusive");
	pSleepConditionVariableSRW = (sSleepConditionVariableSRW)GetProcAddress(kernel32_module,"SleepConditionVariableSRW");
	pTryAcquireSRWLockExclusive = (sTryAcquireSRWLockExclusive)GetProcAddress(kernel32_module,"TryAcquireSRWLockExclusive");
	pTryAcquireSRWLockShared = (sTryAcquireSRWLockShared)GetProcAddress(kernel32_module,"TryAcquireSRWLockShared");

	//条件变量
	pInitializeConditionVariable = (sInitializeConditionVariable)GetProcAddress(kernel32_module,"InitializeConditionVariable");
	pSleepConditionVariableCS = (sSleepConditionVariableCS)GetProcAddress(kernel32_module,"SleepConditionVariableCS");
	pWakeConditionVariable = (sWakeConditionVariable)GetProcAddress(kernel32_module,"WakeConditionVariable");
	pWakeAllConditionVariable = (sWakeAllConditionVariable)GetProcAddress(kernel32_module,"WakeAllConditionVariable");
	
	pGetQueuedCompletionStatusEx = (sGetQueuedCompletionStatusEx)GetProcAddress(kernel32_module,"GetQueuedCompletionStatusEx");


}

