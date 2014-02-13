
#include "otlPool.h"
#include <iostream>
using namespace std;
unsigned int otlPool::total_con_cout = 0;
otlPool* otlPool::p_conn_pool = new otlPool();
otlPool* otlPool::create_conn_pool()
{
	
	return p_conn_pool;
}



otlPool::otlPool(void)
{
	p_con_list = NULL;
	max_con_count = 3;
	min_con_count = 2;
	b_startup = false;
	otl_connect::otl_initialize(1); //线程模型

}
std::string otlPool::get_error()const
{
	return str_error;
}

otlPool::~otlPool(void)
{
	otl_destroy();
	/*
	*用于中止ORACLE OCI８、９的数据库环境。它需要在程序最后连接数据库连接后被执行一次。
	该函数仅是对OCITerminate()函数的一个简单封装。在多线程环境里面，
	为了能够把主线程从数据库断开，该函数必须被调用，因为它会释放一些客户端并且处理其他的一些事情。
	*/
	otl_connect::otl_terminate();
	un_mutex_destroy(&mutex);
}

int otlPool::init_pool(std::string user_name,std::string pwd,std::string tns,unsigned int max_conn, unsigned int min_conn )
{
	user_name = user_name;
	pwd = pwd;
	tns = tns;
	max_con_count = (max_conn > DEFAULT_CON_COUNT) ? max_conn : DEFAULT_CON_COUNT;
	min_con_count = (min_conn > DEFAULT_CON_COUNT) ? min_conn : DEFAULT_CON_COUNT;
	conn_str = user_name +"/"+pwd +"@" +tns;
	try
	{

		//初始化互斥量
		if(0!=un_mutex_init(&mutex))
			abort();

		if(otl_malloc_conn() !=0)
		{
			printf("%s\n","otlPool Init Error");
			abort();
		}
		//启动连接池维护线程
		un_thread_t localThreadId;
		if(0!=un_thread_create(&localThreadId,otl_thread_fun,this))
			abort();
		//pthread_t tid;
		//assert(0==pthread_create(&tid,NULL,otl_thread_fun,this));

		b_startup = true;
		//un_thread_join(&localThreadId);
		//pthread_join(tid,NULL);
		un_sleep(1000);
		//pthread_detach(tid);
	

	}

	catch (otl_exception& e )
	{
		cout<<__FILE__<<":"<<"LINE:"<<__LINE__<<" error:"<<e.msg<<endl;
		str_error = (char*)e.msg;
		return -1;
	}
	catch (std::exception&e)
	{
		cout<<__FILE__<<":"<<"LINE:"<<__LINE__<<" error:"<<e.what()<<endl;
		str_error = e.what();
		return -1;

	}
	return 0;
}

otl_connect* otlPool::get_connect()
{
	int use = 0;
	otl_connect* pconn = NULL;
	otl_lock();
	otl_con_list::iterator it = p_con_list->begin();
	
	while (it != p_con_list->end())
	{
		use = (*it).second;
		pconn = (*it).first;
		if(use == 0)
		{
			(*it).second = 1;
			otl_unlock();
			return pconn;
		}

		++it;
		pconn = NULL;
		
	}
	//otl_unlock()

	//如果没有可用的连接，申请一个连接
	if(pconn == NULL &&  (p_con_list->size() < max_con_count))
	{
		
		try
		{
			//otl_unlock();
			pconn = new otl_connect;
			if(NULL == pconn)
			{
				otl_unlock();
				str_error = "new otl_connect fail may be not memory";
				return NULL;
			}
			pconn->rlogon(this->conn_str.c_str(),1);

		}
		catch(otl_exception& e)
		{
			delete pconn;
			pconn = NULL;
			cout<<__FILE__<<":"<<__LINE__<<"error:"<<e.msg<<endl;
			str_error = (char*)e.msg;

		}
		if(pconn )
		{
			//otl_lock();    //lock
			p_con_list->insert(otl_con_list::value_type(pconn,0));
			otl_unlock();  //unlock
			cout<<"new connect"<<endl;
			return pconn;

		}

	}
		otl_unlock();  //unlock
		str_error = "pool had empty";
		return NULL;
}

