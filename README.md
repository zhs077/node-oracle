node-oracle
===========

nodejs oracle驱动
支持linux windows


在linux系统编译时候，需要设置ORACLE_HOME和LD_LIBRARY_PATH环境变量，否则在编译和运行会报错。
export ORACLE_HOME=/home/zhs/oracle/product/10.2.0/db_2
export LD_LIBRARY_PATH=$ORACLE_HOME/lib;

如果安装数据库的字符集为ZHS16GBK,需要在运行NODEJS程序前先设置环境变量，否则中文会乱码。
 export NLS_LANG=AMERICAN_AMERICA.ZHS16GBK

