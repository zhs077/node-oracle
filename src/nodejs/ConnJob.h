#ifndef _CONN_JOB_H_
#define _CONN_JOB_H_

#include "../../core/platform_config.h"
#include <node.h>

class otl_connect;
class ConnctionPool;
class Conn_Job
{
public:
	Conn_Job();
	~Conn_Job();


public:
	string str_dblink;
	string str_user_name;
	string str_pass_word;
	string str_error;
	ConnctionPool* p_client;
	otl_connect* p_conn;
	uv_work_t req;
	Persistent<Object>callback;



};



Conn_Job::Conn_Job()
{
	p_client = NULL;
}

Conn_Job::~Conn_Job()
{
}
#endif