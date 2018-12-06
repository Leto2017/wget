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

void parse_html( vector<string> &link_list, string filename, char* hostname, bool no_parent, int lvl)
{
	string tmp = "";
	string buf = "";
	string sub="";
	string buf_w="";

	char current[50];


	bool addr=false;

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
								if(c!=0) 
								{
									tmp.push_back(c);
									if(slash_cnt==2)
									{
										if(buf_w.length()<4) buf_w.push_back(c);
									}
								}

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
								if(!buf_w.compare("www.")) beg=beg+4;
								
								buf = tmp.substr(beg, tmp.length());
								size = string(hostname).length();
								sub = buf.substr(0,size);
								res = sub.compare((string)hostname);

								if(!res)
								{
									//printf("%s\n", tmp.c_str());
									link_list.push_back(tmp);
								}
						
								cur_lvl=1;
								slash_cnt=0;
								buf="";
								sub="";
								buf_w="";
							}
							else
							{
								if(!buf_w.compare("www.")) beg=beg+4;
								
								buf = tmp.substr(beg, end-beg);
								sub = (string)hostname;
								size = buf.length();
								res = buf.compare(sub.substr(0,size));

								if(!res)
								{
									//printf("%s\n", tmp.c_str());							
									link_list.push_back(tmp);
								}
						
								cur_lvl=1;
								slash_cnt=0;
								buf="";
								sub="";
								buf_w="";
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

								//printf("%s\n", tmp.c_str());
								link_list.push_back(tmp);
							}
						}
						tmp="";
						slash_cnt=0;
						buf="";
						sub="";
						buf_w="";
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
		link_list.push_back((string)hostname);
	}
}

void parse_hostname(string addr, string &protocol, string &hostname, string &tail)
{
	string buf_h="";
	string buf_p="";
	string buf_t="";
	string buf_w="";
	string buf_wh="";

	int i, slash_count=0;
	
	if(addr.substr(0,7).compare("http://")!=0 && addr.substr(0,8).compare("https://")!=0 && addr.substr(0,6).compare("ftp://")!=0)
	{
		for(i=0; i<addr.length(); i++)
		{
			if(slash_count==0)
			{
				if(addr[i]=='/') slash_count++;
				else
				{
					buf_h.push_back(addr[i]);

					//if(buf_w.length()<4) buf_w.push_back(addr[i]);
					//else buf_wh.push_back(addr[i]); 
				}
			}
			else buf_t.push_back(addr[i]);
		}

		//if(!buf_w.compare("www.")) hostname = buf_wh;
		//else 
		hostname = buf_h;

		protocol = "";
		
		tail = buf_t;
	}
	else
	{
		for(i=0; i<addr.length(); i++)
		{
			if(slash_count<2)
			{
				buf_p.push_back(addr[i]);

				if(addr[i]=='/') slash_count++;
			}
			else if(slash_count==2)
			{	
				if(addr[i]!='/') 
				{
					//if(buf_w.length()<4) buf_w.push_back(addr[i]);
					//else buf_wh.push_back(addr[i]);

					buf_h.push_back(addr[i]);
				}

				else slash_count++;
			}
			else buf_t.push_back(addr[i]);
		}

		//if(!buf_w.compare("www.")) hostname = buf_wh;
		//else 
		hostname = buf_h;

		protocol = buf_p;
		
		tail = buf_t;
	}
}

void parse_error(string filename, string &error, int& err_num, string &location)
{
	string tmp = "";
	string buf = "";

	char current[50];

	bool end=false;
	bool loc=false;

	FILE *f;
	
	err_num=0;

	char *pos;
	
	char c;

	
	f = fopen(filename.c_str(), "r");

	fgets(current, sizeof(current), f);

	pos = strstr(current, " ")+1;

	int i=0;

	while(*(pos+i)!=' ')
	{
		err_num = err_num*10 + (*(pos+i)-'0');
		i++;
	}
		
	i++;

	while(!end)
	{
		while(*(pos+i)!='\n')
		{
			if(pos+i==current+50) break;

			error.push_back(*(pos+i));
			i++;
		}

		if((pos+i)!= current+50) end=true;
		else fgets(current, sizeof(current), f);
			
	}

	end=false;

	if(err_num!=301) location="";
	else
	{
		while(!end)
		{
			fgets(current, sizeof(current), f);

			if(strstr(current, "Location:")==current)
			{
				pos = current+10;

				int i=0;
				
				while(!end)
				{
					while(*(pos+i)!='\n')
					{
						if(pos+i==current+50) break;

						location.push_back(*(pos+i));
						i++;
					}

					if((pos+i)!= current+50) end=true;
					else fgets(current, sizeof(current), f);
				}

			}
		}
		
	}

	fclose(f);

	//printf("%d %s\n", err_num, error.c_str());

}

void parse_error_msg(char* message, string &error, int& err_num, string &location)
{
	string tmp = "";
	string buf = "";

	char current[50];


	bool end=false;
	bool loc=false;
	
	err_num=0;

	char *pos;
	
	char c;

	pos = strstr(message, " ")+1;

	int i=0;

	while(*(pos+i)!=' ')
	{
		err_num = err_num*10 + (*(pos+i)-'0');
		i++;
	}
		
	i++;


	while(*(pos+i)!='\n')
	{

		error.push_back(*(pos+i));
		i++;
	}
			

	if(err_num!=301) location="";
	else
	{
		while(!end)
		{
			//fgets(current, sizeof(current), f);

			pos = strstr(message, "Location:");
			
			pos = pos+10;

			int i=0;
				
			while(!end)
			{
				while(*(pos+i)!='\n')
				{
					//if(pos+i==current+50) break;

					location.push_back(*(pos+i));
					i++;
				}

				end=true;
			}

		}
	}
		


	//printf("%d %s\n", err_num, error.c_str());

}
