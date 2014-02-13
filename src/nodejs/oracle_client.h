
#ifndef  _ORACLE_CLIENT_H
#define _ORACLE_CLIENT_H
#include <node.h>
using namespace node;
using namespace v8;


class OracleClient : public ObjectWrap
{
public:
	static void Init(Handle<Object> target);
	static Handle<Value> New(const Arguments& args);
	static Handle<Value> CreateConnPool(const Arguments& args);

public:
	OracleClient();
	~OracleClient();
public:
	static Persistent<FunctionTemplate> s_ct; 
	
};
#endif