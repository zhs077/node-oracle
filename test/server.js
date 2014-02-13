/**
 * Created with JetBrains WebStorm.
 * User: zhs
 * Date: 13-12-18
 * Time: 上午10:22
 * To change this template use File | Settings | File Templates.
 */

var http = require('http');
var oracle = require('../');
var conn_pool = oracle.Pool('127.0.0.1:1521/lbs','scott','zhs',5,1);

http.createServer(function(req,res){

	conn_pool.acquire(function(err,conn){
		if(err){
			 res.end(err);
			 return;
		}
		conn.query("select * from zhs where name=?",'郑华山',function(err,result){
		if(err){
		conn_pool.release(conn);
			res.end(err);
			return;
		}
		conn_pool.release(conn);
		console.log(result);
		res.end(''+result);
	})

	});

   
    
}).listen(8888);