int otlPool::release_conn(otl_connect* pconn,bool is_delete_conn)
{
	
	if (pconn == NULL)
	{
		return 0;
	}
	otl_lock();
	otl_con_list::iterator it = p_con_list->find(pconn);
	if(it != p_con_list->end())
	{
		if(is_delete_conn == false)
		{
			(*it).second = 0;
			cout<<"release"<<endl;
			otl_unlock();
			return 0;
		}
		try
		{
			p_con_list->erase(pconn);
			pconn->logoff();
			delete pconn;
			pconn = NULL;

		}
		catch(otl_exception&e)
		{
			delete pconn;
			pconn = NULL;
			cout<<__FILE__<<":"<<"LINE:"<<__LINE__<<" error:"<<e.msg<<endl;
			str_error = (char*)e.msg;
			otl_unlock();
			return -1;
		}
	}

	otl_unlock();
	return 0;
}
int otlPool::otl_malloc_conn()
{
	otl_connect* pcon = NULL;
	this->otl_lock();
	if(this->p_con_list == NULL)
	{
		this->p_con_list = new otl_con_list();
	}

		while (this->p_con_list->size() < max_con_count)
		{
			try
			{
				//this->otl_unlock();
				pcon = new otl_connect();
				if(pcon == NULL)
				{
					otl_unlock();
					str_error = "new otl_connect fail may be not memory";
					return -1;
				}
				pcon->rlogon(this->conn_str.c_str(),1); //自动提交

			
			}
			catch(otl_exception&e)
			{
				delete pcon;
				pcon = NULL;
				this->otl_unlock();
				cout<<__FILE__<<":"<<__LINE__<<" error:"<<e.msg<<endl;
				str_error = (char*)e.msg;
				return -1;
			}
			
			//this->otl_lock();
			this->p_con_list->insert(otl_con_list::value_type(pcon,0));
			//this->otl_unlock();

			//this->otl_lock();   //lock
			

		}
	
	this->otl_unlock();
	return 0;

}

bool otlPool::otl_check_conn(otl_connect* pconn)
{
	
	int ret = -1;
	try
	{
		otl_stream stream;
		stream.open(1,"select 1 from dual",*pconn);
		if(!stream.eof())
		{
			stream>> ret;
			//cout<<ret<<endl;

		}
		stream.close();

	}
	
	catch (otl_exception& e)
	{
		cout<<__FILE__<<":"<<"LINE:"<<__LINE__<<" error:"<<e.msg<<endl;
		str_error = (char*)e.msg;
		return false;
	}
	return ret == 1 ? true :false;

}

