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
    hints.ai_socktype = SOCK_STREAM;

    int rc = getaddrinfo(argv[1], argv[2], &hints, &res);

    if ( rc != 0 )
    {
        std::cerr << "getaddrinfo: " << gai_strerror(rc) << std::endl;
        return -1;
    }

    // res contiene la representación como sockaddr de dirección + puerto

    int sd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);

    connect(sd, res->ai_addr, res->ai_addrlen);

    freeaddrinfo(res);
    
    std::cout << "Pulse q desde para cerrar el servidor"<< std::endl;
    std::string buffer;

    while(buffer[0] != 'q')
    {
        char mensaje[80];
        memset(mensaje, 0, 80);

        std::getline(std::cin, buffer);

        send(sd, (void *) buffer.c_str(), sizeof(std::string)*79, 0);

        ssize_t bytes = recv(sd, (void *) mensaje, sizeof(std::char)*79, 0);

        mensaje[bytes] = '\0';

        std::cout << mensaje << std::endl;
    }    

    std::cout << "Servidor cerrado"<< std::endl;

    return 0;
}