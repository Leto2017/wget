#include <iostream>
#include <vector>
#include <string.h>
#include <stdio.h>

using namespace std;

void parse_text( vector<string> &link_list, string string_to_parse)
{
	string tmp = "";

	bool addr=false;

	for(size_t i=0; i<string_to_parse.size(); i++)
	{
		if((!string_to_parse.substr(i,7).compare("http://") || !string_to_parse.substr(i,8).compare("https://"))&& !addr)
		{
			addr=!addr;
			tmp.push_back(string_to_parse[i]);
			continue;
		}

		if(addr && string_to_parse[i]!=' ')
		{
			tmp.push_back(string_to_parse[i]);
			if(i==string_to_parse.size()-1)
			{
				link_list.push_back(tmp);
				tmp="";
				addr=!addr;
			}
		}
		else if(addr && string_to_parse[i]==' ')
		{
			link_list.push_back(tmp);
			tmp="";
			addr=!addr;
		}
	}
}

int main()
{
	vector<string> vec;

	string str="I love sites like http://google.com , а если на русском, то https://yandex.ru";

	parse_text(vec, str);

	for(int i=0; i<vec.size(); i++)
		printf("%s\n", vec[i].c_str());
}
