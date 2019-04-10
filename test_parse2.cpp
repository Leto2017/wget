#include <iostream>
#include <string>
#include <regex>
#include <vector>
#include <fstream>

using namespace std;

void parse_link(vector<string> &link_list, string string_to_parse)
{
	regex http_regex("<a.*=.*\"https{0,1}://.*(?=\"\>)");

	regex link_regex("https{0,1}://.*");

	smatch http_match;
	smatch link_match;

	string tmp;

	if (regex_search(string_to_parse, http_match, http_regex)) {

		for (size_t i = 0; i < http_match.size(); ++i)
		{
			tmp = http_match[i].str();
			if (regex_search(tmp, link_match, link_regex))
			{
				for (size_t j = 0; j < link_match.size(); ++j)
				{
					link_list.push_back(link_match[j]);
				}
			}


		}
	}


}


void parse_hostname(string addr, string &protocol, string &hostname, string &tail)
{
	regex prot_regex("https{0,1}");
	
	regex host_regex("://");
	regex host2_regex("/");

	string tmp;

	tmp = addr;

	smatch match;

	regex_search(tmp, match, prot_regex);

	protocol = match[0];

	tmp = match.suffix();

	regex_search(tmp, match, host_regex);

	tmp = match.suffix();

	regex_search(tmp, match, host2_regex);

	hostname = match.prefix();  //тут надо убирать www или нет?
	tail = match.suffix();
	
}




void parse_error(string filename, string &error, int& err_num, string &location)
{
	string current;
	string tmp;
	string buf;

	regex prot_regex("HTTPS{0,1}");
	regex sp_regex(" ");
	regex loc_regex("Location: ");

	smatch match;

	err_num = 0;

	ifstream file(filename);

	while (getline(file, current)) 
	{
		
		if (regex_search(current, match, prot_regex))
		{
			tmp = match.suffix();

			regex_search(tmp, match, sp_regex);
			
			tmp = match.suffix();

			regex_search(tmp, match, sp_regex);

			buf = match.prefix();

			err_num = atoi(buf.c_str());

			break;
		}
	}

	if (err_num != 301) location = "";
	else
	{
		while (getline(file, current))
		{

			if (regex_search(current, match, loc_regex))
			{
				location = match.suffix();
				break;
			}
		}
	}

	file.close();

}





int main()
{
	string lines[] = { "<a class=gb1 href=\"http://www.google.co.uz/imghp?hl=uz&tab=wi\">Rasmlar</a>",
		"<a class = gb1 href = \"https://mail.google.com/mail/?tab=wm\">Gmail< / a>",
		"<a class = gb1 href = \"https://drive.google.com/?tab=wo\">Drive< / a>",
		"< class = gb1 href = \"htt://www.google.com/calendar?tab=wc\">Taqvim< / a>" };

	string test_line = "<a class=gb1 href=\"http://www.google.co/\">Rasmlar</a>, next <a href=\"https://mail.ru\">Gmail< / a>";

	//find <a href = .... 

	regex http_regex("<a.*=.*\"https{0,1}://.*(?=\"\>)");

	regex link_regex("https{0,1}://.*");


	vector<string> link_list;
	string protocol, hostname, tail;

	for (const auto& line : lines) {
		parse_link(link_list, line);
	}

	parse_hostname(link_list[0], protocol, hostname, tail);

	for (size_t i = 0; i < link_list.size(); i++)
	{
		cout << i << ": " << link_list[i] << "\n\n";
	}

	for (size_t i = 0; i < link_list.size(); i++)
	{
		parse_hostname(link_list[i], protocol, hostname, tail);

		cout << i << " : " << protocol << " : " << hostname << " : " << tail << "\n\n";
	}


	string error;
	int err_num; 
	string location;

	parse_error("wwwunguz.html", error, err_num, location);

	cout << error << " " << err_num << " " << location << "\n";

	parse_error("in.txt", error, err_num, location);

	cout << error << " " << err_num << " " << location << "\n";
}