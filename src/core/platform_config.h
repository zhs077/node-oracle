
#ifndef _PLATFORM_CINFIG_H_
#define  _PLATFORM_CINFIG_H_

//如果是nodejs环境请加上
#define NODEJS

#include <vector>
#include <map>
#include<cassert>
#include<string>
#include<cstring>
#include<cstdio>
#include<cstdlib>
#include<iostream>
#include<sstream>
using namespace std;
#include "otlv4.h"
#include "../third_party/CodingConv/encodeutil.h"


#if defined (__unix) || defined (__linux__) || defined(__QNX__) || defined (_AIX)
	#ifndef OS_LINUX
	#define OS_LINUX
	#endif
#elif defined (_WIN32) || defined (__WIN32) || defined (WIN32) || defined (__WIN32__)
	#ifndef OS_WIN32
	#define OS_WIN32
	#endif
#endif


#if defined  OS_LINUX
	#define strncpy_s(dest,num,src,count) strncpy(dest,src,count)
	#define sprintf_s  snprintf
	#include<iconv.h>
	int gb2312_to_utf8(char *in, char *out, size_t size);
	int utf8_to_gb2312(const char* sIn,char* sOut,size_t size);


#elif defined OS_WIN32 
		#pragma comment(lib,"oci.lib")
#endif



string DoubleToString(double n);
string IntToString(int);
#endif
