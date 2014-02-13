#include "Connection.h"
#include "nodefunction.h"
#include "../../core/platform_config.h"
#include "QueryJob.h"
#include "../../core/otlv4.h"
Persistent<FunctionTemplate>Connection:: constructorTemplate;
Connection::Connection()
{
	pConn = NULL;
}

Connection::~Connection()
{
	if(pConn)
	{
		delete pConn;
		pConn = NULL;
	}
}

void Connection::Init(Handle<Object>target)
{
	HandleScope scope;
	Local<FunctionTemplate> l = FunctionTemplate::New(New);
	constructorTemplate = Persistent<FunctionTemplate>::New(l);
	constructorTemplate->InstanceTemplate()->SetInternalFieldCount(1);
	constructorTemplate->SetClassName(String::NewSymbol("Connection"));
	NODE_SET_PROTOTYPE_METHOD(constructorTemplate,"execute_",Execute);
	NODE_SET_PROTOTYPE_METHOD(constructorTemplate,"query_",Query);
	NODE_SET_PROTOTYPE_METHOD(constructorTemplate,"beginTran_",BeginTran);
	NODE_SET_PROTOTYPE_METHOD(constructorTemplate,"commit_",Commit);
	NODE_SET_PROTOTYPE_METHOD(constructorTemplate,"rollback_",Rollback);
	target->Set(String::NewSymbol("Connection"), constructorTemplate->GetFunction());


}

Handle<Value> Connection::New(const Arguments& args)
{
	HandleScope scope;
	Connection* pconn = new Connection();
	pconn->Wrap(args.This());
	return args.This();

}

void Connection::setConnection(otl_connect* pconn)
{
	this->pConn = pconn;
}
otl_connect* Connection::getConnection()
{
	return pConn;
}
Handle<Value> Connection::Execute(const Arguments& args)
{
	HandleScope scope;
	Connection* pConn = ObjectWrap::Unwrap<Connection>(args.This());
	string sql =NodeFunc::MergeSql(args);
	QueryJob* job = new QueryJob();
	job->str_sql = sql;
	job->callback = Persistent<Object>::New(args[args.Length()-1]->ToObject());
	job->pClinet = pConn;
	job->req.data = job;
	pConn->Ref();
	
	uv_queue_work(uv_default_loop(),&job->req,EIO_Execute,EIO_After_Execute);
	
	return Undefined();
	
}
void Connection::EIO_Execute(uv_work_t* req)
{
	QueryJob* job = static_cast<QueryJob*>(req->data);
	try
	{
#ifdef OS_LINUX
		//UTF-8 To GB2312
		size_t length = job->str_sql.size()+1;
		char *p_gb2312 = new char[length];
		memset(p_gb2312,0,length);
		utf8_to_gb2312(job->str_sql.c_str(),p_gb2312,length);
		job->pClinet->pConn->direct_exec(p_gb2312);
		delete [] p_gb2312;
#else
		job->pClinet->pConn->direct_exec(job->str_sql.c_str());
#endif
		//job->pClinet->pConn->commit();
	}
	catch (otl_exception& e)
	{
		cout<<__FILE__<<":"<<"LINE:"<<__LINE__<<" error:"<<e.msg<<endl;
		job->str_error = (char*)e.msg;

	}
	catch (std::exception& e)
	{
		cout<<__FILE__<<":"<<"LINE:"<<__LINE__<<" error:"<<e.what()<<endl;
		job->str_error = e.what();
	}

	
}
void Connection::EIO_After_Execute(uv_work_t* req, int status)
{
	HandleScope scope;
	QueryJob* job = static_cast<QueryJob*>(req->data);
	job->pClinet->Unref();
	Handle<Value>argv[1];
	if(!job->str_error.empty())
	{
		cout<<job->str_error<<endl;
#ifdef OS_WIN32
		job->str_error = encodeConv::CodingConv::ascii2Utf8(job->str_error.c_str());
#endif
		argv[0] = String::New(job->str_error.c_str());	

	}
	else
	{
		argv[0] = Undefined();
	}
	TryCatch tryCatch;
	job->callback->CallAsFunction(Object::New(),1,argv);
	if (tryCatch.HasCaught()) 
	{
		node::FatalException(tryCatch);
	}
	job->callback.Dispose();
	delete job;
	job = NULL;
	scope.Close(Undefined());

}

