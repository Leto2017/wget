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

void parse_html( vector<char*> &link_list, string filename, char* hostname, bool no_parent, int lvl)
{
	string tmp = "";
	char current[50];


	bool addr=false;
	//bool end=true;
	//bool read = true;

	FILE *f;

	char *pos;
	char c;

	int cur_lvl=1;
	int slash_cnt=0;

	f = fopen(filename.c_str(), "r");

	while(fgets(current, sizeof(current), f))
	{
		if(!addr) pos = strstr(current, "<a");
		else pos = current;

		if(pos!=NULL || addr)
		{
			int i=0;
			//end = false;

			while(*(pos+i)!='>' && !(current+46==pos+i) && !addr)
			{
				if(*(pos+i)=='h' && *(pos+i+1)=='r' && *(pos+i+2)=='e' && *(pos+i+3)=='f')
				{
					i=i+4;
					addr = true;
					break;
				}
				else i++;
			}

			if(addr)
			{
				while((*(pos+i)!='>') && !(current+50==pos+i))
				{
					if(*(pos+i)=='=' && *(pos+i+1)=='\"') i=i+2;
					else 
					{
						if(*(pos+i)!='\"') 
						{
							c = *(pos+i);
							if(c!=0) tmp.push_back(c);

							if(c=='/') slash_cnt++;
							if(slash_cnt>=3) cur_lvl=slash_cnt-1;
						}
						else break;

						i++;
					}
				}

				if(current+50==pos+i) 
				{
					//end = true;
					continue;
				}
				else
				{
					if((!tmp.substr(0,7).compare("http://") || !tmp.substr(0,8).compare("https://")) && lvl>1)
					{
						
						printf("%s\n", tmp.c_str());
						link_list.push_back((char*)tmp.c_str());
				
						cur_lvl=1;
						slash_cnt=0;
					}
					tmp="";
					addr=false;
					//end = false;
				}

			}

		}
	}	

	fclose(f);


}

int main()
{
	vector<string> vec;
	vector<char*> vec1;

	string str="I love sites like http://google.com , а если на русском, то https://yandex.ru";

	parse_text(vec, str);

	//for(int i=0; i<vec.size(); i++)
	//	printf("%s\n", vec[i].c_str());

	parse_html(vec1, "in.txt", "msu.uz", false, 2);
}
