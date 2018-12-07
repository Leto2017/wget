#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <fstream>
#include <vector>
#include <cctype>
#include <iostream>
#include <regex>
#include <stdlib.h>
#include <getopt.h>
#include <algorithm>
#include <sys/stat.h>
#include <sys/types.h>

#include "parser.cpp"

using namespace std;

void getHost_byName(char* hostname, string& ip);
int read(char* address, int socket_desc, char* hostname);
string getFileName(char* hostname);
int process(char *hostname, int socket_desc, int level);
int readSubLinks(int level, char* hostname);

string hm;
string protocol;
string tail;

vector<int> socketList;

static void show_usage(std::string name)
{
    std::cerr << "Usage: <option(s)> SOURCES"
              << "Options:\n"
              << "\t-u --url=<string> = the http-address to download web-page\n"
              << "\t-r --recursive       = load pages by links found in the download pages\n"
              << "\t-l --level=<uint> = глубина рекурсивного скачивания сайта по ссылкам на скачанных страницах\n"
              << "\t-t --tries=<uint> = число попыток скачать страницу до выдачи ошибки\n"
              << "\t-n --no-parent    = загружать страницы не выше по иерархии заданной\n"
              << "\t-i --infile=<path> = путь к входному txt файлу со списком http-ссылок\n"
              << "\t-s --savedir=<path> = путь до папки, где сохранять html-страницы\n"
              << "\t-v --verbose = печатать в stdout подробно производимые операции, без этого флага печатать только ошибки\n"
              << "\t-h --help =  показать как использовать программу, выдать аргументы консоли\n"
              << std::endl;
}
static const char *optString = "hu:rl:t:n:i:s:v?";
static const struct option longOpts[] = {
    { "help", no_argument, NULL, 'h' },
    { "url", required_argument, NULL, 'u' },
    { "level", required_argument, NULL, 'l' },
    { "tries", required_argument, NULL, 't' },
    { "infile", required_argument, NULL, 'i' },
    { "savedir", required_argument, NULL, 's' },
    { "verbose", no_argument, NULL, 'v' },
    { "recursive", no_argument, NULL, 'r' },
    { "no-parent", no_argument, NULL, 'n' },
    { NULL, no_argument, NULL, 0 }
    };

static struct globalArgs_t
{
    char* url;
    bool recursive;
    bool noparent;
    bool verbosity;
    int level;
    int tries;
    std::string filename;
    std::string savedir;
} globalArgs;


static struct returnCodeStruct
{
    std::string error;
    std::string location;
    int code;
} returnCode;


int main(int argc, char **argv)
{
    if (argc < 2) {
        show_usage(argv[0]);
        return 1;
    }

    returnCode.code = 0;
    globalArgs.recursive = false;
    globalArgs.noparent = false;
    globalArgs.verbosity = false;
    globalArgs.level = 1;
    globalArgs.tries = 1;

    if (globalArgs.verbosity)
    {
        puts("Start");
    }

    int opt = 0;

    int longIndex;
    opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
    while( opt != -1 ) {
        switch( opt ) {
            case 'h':
            case '?':
                show_usage(argv[0]);
                return 0;

            case 'u':
                globalArgs.url = optarg;
                break;

            case 'l':
                globalArgs.level = atoi(optarg);
                break;

            case 't':
                globalArgs.tries = atoi(optarg);
                break;

            case 'i':
                globalArgs.filename = static_cast<std::string>(optarg);
                break;

            case 's':
                globalArgs.savedir = static_cast<std::string>(optarg);
                break;

            case 'n':
                globalArgs.noparent = true;
                break;

            case 'r':
                globalArgs.recursive = true;
                break;

            case 'v':
                globalArgs.verbosity = true;
                break;

            default:
                break;
        }

        opt = getopt_long( argc, argv, optString, longOpts, &longIndex );
    }

    if (globalArgs.verbosity)
    {
        puts("Create socket");
    }
    //create socket
    int socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    socketList.push_back(socket_desc);

    if (socket_desc == -1)
    {
        puts("could not create socket");
        return 1;
    }

    //if (!globalArgs.filename.empty())
    //{
    //    puts("Reading urls from file");
    //    std::string s;
    //    ifstream file(globalArgs.filename);/

    //    while(getline(file, s)){
    //        cout << s << endl;
    //        process(const_cast<char*>(s.c_str()), socket_desc);
    //    }

    //    file.close();
    //}

    if (globalArgs.verbosity)
    {
        puts("Process");
    }
    int success = process(globalArgs.url, socket_desc, globalArgs.level);
    if (success != 0)
    {
        puts("Can't process");
        return 1;
    }

    if (globalArgs.verbosity)
    {
        puts("Close socket");
    }
    if (!socketList.empty())
    {
	for (int j : socketList)
	{
		close(j);
	}
    }
    return 0;
}

