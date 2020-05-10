#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>

#include <iostream>
#include <thread>
#include <vector>

bool working = true;

const int NUMERO_THREADS = 5; 

class clock_Threads
{
    private:
        int _id;
        int _sd;
    public:
    clock_Threads(int sd, int id) : _sd(sd), _id(id) {};
        void haz_mensaje()
        {
            // ---------------------------------------------------------------------- //
            // RECEPCIÓN MENSAJE DE CLIENTE //
            // ---------------------------------------------------------------------- //
            char buffer[80];
            char host[NI_MAXHOST];
            char service[NI_MAXSERV];

            struct sockaddr client_addr;
            socklen_t client_len = sizeof(struct sockaddr);
            while(working)
            {
                ssize_t bytes = recvfrom(_sd, buffer, 79 * sizeof(char), 0, &client_addr, &client_len);
                if ( bytes == -1)
                {
                    std::cerr << "recvfrom: " << std::endl;
                    return;
                }
                getnameinfo(&client_addr, client_len, host, NI_MAXHOST, service,
                NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV);

                std::cout << "THREAD: " << std::this_thread::get_id() << " IP: " << host << " PUERTO: " << service
                << " MENSAJE: " << buffer << std::endl;

                sleep(3);
                // ---------------------------------------------------------------------- //
                // RESPUESTA AL CLIENTE //
                // ---------------------------------------------------------------------- //
                char i [80];
                time_t rawtime;
                struct tm* timeinfo;
                int size;
                time (&rawtime);
                timeinfo = localtime(&rawtime);
                switch(buffer[0])
                {
                    case 't':
                        size = strftime(i, 80, "%r", timeinfo);
                        sendto(_sd, i, size, 0, &client_addr, client_len);
                        break;
                    case 'd':
                        size = strftime(i, 80, "%F", timeinfo);
                        sendto(_sd, i, size, 0, &client_addr, client_len);
                        break;
                    case 'q':
                        if(strlen(buffer) > 2) working = false;
                        break;
                    default:
                        std::cout << "Comando no soportado " << buffer[0] << std::endl;
                        break;
                }
            }
            close(_sd);
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
    hints.ai_socktype = SOCK_DGRAM;

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

    // -------------------------------------------------------------------------
    // POOL DE THREADS
    // -------------------------------------------------------------------------
    std::vector<std::thread> pool;

    std::cout << "Servidor de " << NUMERO_THREADS << " hilos iniciado" << std::endl;

    for (int i = 0; i < NUMERO_THREADS; ++i)
    {
        clock_Threads* cTh = new clock_Threads(sd, i);
        pool.push_back(std::thread([&cTh](){
                cTh->haz_mensaje();
                delete cTh;
                }));
    }

    for(auto &t : pool)
    {
        t.detach();
    }

    while(working){}

    std::cout << "Servidor cerrado" << std::endl;

    return 0;
}