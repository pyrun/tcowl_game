#ifndef NETWORK_CLIENT_CONNECTION_H
#define NETWORK_CLIENT_CONNECTION_H

#include "network.h"

#include <string>
#include <vector>

namespace network {
    class client_connection : public interface {
        public:
            client_connection();
            ~client_connection();
    
            bool begin();
            void sendPacket( packet packet, client *client); //interface

            void addSync( synchronisation *sync);
            bool delSync( synchronisation *sync);

            uint8_t* recvData( uint16_t* length);
            void stream( uint8_t *data, uint32_t length);

            void update();

            void close();
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