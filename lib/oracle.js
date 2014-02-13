var oracle = require('../build/Release/oracle_client');
var Connection = require('./connection');
var node_oracle = new oracle.oracle_client();

/**
 *
 * @param tns  数据库实例
 * @param user_name 用户名
 * @param pwd 密码
 * @param max_conn 最大的数据库连接数
 * @param min_conn 最小的数据库连接数
 * 返回数据库连接池
 */
exports.Pool=function(tns,user_name,pwd, max_conn,min_conn){
    var conn_pool = {};
    conn_pool = node_oracle.create_conn_pool(tns,user_name,pwd,max_conn,min_conn);


    /**
     *获取连接
     * @param callback(err,conn)
     */
    conn_pool.acquire = function(callback){
       conn_pool.acquire_(function(err,conn_){
       
       			if(err){
       				callback(err,null)
       			}
       			else{
       				var conn = new Connection(conn_)
       				callback(null,conn);
       			}
       			
       });
       
       
    }
    /**
     *释放连接
     * @param Conn
     * @private
     */
        conn_pool.release = function(Conn){
       conn_pool.release_(Conn.conn_);

    }

    return conn_pool;
}
