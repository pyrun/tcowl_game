#ifndef NETWORK_SERVER_H
#define NETWORK_SERVER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <vector>

#include "network.h"

#define NETWORK_SERVER_MAX_CLIENTS 16
#define NETWORK_SERVER_PACKETSIZE 256

namespace network {
    class server : public interface {
        public:
            server();
            ~server();

            bool begin() override;
            void sendPacket( packet packet, client *client) override;

            void addSync( synchronisation *sync);
            bool delSync( synchronisation *sync);

            bool addClient( TCPsocket socket);
            void delClient( client *client_ptr);
            uint8_t* recvData( client *client_ptr, uint16_t* length);

            void update() override;

            void close() override;
        private:
            int32_t getFreeClientIndex();
            client *getClient( uint32_t index) { return p_clients[index]; }
        private:
            TCPsocket p_server_socket;
            IPaddress p_ipadress;
            SDLNet_SocketSet p_server_socketset;
            uint16_t p_port;

            uint32_t p_id_counter;
            
            client *p_clients[NETWORK_SERVER_MAX_CLIENTS];
            std::vector<synchronisation*> p_sync_objects;
    };
};

#endif