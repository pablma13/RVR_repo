#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include<iostream>



int main(int argc, char **argv)
{
    struct addrinfo hints;
    struct addrinfo *res;

    //INICIALIZACIÓN SOCKET & BIND

    memset (&hints, 0, sizeof(struct addrinfo));

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;

    int rc =  getaddrinfo(argv[1], argv[2], &hints, &res);

    if(rc != 0)
    {
        std::cerr << "getaddrinfo: " << gai_strerror(rc) << std::endl;
        return -1; 
    }

    int sd = socket (res->ai_family, res->ai_socktype, res->ai_protocol);


    if(bind (sd, res->ai_addr, res->ai_addrlen) != 0)
    {
        std::cerr << "bind: " << std::endl;
        return -1;
    }

    freeaddrinfo(res);

    char buffer[80];
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    struct sockaddr client_addr;
    socklen_t client_len = sizeof(struct sockaddr);

    while(buffer[0] != 'q')
    {
        ssize_t bytes = recvfrom(sd, buffer, 79*sizeof(char), 0, &client_addr, &client_len);

        if(bytes == -1)
        {
            std::cerr << "recvfrom: " << std::endl;
            return -1;
        }

        getnameinfo(&client_addr, client_len, host, NI_MAXHOST, service, NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        std::cout << (int)bytes << " bytes de " << host << ":" << service << std::endl;

        //RESPUESTAS AL CLIENTE

        char i [80];
        time_t rawtime;
        struct tm* timeinfo;
        timeinfo = localtime(&rawtime);
        switch(buffer[0])
        {
            case 't':
                strftime(i, 80, "%r", timeinfo);
                sendto(sd, i, strlen(i), 0, &client_addr, client_len);
                break;
            case 'd':
                strftime(i, 80, "%F", timeinfo);
                sendto(sd, i, strlen(i), 0, &client_addr, client_len);
                break;
            case 'q':
                break;
            default:
                std::cout << "Comando no soportado " << buffer[0] << std::endl;
                break;
        }
    }
    std::cout << "Saliendo... " << std::endl;
    return 0;
}