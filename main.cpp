#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <netdb.h>

int main(int argc, char *argv[])
{
    struct sockaddr_in server;
    char* hostname = "www.google.com";
    char ip[100];
    struct hostent *he;
    struct in_addr **addr_list;
    char* message, server_reply[2000];

    int socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc == -1)
    {
        printf("could not create socket");

    }

    if ( (he = gethostbyname( hostname)) == NULL)
    {
        herror("gethostbyname");
        return 1;
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
        printf("cannot get ip from hostname");
        return 1;
	}

	puts(ip);
    server.sin_addr.s_addr = inet_addr(ip);
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

	//char arr[200];
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
	int size_recv , total_size= 0;
	char chunk[512];

	//loop
	while(1)
	{
		memset(chunk ,0 , 512);	//clear the variable
		if((size_recv =  recv(socket_desc , chunk , 512, 0) ) < 0)
		{
			break;
		}
		else
		{
			printf("%s" , chunk);
		}
	}


	close(socket_desc);
    return 0;
}
