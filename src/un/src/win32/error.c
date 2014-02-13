
#include "un.h"
#include <stdio.h>
void un_get_error(const int error_no, const char* err_msg)
{
	char* buf = NULL;
	const char* pTemp;
	FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,error_no,MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),(LPSTR)&buf,0,NULL);
	if (buf)
	{
		pTemp = buf;
	}
	else
	{
		pTemp = "Unknown error";
	}

	if (err_msg) 
	{
		fprintf(stderr, "%s: (%d) %s", err_msg, error_no, pTemp);
	} 
	else 
	{
		fprintf(stderr, "(%d) %s", error_no, pTemp);
	}
	if (buf) 
	{
		LocalFree(buf);
	}
	*((char*)NULL) = 0xff; /* Force debug break */
	abort();
	

}