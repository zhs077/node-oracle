#ifndef _NODE_FUNCTION_H_
#define _NODE_FUNCTION_H_

/************************************************************************/
/* 一些nodejs用到的公共函数                                             */
/************************************************************************/
#include<node.h>
using namespace v8;
using namespace node;
#include"../../core/platform_config.h"
#include "../../third_party/CodingConv/encodeutil.h"
namespace NodeFunc
{
	const char* ToCString(const String::Utf8Value& value);
	string Replace( const string& orignStr, const string& oldStr, const vector<string>&vc );
	string MergeSql(const Arguments&args);
};
#endif
