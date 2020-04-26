#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <iostream>

int main(int argc, char **argv)
{
    struct addrinfo hints;
    struct addrinfo * res;

    memset(&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;


    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);


    if(rc != 0)
    {
        std::cerr << "getaddrinfo: " << gai_strerror(rc) << std::endl;
        return -1;
    }

    int sd = socket (res->ai_family, res->ai_socktype, res->ai_protocol);
    
    char buffer[80]; 
    char host[NI_MAXHOST];
    char reloj[NI_MAXSERV];

    memset(buffer, 0, 80);

    getnameinfo(res->ai_addr, res->ai_addrlen, host, NI_MAXHOST, reloj, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

    //Lo mandamos
    sendto(sd, argv[3], strlen(argv[3]), 0, res->ai_addr, res->ai_addrlen);
    //Lo recibimos
    recvfrom(sd, buffer, 80, 0,res->ai_addr, &res->ai_addrlen);

    std::cout << buffer << std::endl;

    return 0;
}