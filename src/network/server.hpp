#ifndef NETWORK_SERVER_H
#define NETWORK_SERVER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <vector>

#include "network.hpp"

#define NETWORK_SERVER_MAX_CLIENTS 16
#define NETWORK_SERVER_PACKETSIZE 256

namespace network {
    class server : public service {
        public:
            server();
            ~server();

            bool begin() override;
            void sendPacket( packet packet, client *client) override;

            void addSync( synchronisation *sync) override;
            bool delSync( synchronisation *sync) override;

            bool addClient( ENetPeer *peer);
            void delClient( client *client_ptr);
            void update() override;

            void close() override;

            bool isConnected() override { return true; } // server is always online
        private:
            int32_t getFreeClientIndex();
            client *getClient( uint32_t index) { return p_clients[index]; }
        private:
            ENetAddress p_address;
            ENetHost* p_server;
            uint16_t p_port;

            uint32_t p_id_counter;
            
            client *p_clients[NETWORK_SERVER_MAX_CLIENTS];
            std::vector<synchronisation*> p_sync_objects;
    };
};

#endif