#ifndef NETWORK_CLIENT_CONNECTION_H
#define NETWORK_CLIENT_CONNECTION_H

#include "network.h"

#include <string>
#include <vector>

namespace network {
    class client_connection : public service {
        public:
            client_connection();
            ~client_connection();
    
            bool begin() override;
            void sendPacket( packet packet, client *client) override; //interface

            void addSync( synchronisation *sync) override;
            bool delSync( synchronisation *sync) override;

            void stream( uint8_t *data, uint32_t length);

            void update() override;

            void close() override;

            bool isConnected() override { return p_peer != nullptr; }
        private:
            uint16_t p_port;
            std::string p_address;
            ENetHost *p_client;
            ENetPeer *p_peer;
            std::vector<synchronisation*> p_sync_objects;

            uint32_t p_input_length;
            uint8_t p_input_stream[NETWORK_INPUT_BUFFER];
    };
}

#endif