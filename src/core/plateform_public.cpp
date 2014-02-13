#include "platform_config.h"

/*
// 将一个Double型转换为string型
string DoubleToString(double n)
{
	string s;
	//设置精度
	int prec = numeric_limits<long double>::digits10;
	ostringstream is;
	is << n;
	s = is.str();
	return s;
}
string IntToString(int n)
{
	stringstream is;
	string s;

	is << n;
	is >> s;
	return s;
}*/
#if defined (OS_LINUX)
int gb2312_to_utf8(char *in, char *out, size_t size) 
{

         //使用iconv转换GB2312为UTF-8编码

        iconv_t cd;
         cd = iconv_open("UTF-8//IGNORE", "GB2312");  //特殊符号那么就跳过错误，而不是默认的停在那里。
         if ( cd == (iconv_t)(-1) ) 
			{
            	 perror("iconv_open failed");
            	 return 0;
         	}

         size_t in_left = strlen(in) + 1;
         char *out_ptr;
         size_t res;

         out_ptr = out;
         res = iconv(cd, &in, &in_left, &out_ptr, &size);
         if ( res == (size_t) (-1) )
			{
             perror("iconv failed");
             return 0;
         	}

         iconv_close(cd);
         return 1;
     }

int utf8_to_gb2312(const char* sIn,char* sOut,size_t size)
{

	char *pIn = (char *)sIn;
	char *pOut = sOut;
	iconv_t cd;
	cd = iconv_open("GB2312//IGNORE", "UTF-8");
	if (cd == (iconv_t) - 1)
	{
		 perror("iconv_open failed");
		return 0;
	}
	size_t in_left = strlen(pIn) + 1;
	size_t ret = iconv(cd, &pIn,&in_left, &pOut,&size);
	if (ret == (size_t) - 1)
	{
		iconv_close(cd);
		perror("iconv failed");
		return 0;
	}

	iconv_close(cd);
	return 1;

}

#endif


