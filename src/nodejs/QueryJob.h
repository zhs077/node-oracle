
/************************************************************************/
/*                                                                 */
/************************************************************************/

#ifndef QUERY_JOB_H_
#define QUERY_JOB_H_
#include "../../core/platform_config.h"
#include "Connection.h"
#include<node.h>
using namespace v8;
using namespace node;

enum 
{
	VALUE_TYPE_NULL = 1,
	VALUE_TYPE_DOUBLE = 2,
	VALUE_TYPE_STRING = 3,
	VALUE_TYPE_INT = 4,
	VALUE_TYPE_DATE = 5,
	VALUE_TYPE_TIMESTAMP = 6,
	VALUE_TYPE_CLOB = 7,
	VALUE_TYPE_BLOB = 8
};
/*1列*/
typedef struct column_t 
{
	unsigned int type;
	string column_name;//名字

}column_t ;
/*1行*/
typedef struct row_t 
{
	std::vector<void*> values;
}row_t;
/*结果集*/
typedef struct output_t 
{
	int type;
	int index;
	std::string strVal; 
	int intVal;
	double doubleVal;
	float floatVal;
	vector<column_t> columns;
	vector<row_t> rows;

}output_t ;





class QueryJob
{
public:
	QueryJob(){pClinet = NULL;};
	~QueryJob(){};
public:
	string str_sql;//sql语句
	string str_error;//错误
	uv_work_t req;
	Persistent<Object>callback;//JS回调函数
	Connection* pClinet;//
	//vector<output_t> outputs; //oracle返回的数据
	vector<column_t> columns;//列
	vector<row_t> rows;//行数


private:

};

#endif