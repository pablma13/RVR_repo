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
        std::cerr << gai_strerror(rc) << std::endl;
        return -1;
    }
    
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];
    struct addrinfo *i;
    for (i = res; i != NULL; i = i->ai_next)
    {
        
       int nameInfo = getnameinfo(i->ai_addr, i->ai_addrlen,host,NI_MAXHOST,service,NI_MAXSERV,NI_NUMERICHOST | NI_NUMERICSERV);
       if (nameInfo != 0)
       {
           std::cerr << "ERROR!!" << std::endl;
       }
       else 
       { 
           std::cerr << host << "  " << i->ai_family << "  " << i->ai_socktype << std::endl;
       }
        
    }
    return 0;
}