var oracle = require('../');
var conn_pool = oracle.Pool('127.0.0.1:1521/lbs','scott','zhs',12,10);
conn_pool.acquire(function(err,conn){
	if(err){
		console.error(err);
	}
	conn.beginTran(function(err){
		if(err){
			console.error(err);
			return;
		}
		conn.execute('insert into zhs values(?,?)',10,'abc',function(err){
			if(err){
					console.log(err);
				conn.rollback(function(err){});
			}else{
			conn.rollback(function(err){
			conn.execute('insert into zhs values(?,?)',100,'xlp',function(err){
				//conn.commit(function(err){})
				conn_pool.release(conn);
			});
					
			});
			}
		
		});
		
	});
});
	


