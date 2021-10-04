#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
    int sock_fd;
    int conn_fd;

    struct sockaddr saddr = {AF_LOCAL, "incr-num-serv"};
    socklen_t saddrlen = sizeof(struct sockaddr) + 6;

    sock_fd = socket(AF_LOCAL, SOCK_STREAM, 0);
    conn_fd = connect(sock_fd, &saddr, saddrlen);

    char req[4];
    int BUFF[1];
    BUFF[0] = -1;
    sprintf(req, "%d", 100);

    write(sock_fd, req, 3);
    read(sock_fd, BUFF, 4);

    int val = BUFF[0];
    if(val != -1)
    {
        printf("Got back %d\n", val);
    }

    close(sock_fd);
    close(conn_fd);

    return 0;
}