int process(char *hostname, int socket_desc, int level)
{
    string ip;

    if (globalArgs.verbosity)
    {
        puts("Parse url");
    }
    parse_hostname(string(hostname), protocol, hm, tail);
    if (!protocol.empty() && hm.empty())
    {
        hm = protocol;
        protocol.clear();
    }
    //puts(const_cast<char*>(hm.c_str()));
    //puts(const_cast<char*>(protocol.c_str()));
    //puts(const_cast<char*>(tail.c_str()));

    //puts(hostname);

    if (globalArgs.verbosity)
    {
        puts("Get host by name");
    }

    int tries = globalArgs.tries;

    getHost_byName(const_cast<char*>(hm.c_str()), ip);

    if (globalArgs.verbosity)
    {
        cout << "IP: ";
        cout << ip << endl;
    }

    if (globalArgs.verbosity)
    {
        puts("Read");
    }

    int success = read(const_cast<char*>(ip.c_str()), socket_desc, hostname);
    if (success == 1)
    {
        if (returnCode.code > 0 && (tries > 0))
        {
            socket_desc = socket(AF_INET, SOCK_STREAM, 0);
            socketList.push_back(socket_desc);


            if (socket_desc == -1)
            {
                puts("could not create socket");
                return 1;
            }
            //globalArgs.tries -= 1;
            if (globalArgs.verbosity)
            {
                cout << "New location "<< returnCode.location <<endl ;
                cout << "Attempt number: " << tries << endl;
            }
            tries -= 1;
            process(const_cast<char*>(returnCode.location.c_str()), socket_desc, level);
        }
        //puts("Cann't read");
        return 1;
    }
    else if (success == 2)
    {
        if (level - 1 != 0 )
        {
            int ok = readSubLinks(level-1, hostname);
        }
    }
    else if (success == 3)
    {
        if (hostname == globalArgs.url)
            return 1;
    }
    hm.clear();
    protocol.clear();
    tail.clear();
    return 0;
}

int readSubLinks(int level, char* hostname)
{
    cout << "sublinks" << endl;
     vector<string> linkList;
     parse_html(linkList, getFileName(hostname), const_cast<char*>(hm.c_str()), globalArgs.noparent, level+1);

     cout << "size "<<linkList.size() << endl;
     if (!linkList.empty())
     {
         for(string i : linkList)
         {
                int socket_desc = socket(AF_INET, SOCK_STREAM, 0);
                socketList.push_back(socket_desc);


                if (socket_desc == -1)
                {
                    puts("could not create socket");
                    return 1;
                }
                if (globalArgs.verbosity)
                {
                    cout << "Link to process: "<< i <<endl ;
                }
                process(const_cast<char*>(const_cast<char*> (i.c_str())), socket_desc, level);
          }
      }

      return 0;
}

