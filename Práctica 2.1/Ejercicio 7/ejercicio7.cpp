#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <thread>
#include <vector>

class chat_Threads
{
    private:
        int _sd_client;
        int _thid;
    public:
        chat_Threads(int sd_client, int thid) : _sd_client(sd_client), _thid(thid) {};
        void haz_conexion()
        {
            // ---------------------------------------------------------------------- //
            // GESTION DE LA CONEXION CLIENTE //
            // ---------------------------------------------------------------------- //
            char buffer[80];
            while(strlen(buffer) > 2 || buffer[0] != 'q')
            {
                 memset(buffer, 0, 80);
                 
                ssize_t bytes = recv(_sd_client, (void *) buffer, sizeof(char)*79, 0);

                if ( bytes <= 0 )
                {
                    return;
                }

                std::cout << "[" << _thid << "] MENSAJE: " << buffer << std::endl;

                send(_sd_client, (void *) buffer, bytes, 0);
            }
            std::cout << "el cliente del thread [" << _thid << "] ha finalizado el chat" << std::endl;
            // FIN CONEXION
            close(_sd_client);
        };
};



int main(int argc, char **argv)
{
    struct addrinfo hints;
    struct addrinfo * res;
    // ---------------------------------------------------------------------- //
    // INICIALIZACIÓN SOCKET & BIND //
    // ---------------------------------------------------------------------- //

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

    if ( bind(sd, res->ai_addr, res->ai_addrlen) != 0 )
    {
        std::cerr << "bind: " << std::endl;
        return -1;
    }

    freeaddrinfo(res);

    // ---------------------------------------------------------------------- //
    // PUBLICAR EL SERVIDOR (LISTEN) //
    // ---------------------------------------------------------------------- //
    listen(sd, 16);

    // ---------------------------------------------------------------------- //
    // GESTION DE LAS CONEXIONES AL SERVIDOR //
    // ---------------------------------------------------------------------- //
    char host[NI_MAXHOST];
    char service[NI_MAXSERV];

    int th_id = 0;

    std::vector<std::thread> pool_thread;

    std::cout << "Server del chat inicializado" << std::endl;

    while(true)
    {
        struct sockaddr client_addr;
        socklen_t client_len = sizeof(struct sockaddr);

        // Thread ppal se bloquea en accept
        int sd_client = accept(sd, &client_addr, &client_len);

        getnameinfo(&client_addr, client_len, host, NI_MAXHOST, service,
        NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

        std::cout << "CONEXION DESDE IP: " << host << " PUERTO: " << service << std::endl;

        //Crear thread para tratar cada conexion
        chat_Threads* c_TH = new chat_Threads(sd_client, th_id++);
        
        std::thread([&c_TH](){
                c_TH->haz_conexion();
                delete c_TH;
                }).detach();
    }
    return 0;
}