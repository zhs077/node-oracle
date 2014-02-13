var oracle = require('../');

var conn_pool = oracle.Pool('127.0.0.1:1521/lbs','scott','zhs',12,10);
conn_pool.acquire(function(err,conn){
	console.log(err);
	console.log('conn'+conn);

	
});

