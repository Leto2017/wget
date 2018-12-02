#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>
#include <fstream>
#include <vector>
#include <cctype>
using namespace std;

void getHost_byName(char* hostname, char* ip);
int read(char* address, int socket_desc, char* hostname);
string getFileName(char* hostname);

int main(int argc, char *argv[])
{
    char* hostname = "www.google.com";

    //create socket
    int socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc == -1)
    {
        puts("could not create socket");

    }

    char* ip = new char;
    getHost_byName(hostname, ip);
    int success = read(ip, socket_desc, hostname);
    if (success != 0)
    {
        puts("Error");
    }

	close(socket_desc);
    return 0;
}

int read(char* address, int socket_desc, char* hostname)
{
    struct sockaddr_in server;

    server.sin_addr.s_addr = inet_addr(address);
	server.sin_family = AF_INET;
	server.sin_port = htons( 80 );
	//Connect to remote server
	if (connect(socket_desc , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		puts("connect error");
		return 1;
	}

	puts("Connected\n");
	puts(hostname);

	char* message;
	const char * format = "GET \/ HTTP\/1.1\r\nHost: %s\r\nUser-Agent: fetch.c\r\n\r\n";
	int status = asprintf(& message, format, hostname);
    if (status == -1)
    {
        printf("asprintf doesn't work");
        return 1;
    }
    //puts(message);
	if (send(socket_desc, message, strlen(message), 0) < 0)
	{
        puts("send failed");
        return 1;
	}

	puts("data send");

	//-recv - calls are used to receive messages from a socket
	int size_recv , total_size= 0, BLOCK_SIZE = 512;
	char chunk[BLOCK_SIZE];

    ofstream fout(getFileName(hostname));
	//loop
	while(1)
	{
		memset(chunk ,0 , BLOCK_SIZE);	//clear the variable
		if((size_recv = recv(socket_desc , chunk , BLOCK_SIZE, 0) ) < 0)
		{
			break;
		}
		else
		{
            fout << chunk;
			printf("%s" , chunk);
		}
	}

    fout.close();
    return 0;
}

string getFileName(char* hostname)
{
    string str = hostname;
	string str2="";

    for (const auto c: str)
    {

        if(!ispunct(c)){

            str2.push_back(c);
        }
    }

    char *outputFileName;
	const char* outputFileNameFormat = "%s.html";
	int status2 = asprintf(& outputFileName, outputFileNameFormat, str2.c_str());
	if (status2 == -1)
    {
        printf("asprintf doesn't work");
    }
    puts(outputFileName);
    string res(outputFileName);
    return res;
}

void getHost_byName(char* hostname, char *ip)
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

	for(int i = 0; addr_list[i] != NULL; i++)
	{
		//Return the first one;
		strcpy(ip , inet_ntoa(*addr_list[i]) );
	}

	if (strlen(ip) == 0)
	{
        puts("cannot get ip from hostname");
        return;
	}
}
