#include "oracle_client.h"
#include "nodefunction.h"
#include "ConnctionPool.h"
#include "Connection.h"


OracleClient::OracleClient(){}
OracleClient::~OracleClient(){}
Persistent<FunctionTemplate> OracleClient::s_ct;

//初始化,告诉JS有哪些方法
void OracleClient::Init(Handle<Object> target)
{
	HandleScope scope;
	Local<FunctionTemplate> t = FunctionTemplate::New(New);
	s_ct = Persistent<FunctionTemplate>::New(t);
	s_ct->InstanceTemplate()->SetInternalFieldCount(1); //设定内部储存多少个内部变量
	s_ct->SetClassName(String::NewSymbol("oracle_client"));
	//添加函数

	NODE_SET_PROTOTYPE_METHOD(s_ct,"create_conn_pool",CreateConnPool);

	target->Set(String::NewSymbol("oracle_client"), s_ct->GetFunction());






}
//构造 JS new一个对象，C++同时要new一个对象
Handle<Value>OracleClient::New(const Arguments& args)
{
	HandleScope scope;
	//unsigned int count = args[0]->IsUndefined()? 20 : args[0]->Uint32Value();
	OracleClient* p_client = new OracleClient();
	p_client->Wrap(args.This()); //将p_client引用保存在args.This()
	return args.This();


}

//创建数据库连接池
Handle<Value>OracleClient::CreateConnPool(const Arguments& args)
{
	HandleScope scope;
	Handle<Value> argv[1];
	Handle<Object> connection = ConnctionPool::constructorTemplate->GetFunction()->NewInstance();
	ConnctionPool* pool =ObjectWrap::Unwrap<ConnctionPool>(connection);//获取指针
	string str_dblink = NodeFunc::ToCString(String::Utf8Value(args[0]));
	string user_name = NodeFunc::ToCString(String::Utf8Value(args[1]));
	string pass_word = NodeFunc::ToCString(String::Utf8Value(args[2]));
	unsigned int max_conn = args[3]->Int32Value();
	unsigned int min_conn = args[4]->Int32Value();
	static otlPool* p = pool->p_otl_pool;
	int ret = p->init_pool(user_name,pass_word,str_dblink,max_conn,min_conn);
	if(ret == 0)
	{
		argv[0] = connection;
	}
	else
	{
		argv[0]=Undefined();
		cout<<"create pool fail"<<endl;
	}

	return scope.Close(argv[0]);



}



extern "C" 
{
	static void init(Handle<Object> target)
	{
		OracleClient::Init(target);
		ConnctionPool::Init(target);
		Connection::Init(target);

	}

	NODE_MODULE(oracle_client, init);
}