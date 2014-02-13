var oracle = require('../');
var conn_pool = oracle.Pool('127.0.0.1:1521/lbs','scott','zhs',12,10);
conn_pool.acquire(function(err,conn){

		conn.execute("insert into zhs values(1,'郑华山')",function(err){
			if(err){
				console.log(err);
			}
			conn_pool.release(conn);
		});
});

