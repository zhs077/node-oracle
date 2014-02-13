
/**
* 数据库连接对象
*/
function Connection(conn){

	this.conn_ = conn;
}

function to_sql(sql,args){

	var re =/\?/; //正则表达式
 	var filter = args.shift();
	while(filter){
		sql=sql.replace(re,"'"+filter+"'");
		filter = args.shift();
	}
	return sql;
}
/** 
 *  调用形式 query('select * from table where name=?,'123',function(err,result){});
 * 支持的数据类型有 int String Date
 */
Connection.prototype.query = function(){

	var args = Array.prototype.slice.call(arguments,0);
	var callback = args.pop(); // callback
	var sql = args.shift();//sql
	sql = to_sql(sql,args);


	this.conn_.query_(sql,callback);
	
}
/**
 *  调用形式 execute('create table(id int)',function(err){});
 */
Connection.prototype.execute = function(){

	var args = Array.prototype.slice.call(arguments,0);
	var callback = args.pop(); // callback
	var sql = args.shift();//sql
	sql = to_sql(sql,args);
	this.conn_.execute_(sql,callback);
	
}
/**
 *  调用形式 beginTran(function(err){});
 */
Connection.prototype.beginTran = function(callback){

	this.conn_.beginTran_(callback);
	
}
/**
 *  调用形式 commit(function(err){});
 */
Connection.prototype.commit = function(callback){

	this.conn_.commit_(callback);
	
}
/**
 *  调用形式 rollback(function(err){});
 */
Connection.prototype.rollback = function(callback){

	this.conn_.rollback_(callback);
	
}

module.exports=Connection;
