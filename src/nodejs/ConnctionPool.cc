#include "ConnctionPool.h"
#include "ConnJob.h"
#include "Connection.h"


class otlPool;
//创建个连接池对象
otlPool* ConnctionPool::p_otl_pool = otlPool::create_conn_pool();

Persistent<FunctionTemplate> ConnctionPool::constructorTemplate;


ConnctionPool::ConnctionPool(void)
{ 
}


ConnctionPool::~ConnctionPool(void)
{
}

void ConnctionPool::Init(Handle<Object> target)
{
	HandleScope scope;
	Local<FunctionTemplate> t = FunctionTemplate::New(New);
	constructorTemplate = Persistent<FunctionTemplate>::New(t);
	p_otl_pool = otlPool::create_conn_pool();
	constructorTemplate->InstanceTemplate()->SetInternalFieldCount(1);
	constructorTemplate->SetClassName(String::NewSymbol("conn_pool"));
	NODE_SET_PROTOTYPE_METHOD(constructorTemplate,"acquire_",GetConnn);
	NODE_SET_PROTOTYPE_METHOD(constructorTemplate,"release_",ReleaseConn);
	target->Set(String::NewSymbol("conn_pool"), constructorTemplate->GetFunction());




}
Handle<Value>ConnctionPool::New(const Arguments& args)
{
	HandleScope scope;
	ConnctionPool* p_conn_pool = new ConnctionPool();
	p_conn_pool->Wrap(args.This());
	return args.This();

}

Handle<Value>ConnctionPool::GetConnn(const Arguments& args)
{
	
	HandleScope scope;
	ConnctionPool* pool = ObjectWrap::Unwrap<ConnctionPool>(args.This());
	Conn_Job* job = new Conn_Job();
	job->callback = Persistent<Object>::New(args[args.Length()-1]->ToObject());
	job->p_client = pool;
	job->req.data = job;
	pool->Ref();
	//cout<<"getconn2"<<endl;
	uv_queue_work(uv_default_loop(), &job->req, EIO_Get_Connect, (uv_after_work_cb)EIO_AfterConnect);

	return Undefined();
}

void ConnctionPool::EIO_Get_Connect(uv_work_t* req)
{
	//cout<<"Connect"<<endl;
	Conn_Job* job = static_cast<Conn_Job*>(req->data);
	otl_connect* pconn = job->p_client->p_otl_pool->get_connect();
	if(pconn == NULL)
	{
		//cout<<"get conn fail"<<endl;
		job->str_error= job->p_client->p_otl_pool->get_error();
		job->p_conn = NULL;
	}
	else
	{
		job->p_conn = pconn;

	}



}
void ConnctionPool::EIO_AfterConnect(uv_work_t* req, int status)
{
	//cout<<" after Connect"<<endl;
	HandleScope scope;
	Handle<Value> argv[2];
	Conn_Job* job = static_cast<Conn_Job*>(req->data);
	job->p_client->Unref();
	if(!job->str_error.empty())
	{
	
#ifdef OS_WIN32
		job->str_error = encodeConv::CodingConv::ascii2Utf8(job->str_error.c_str());
#endif
		argv[0] = String::New(job->str_error.c_str());
		argv[1]=Undefined();

	}
	else
	{
		argv[0] = Undefined();
		Handle<Object> connection = Connection::constructorTemplate->GetFunction()->NewInstance();
		//Connection* p = ObjectWrap::Unwrap<Connection>(connection);
		(node::ObjectWrap::Unwrap<Connection>(connection))->setConnection(job->p_conn);
		argv[1] = connection;
	}

	TryCatch tryCatch;
	job->callback->CallAsFunction(Object::New(),2,argv);
	if (tryCatch.HasCaught()) 
	{
		node::FatalException(tryCatch);
	}
	job->callback.Dispose();
	delete job;
	job = NULL;
	scope.Close(Undefined());


}
Handle<Value>ConnctionPool::ReleaseConn(const Arguments& args)
{
	HandleScope scope;
	Connection * pConn = ObjectWrap::Unwrap<Connection>(args[0]->ToObject());
	ConnctionPool*pool = ObjectWrap::Unwrap<ConnctionPool>(args.This());
	int ret =pool->p_otl_pool->release_conn(pConn->getConnection());
	bool b = ret == 0 ? true:false;
	return scope.Close(Boolean::New(b));

	

}