int read(char* address, int socket_desc, char* hostname)
{
    struct sockaddr_in server;

    server.sin_addr.s_addr = inet_addr(address);
	server.sin_family = AF_INET;
	server.sin_port = htons( 80 );
	//Connect to remote server
	if (globalArgs.verbosity)
    {
        puts("Connect to remote server");
    }
	if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		puts("connect error");
		return 1;
	}

	if (globalArgs.verbosity)
    {
        puts("Connected\n");
//        puts(hostname);
    }

	char* message;
	const char * format = "GET %s HTTP/1.1\r\nHost: %s\r\nUser-Agent: Mozilla/5.0 (X11; U; Linux i686; ru; rv:1.9b5) Gecko/2008050509 Firefox/3.0b5\r\nAccept: text/html\r\nConnection: close\r\n\r\n";
	if (tail.empty())
        tail = "/";

    int status = asprintf(& message, format, tail.c_str(), hm.c_str());

	if (status == -1)
    {
        printf("asprintf doesn't work");
        return 1;
    }

    if (globalArgs.verbosity)
    {
        puts("Send message:");
        puts(message);
    }
    if (send(socket_desc, message, strlen(message), 0) < 0)
	{
        puts("send failed");
        return 1;
	}

	//-recv - calls are used to receive messages from a socket
	int size_recv , total_size= 0, BLOCK_SIZE = 512;
	char chunk[BLOCK_SIZE];
    if (globalArgs.verbosity)
    {
        puts("Receiving answer ... ");
    }
    if (!globalArgs.savedir.empty())
    {
        const int dir=mkdir(const_cast<char*>(globalArgs.savedir.c_str()),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    }
    string filename = getFileName(hostname);
    ofstream fout(filename);

    //loop
	while(1)
	{
		memset(chunk ,0 , BLOCK_SIZE);	//clear the variable
		if((size_recv = recv(socket_desc , chunk , BLOCK_SIZE, 0) ) == 0)
		{
			break;
		}
		else
		{
            fout << chunk;
            //if (globalArgs.verbosity)
            //    printf("%s" , chunk);
		}
	}
    fout.close();

    if (globalArgs.verbosity)
        puts("Analize response");

    parse_error(filename, returnCode.error, returnCode.code, returnCode.location);

    if (globalArgs.verbosity)
        cout << "CODE: "<<returnCode.code << endl;

    if (returnCode.code >= 500 && returnCode.code < 600)
    {
        if (globalArgs.verbosity)
            cout << "Status code: " << returnCode.code << endl;
        return 3;
    }
    if (returnCode.code >=300 && returnCode.code < 400)
    {
        if (globalArgs.verbosity)
            cout << "Status code: " << returnCode.code << endl;
        return 1;
    }
    if (returnCode.code == 200 && globalArgs.recursive && globalArgs.level > 1)
    {
        if (globalArgs.verbosity)
            cout << "Status code: " << returnCode.code << " recursive with level "<< globalArgs.level <<endl;
        return 2;
    }

    return 0;
}

string getFileName(char* hostname)
{
    if (globalArgs.verbosity)
    {
        puts("Get filename");
    }

    std::string str = hostname;
    str.erase(std::remove_if(str.begin(), str.end(), ::ispunct), str.end());
    if (!globalArgs.savedir.empty())
    {
        str = globalArgs.savedir + "/" + str;
    }

    str += ".html";
    if (globalArgs.verbosity)
        cout << "Filename: "<< str << endl;
    return str;
}

void getHost_byName(char* hostname, string &ip)
{
    struct hostent *he;
    struct in_addr **addr_list;

    if ( (he = gethostbyname( hostname)) == NULL)
    {
        herror("gethostbyname");
        return;
    }

    //Cast the h_addr_list to in_addr , since h_addr_list
    //also has the ip address in long format only
	addr_list = (struct in_addr **) he->h_addr_list;

	//for(int i = 0; addr_list[i] != NULL; i++)
	//{
		ip += string(inet_ntoa(*addr_list[0])) ;
	//}

	if (ip.empty())
	{
        puts("Cannot get ip from hostname");
        return;
	}
}


