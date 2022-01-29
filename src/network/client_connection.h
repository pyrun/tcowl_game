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

            uint8_t* recvData( uint16_t *length);
            void stream( uint8_t *data, uint32_t length);

            void update() override;

            void close() override;
        private:
            uint16_t p_port;
            std::string p_address;
            TCPsocket p_socket;
            SDLNet_SocketSet p_socketset;
            std::vector<synchronisation*> p_sync_objects;

            uint32_t p_input_length;
            uint8_t p_input_stream[NETWORK_INPUT_BUFFER];
    };
}

#endif