Handle<Value> Connection::BeginTran(const Arguments& args)
{
	HandleScope scope;
	Local<Function>cb = Local<Function>::Cast(args[0]);
	Connection* pConn = ObjectWrap::Unwrap<Connection>(args.This());
	Local<Value> argv[1];
	string error;

	try
	{
		pConn->pConn->auto_commit_off();//取消自动提交
		argv[0] = *Undefined();
	}
	catch(otl_exception& e)
	{
		cout<<__FILE__<<":"<<"LINE:"<<__LINE__<<" error:"<<e.msg<<endl;
		error =(char*)e.msg;
		argv[0] = Local<Value>::New(String::New(error.c_str()));

	}
	//argv[0] = Local<Value>::New(String::New(error.c_str()));
	cb->Call(Context::GetCurrent()->Global(),1,argv);
	return scope.Close(Undefined());
}
Handle<Value> Connection::Commit(const Arguments& args)
{
	HandleScope scope;
	Local<Function>cb = Local<Function>::Cast(args[0]);
	Connection* pConn = ObjectWrap::Unwrap<Connection>(args.This());
	Local<Value> argv[1];
	string error="";

	try
	{
		pConn->pConn->commit();
		//cout<<"commit"<<endl;
		pConn->pConn->auto_commit_on();//回复自动提交
		argv[0] = *Undefined();
	}
	catch(otl_exception& e)
	{
		cout<<__FILE__<<":"<<"LINE:"<<__LINE__<<" error:"<<e.msg<<endl;
		error =(char*)e.msg;
		argv[0] = Local<Value>::New(String::New(error.c_str()));

	}
	
	cb->Call(Context::GetCurrent()->Global(),1,argv);
	
	return scope.Close(Undefined());
}
Handle<Value> Connection::Rollback(const Arguments& args)
{
	HandleScope scope;
	Local<Function>cb = Local<Function>::Cast(args[0]);
	Connection* pConn = ObjectWrap::Unwrap<Connection>(args.This());
	Local<Value> argv[1];
	string error;

	try
	{
		pConn->pConn->rollback();//
		pConn->pConn->auto_commit_on();//回复自动提交
		argv[0] = *Undefined();
	}
	catch(otl_exception& e)
	{
		cout<<__FILE__<<":"<<"LINE:"<<__LINE__<<" error:"<<e.msg<<endl;
		error =(char*)e.msg;
		argv[0] = Local<Value>::New(String::New(error.c_str()));

	}
	cb->Call(Context::GetCurrent()->Global(),1,argv);
	return scope.Close(Undefined());
}


Handle<Value>Connection::Query(const Arguments& args)
{
	HandleScope scope;
	Connection* pConn = ObjectWrap::Unwrap<Connection>(args.This());
	string sql =NodeFunc::MergeSql(args);
	QueryJob* job = new QueryJob();
	job->str_sql = sql;
	job->callback = Persistent<Object>::New(args[args.Length()-1]->ToObject());
	job->pClinet = pConn;
	job->req.data = job;
	pConn->Ref();
	uv_queue_work(uv_default_loop(),&job->req,EIO_Query,EIO_After_Query);
	return scope.Close(Undefined());
}

