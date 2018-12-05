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
	string buf = "";
	string sub="";

	char current[50];


	bool addr=false;
	//bool end=true;
	bool res;

	FILE *f;

	char *pos;
	int beg;
	int end;
	int size;

	char c;

	int cur_lvl=1;
	int slash_cnt=0;

	if(lvl!=1)
	{
		f = fopen(filename.c_str(), "r");

		while(fgets(current, sizeof(current), f))
		{
			if(!addr) pos = strstr(current, "<a");
			else pos = current;

			if(pos!=NULL || addr)
			{
				int i=0;

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
						if(*(pos+i)=='=' && *(pos+i+1)=='\"') 
						{
							i=i+2;
						}
						else 
						{
							if(*(pos+i)!='\"') 
							{
								c = *(pos+i);
								if(c!=0) tmp.push_back(c);

								if(c=='/') 
								{
									slash_cnt++;

									if(slash_cnt==2) beg = tmp.length();
									else if(slash_cnt==3) end = tmp.length()-1;
								}
								if(slash_cnt>=3) cur_lvl=slash_cnt-1;
							}
							else break;

							i++;
						}
					}

					if(current+50==pos+i) 
					{
						continue;
					}
					else
					{
						if((!tmp.substr(0,7).compare("http://") || !tmp.substr(0,8).compare("https://") || !tmp.substr(0,6).compare("ftp://")) && lvl>1)
						{
							if(no_parent)
							{
								buf = tmp.substr(beg, tmp.length());
								size = string(hostname).length();
								sub = buf.substr(0,size);
								res = sub.compare((string)hostname);

								if(!res)
								{
									printf("%s\n", tmp.c_str());
									link_list.push_back((char*)tmp.c_str());
								}
						
								cur_lvl=1;
								slash_cnt=0;
								buf="";
								sub="";
							}
							else
							{
								buf = tmp.substr(beg, end-beg);
								sub = (string)hostname;
								size = buf.length();
								res = buf.compare(sub.substr(0,size));

								if(!res)
								{
									printf("%s\n", tmp.c_str());
									link_list.push_back((char*)tmp.c_str());
								}
						
								cur_lvl=1;
								slash_cnt=0;
								buf="";
								sub="";
							}
						}
						else
						{
							if(!tmp.substr(0,1).compare("/"))
							{
								buf = "http://";
								buf.append((string)hostname);
								buf.append(tmp);
								tmp=buf;

								printf("%s\n", tmp.c_str());
								link_list.push_back((char*)tmp.c_str());
							}
						}
						tmp="";
						slash_cnt=0;
						buf="";
						sub="";
						addr=false;
						
					}

				}

			}
		}	

		fclose(f);

	}
	else
	{
		printf("%s\n", hostname);
		link_list.push_back(hostname);
	}
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
