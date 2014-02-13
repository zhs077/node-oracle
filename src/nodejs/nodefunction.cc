#include"nodefunction.h"


namespace NodeFunc
{
	const char* ToCString(const String::Utf8Value& value)
	{	
		return *value ? *value: "string conversion failed";
	}
	string Replace( const string& orignStr, const string& oldStr, const vector<string>&vc ) 
	{ 
		size_t pos = 0; 
		string tempStr = orignStr; 
		string::size_type newStrLen; 
		string::size_type oldStrLen = oldStr.length();
		int i=0;

		while(true) 
		{ 
			pos = tempStr.find(oldStr, pos); 
			if (pos == string::npos) break; 
			string s=vc.at(i);
			s = "\'"+s +"\'";
			newStrLen= s.length(); 

			tempStr.replace(pos, oldStrLen, s);         
			pos += newStrLen;
			i++;

		} 

		return tempStr; 
	}
	string MergeSql(const Arguments&args)
	{
		String::Utf8Value str(args[0]);
		const char * pstr = ToCString(str);
		string s1 = pstr;
#ifdef OS_WIN32
		s1 = encodeConv::CodingConv::utf82Ascii(pstr);
#endif

		vector<string> vc;
		int length = args.Length();
		if (args[length-1]->IsFunction())
		{
			length -= 1;
		}

		for(int i = 0; i<length;i++)
		{
			String::Utf8Value str1(args[i+1]);
			const char * pstr_tmp = ToCString(str1);
			string tmp = pstr_tmp;
#ifdef OS_WIN32
			tmp = encodeConv::CodingConv::utf82Ascii(pstr_tmp);
#endif
			vc.push_back(tmp);

		}
		string sql=Replace(s1,"?",vc);

		return sql;


	}
};