void Connection::EIO_Query(uv_work_t* req)
{

	QueryJob* job = static_cast<QueryJob*>(req->data);

	otl_column_desc *desc;
	int field_count = 0;
	unsigned int count=0;
	string str;
	int ivalue;
	double dvalue=0.0f;
	char *pstr = NULL;
	double *pdouble = NULL;
	int *pint = NULL;
	otl_datetime dv;
	stringstream ss;
	stringstream tmp;
	
	try
	{
		//cout<<job->str_sql.c_str()<<endl;
		#ifdef OS_LINUX
		//UTF-8 To GB2312
		size_t length = job->str_sql.size()+1;
		char *p_gb2312 = new char[length];
		memset(p_gb2312,0,length);
		utf8_to_gb2312(job->str_sql.c_str(),p_gb2312,length);
		otl_stream ostr (10000,p_gb2312, *job->pClinet->pConn);
		delete [] p_gb2312;
		#else
			otl_stream ostr (10000,job->str_sql.c_str(), *job->pClinet->pConn);
		#endif
		//otl_stream ostr (10000,job->str_sql.c_str(), *job->pClinet->pConn);
		//otl_connect db;
		//otl_connect::otl_initialize(1); //线程模型
		//db.rlogon("scott/zhs@127.0.0.1:1522/lbs");
		
		
		desc = ostr.describe_select(field_count);
		//cout<<"field_count:"<<field_count<<endl;
	
		int i=0;
		for (i=0; i < field_count;++i)
		{
			//desc[i].name_len_
			column_t columm;
			if(desc[i].dbtype == SQLT_CHR || desc[i].dbtype == SQLT_VCS || desc[i].dbtype == SQLT_AFC )
			{
				columm.type =VALUE_TYPE_STRING;
			}
			else if(desc[i].dbtype == SQLT_NUM && desc[i].scale >0)
			{
					columm.type =VALUE_TYPE_DOUBLE;
			}
			else if((desc[i].dbtype == SQLT_NUM && desc[i].scale <=0) || desc[i].dbtype ==SQLT_INT || desc[i].dbtype ==SQLT_LNG || desc[i].dbtype ==SQLT_UIN )
			{
				columm.type = VALUE_TYPE_INT;
			}
			else if(desc[i].dbtype == SQLT_DAT)
			{
				columm.type = VALUE_TYPE_DATE;
			}
			columm.column_name = desc[i].name;
			job->columns.push_back(columm);
			//cout<<columm.column_name<<endl;
			//cout<<desc[i].dbtype<<endl;
			
		}
		while (!ostr.eof())
		{
			//row_t t;
			//cout<<"1111"<<endl;
			count++;
			row_t row;
				for (int i=0; i < field_count;++i)
				{
					
					switch (desc[i].dbtype)
					{
					case SQLT_CHR:  //VARCHAR2
					case SQLT_VCS:  //VARCHAR
					case SQLT_AFC: //CHAR
					 ostr >> str;
					// output_t.rows.
					 pstr = new char[desc[i].dbsize+1];
					 memset(pstr,0,sizeof(desc[i].dbsize+1));
					 memcpy(pstr,str.c_str(),str.length());
					 row.values.push_back(pstr);
					// cout<<pstr<<endl;
					 
					 break;
					case SQLT_NUM:  //DOUBLE
						if(desc[i].scale >0)
						{
							ostr >> dvalue;
							pdouble = new double;
							*pdouble = dvalue;
							 row.values.push_back(pdouble);
						}
						else
						{
							ostr >> ivalue;
							pint = new int;
							*pint = ivalue;
							row.values.push_back(pint);
							//cout<<*pint<<"pint"<<endl;
						}

						break;
					case SQLT_INT: //INTEGER
					case SQLT_LNG: //LONG
					case SQLT_UIN: //UNSIGNED INT
						ostr >> ivalue;
						
						 pint = new int;
						*pint = ivalue;
						 row.values.push_back(pint);
						 //cout<<*pint<<"int"<<endl;
						break;
					case SQLT_DAT:
						ostr >> dv;
						ss.str("");
						ss<<dv.year<<"-"<<dv.month<<"-"<<dv.day<<" "<<dv.hour<<":"<<dv.minute<<":"<<dv.second;
						if(ostr.is_null())
						{
							 pstr = new char[1];
							 pstr[0]='\0';
						}	
						else
						{
							str = ss.str();
							pstr = new char[str.size()+1];
							memset(pstr,0,sizeof(pstr));
						}
						memcpy(pstr,str.c_str(),str.length());
						row.values.push_back(pstr);
						break;
						
					default:
						break;
					}
						
				}

				job->rows.push_back(row);

		}
		//cout<<count<<endl;

	}
	catch(otl_exception& e)
	{
		cout<<__FILE__<<":"<<"LINE:"<<__LINE__<<" error:"<<e.msg<<endl;
		job->str_error =(char*)e.msg;

	}




}
void Connection::EIO_After_Query(uv_work_t* req, int status)
{
	HandleScope scope;
	QueryJob* job = static_cast<QueryJob*>(req->data);
	job->pClinet->Unref();
	Handle<Value>argv[2];
	if(!job->str_error.empty())
	{
		cout<<job->str_error<<endl;
#ifdef OS_WIN32
		job->str_error = encodeConv::CodingConv::ascii2Utf8(job->str_error.c_str());
#endif
		argv[0] = String::New(job->str_error.c_str());
		argv[1] = Undefined();

	}
	else
	{
		argv[0] = Undefined();
		size_t totalRows = job->rows.size();
		Local<Array> retRows = Array::New(totalRows);
		vector<row_t>::iterator it = job->rows.begin();
		int index=0;
		char* pstr = NULL;
		double* pdouble = NULL;
		int* pint = NULL;
		int length=0;
		char* buffer=NULL;
		for (;it!= job->rows.end();++it,index++)
		{
			 Local<Object> obj= Object::New();
			for (unsigned i=0; i < job->columns.size();i++)
			{
				column_t columm = job->columns.at(i);
				switch (columm.type)
				{
				case VALUE_TYPE_STRING:
				
				
						pstr =(char*)it->values.at(i);
						#ifdef OS_LINUX
						//cout<<pstr<<endl;
						length = strlen(pstr)*1.5 +1;
						buffer = new char[length];
						memset(buffer,0,length);
						gb2312_to_utf8(pstr,buffer,length);
						obj->Set(String::New(columm.column_name.c_str()),String::New(buffer));
					delete []pstr;
					delete []buffer;
				#else
					obj->Set(String::New(columm.column_name.c_str()),String::New(pstr));
					delete []pstr;
				#endif
					
					break;
				case VALUE_TYPE_INT:
					pint = (int*)it->values.at(i);
					obj->Set(String::New(columm.column_name.c_str()),Integer::New(*pint));
					delete pint;

					break;
				case  VALUE_TYPE_DOUBLE:
					pdouble = (double*)it->values.at(i);
		
					obj->Set(String::New(columm.column_name.c_str()),Number::New(*pdouble));
					delete pdouble;
					break;
				case VALUE_TYPE_DATE:
					pstr =(char*)it->values.at(i);
					obj->Set(String::New(columm.column_name.c_str()),String::New(pstr));
					delete []pstr;
					break;
				default:
					cout << "Unhandled type: " << columm.type<<endl;;
			
					break;
				}
			}
			   retRows->Set(index, obj);
		}
		argv[1]=retRows;
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

