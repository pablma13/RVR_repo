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
    char buffer[80];
    char mensaje[80];
    
    while(strlen(buffer) > 2 || buffer[0] != 'q')
    {
        memset(mensaje, 0, 80);
        memset(buffer, 0, 80);

        fgets(buffer, sizeof(buffer)-1, stdin);

        send(sd, buffer, sizeof(buffer), 0);

        ssize_t bytes = recv(sd, mensaje, sizeof(mensaje), 0);

        std::cout << "mensaje: " << mensaje << std::endl;

        mensaje[bytes] = '\0';
    }    

    std::cout << "Servidor cerrado"<< std::endl;

    return 0;
}