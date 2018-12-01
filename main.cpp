#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <netdb.h>

int main(int argc, char *argv[])
{
    struct sockaddr_in server;
    char* hostname = "www.olx.uz";
    char ip[100];
    struct hostent *he;
    struct in_addr **addr_list;
    char* message, server_reply[2000];
    int i;

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

	for(i = 0; addr_list[i] != NULL; i++)
	{
		//Return the first one;
		strcpy(ip , inet_ntoa(*addr_list[i]) );
	}

	if (strlen(ip) == 0)
	{
        printf("cannot get ip from hostname");
        return 1;
	}

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

	char arr[200];
	message = "GET / HTTP/1.1\r\n\r\nUser-Agent: Mozilla/4.0 (compatible; MSIE5.01;)\r\n\r\nHost: ";
	strcat(arr, message);
	strcat(arr, hostname);
	strcat(arr, "\r\n\r\n");
	strcat(arr, "Accept: text/html, text/xml\r\n\r\nAccept-Language: en-us, ru-ru\r\n\r\nContent-Type: text/xml; charset=cp1251\r\n\r\nConnection: close");
	if (send(socket_desc, arr, strlen(arr), 0) < 0)
	{
        puts("send failed");
        return 1;
	}

	puts("data send");

	//-recv - calls are used to receive messages from a socket
	if (recv(socket_desc, server_reply, 2000, 0) < 0)
	{
        puts("recv failed");
	}

	puts("reply received");
	puts(server_reply);
	//close(socket_desc);
    return 0;
}
