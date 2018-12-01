#include <stdio.h>
#include <sys/socket.h>

int main(int argc, char *argv[])
{
    int socket_desc;
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_desc == -1)
    {
        printf("could not create socket");

    }

    printf("YES\n");
    printf("%d", socket_desc);

    return 0;
}
