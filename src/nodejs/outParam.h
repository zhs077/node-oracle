
/************************************************************************/
/*         ≤È—Ø”Ôæ‰ ‰≥ˆ                                                        */
/************************************************************************/

#ifndef OUT_PARAM_H_
#define OUT_PARAM_H_
#include "../../core/platform_config.h"
#include "Connection.h"
#include<node.h>
#include <v8.h>
using namespace v8;
using namespace node;

class OutParam :public ObjectWrap
{
public:
	static void Init(Handle<Object>target);
	static Handle<Value>New(const Arguments& args);
	OutParam();
	~OutParam();
public:
	string str_sql;//sql”Ôæ‰

private:

};

#endif