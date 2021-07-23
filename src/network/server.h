#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#define NETWORK_SERVER_DEFAULT_PORT 22255

namespace network {
    struct server_client {
        uint32_t id;
        TCPsocket socket;
        IPaddress *IPadress;
    };

    class server {
        public:
            server();
            ~server();

            bool begin();

            void update();

            void close();
        private:
            TCPsocket p_server;
            IPaddress p_ipadress; 
            uint16_t p_port;

            uint32_t p_id_counter;
    };
};

#endif