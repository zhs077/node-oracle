#ifndef CONNNECTION_H_
#define CONNNECTION_H_
#include <node.h>
using namespace node;
using namespace v8;
#include "../../core/otlPool.h"

class Connection :public ObjectWrap
{
public:
	static void Init(Handle<Object>target);
	static Handle<Value> New(const Arguments& args);
	static Handle<Value> Query(const Arguments& args);
	static Handle<Value> Execute(const Arguments& args);
	static Handle<Value> BeginTran(const Arguments& args);
	static Handle<Value> Commit(const Arguments& args);
	static Handle<Value> Rollback(const Arguments& args);
	Connection();
	~Connection();
	void setConnection(otl_connect* pconn);
	otl_connect*  getConnection();
private:
	otl_connect* pConn;
public:
	static Persistent<FunctionTemplate> constructorTemplate;


private:
	static void EIO_Execute(uv_work_t* req);
	static void EIO_After_Execute(uv_work_t* req, int status); //回调函数
	static void EIO_Query(uv_work_t* req);//查询
	static void EIO_After_Query(uv_work_t* req, int status); //回调函数 

};


#endif