void otlPool::otl_thread_fun(void* arg)
{
	otlPool* p_pool = static_cast<otlPool*>(arg);
	assert(p_pool);

	//cout<<"otl_thread_fun begin"<<endl;
	while (p_pool->b_startup == true)
	{
		//cout<<i++<<endl;
		otl_refresh_pool(p_pool);
		un_sleep(50);
		//un_sleep(50);
	}
	p_pool->otl_lock();
	if(p_pool->p_con_list !=NULL)
	{
		delete p_pool->p_con_list;
		p_pool->p_con_list = NULL;
	}
	p_pool->otl_unlock();
	un_mutex_destroy(&p_pool->mutex);
	//cout<<"otl_thread_fun end"<<endl;
	
}
void otlPool::otl_refresh_pool(void * pParam)
{
//	printf("begin refresh_pool\n");
	static time_t refresh_time = time(NULL); //记录开始的时间
	static time_t print_time = time(NULL);
	otlPool * pool = static_cast<otlPool*>(pParam);
	int use;
	otl_connect* pconn = NULL;
	if(time(NULL) - refresh_time  >= REFRESH_INTER_SENCOND ) //刷新
	{
		
		refresh_time = time(NULL);
		pool->otl_lock();
		//printf("otl_refresh_pool lock\n");
		//printf("begin refresh_pool\n");
		otl_con_list::iterator it = pool->p_con_list->begin();
		while (it != pool->p_con_list->end())
		{
			use = (*it).second;
			pconn = (*it).first;
			it++;

			if(use == 0)
			{
				if(!pool->otl_check_conn(pconn)) //连接不可用
				{
					pool->p_con_list->erase(pconn);
					delete pconn;
					pconn = NULL;
					it = pool->p_con_list->begin();
					printf("connect logoff connect delete conn\n");
				}
			}
			

		}
		pool->otl_unlock();
		//printf("otl_refresh_pool unlock\n");
		//printf("end refresh_pool\n");
		return;

	}
	//打印当前连接池的使用情况
	if(time(NULL) - print_time >= PRINT_POOL_TIME)
	{

		print_time = time(NULL);
		int used = 0;
		int no_used =0;
		int total = 0;
		pool->otl_lock();
		otl_con_list::iterator it = pool->p_con_list->begin();
		while(it != pool->p_con_list->end())
		{
			total++;
			if((*it).second == 0)
			{
				no_used++;
			}
			else if((*it).second == 1)
			{
				used++;
			}
			it++;
		}
		pool->otl_unlock();
		printf("current pool counts=%d,used=%d,free=%d\n",total,used,no_used);
		return ;

	}
	//维护连接池的大小，
	pool->otl_lock();
	if(pool->p_con_list->size() < pool->max_con_count)
	{
		try
		{
			//otl_unlock();
			pconn = new otl_connect;
			if(NULL == pconn)
			{
				pool->otl_unlock();
				return ;
			}
			pconn->rlogon(pool->conn_str.c_str(),1);

		}
		catch(otl_exception& e)
		{
			delete pconn;
			pconn = NULL;
			cout<<__FILE__<<":"<<__LINE__<<"error:"<<e.msg<<endl;;

		}
		if(pconn )
		{
			//otl_lock();    //lock
			pool->p_con_list->insert(otl_con_list::value_type(pconn,0));
			//pool->otl_unlock();  //unlock
			//cout<<"new connect"<<endl;
			//return;

		}
		
	}
	pool->otl_unlock();

	//维护连接池的大小，将最后一个没用的连接去掉
	
	pool->otl_lock();
	//printf("begin refresh_pool223232\n");
	otl_con_list::iterator it = pool->p_con_list->begin();
	while(it != pool->p_con_list->end())
	{
		if((*it).second == 1)
		{
			it++;
			continue;;
		}
		break;
	}
	if(it == pool->p_con_list->end())
	{	
		pool->otl_unlock();
		return ;
	}
	otl_con_list::iterator next =it;
	next++;
	if(next != pool->p_con_list->end() && pool->p_con_list->size() >pool->max_con_count)
	{
		pconn = (*next).first;
		if((*next).second == 0)
		{
			try
			{
				pool->p_con_list->erase(pconn);
				pconn->logoff();
				delete pconn;
				pconn = NULL;
			}
			catch(otl_exception&e)
			{
				delete pconn;
				pconn = NULL;
				cout<<__FILE__<<":"<<"LINE:"<<__LINE__<<" error:"<<e.msg<<endl;;
				
			}		

		}

	}
	pool->otl_unlock();
	//printf(" end begin refresh_pool223232\n");
}

void otlPool::otl_destroy()
{
	otl_lock();
	//printf("otl_destroy lock\n");
	otl_con_list::iterator it = p_con_list->begin();
	otl_connect* pconn = NULL;
	while (it != p_con_list->end())
	{
		pconn = (*it).first;
		p_con_list->erase(pconn);
		if(pconn->connected)
		{
			pconn->logoff();
		}
		delete pconn;
		pconn= NULL;
		it = p_con_list->begin();


	}
	
	
	b_startup =false;
	otl_unlock();
	//printf("otl_destroy unlock\n");

}
void otlPool::otl_lock()
{
	un_mutex_lock(&mutex);

}
void otlPool::otl_unlock()
{
	un_mutex_unlock(&mutex);
}
