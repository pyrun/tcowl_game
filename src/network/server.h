#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>
#include <vector>

#define NETWORK_SERVER_DEFAULT_PORT 22255
#define NETWORK_SERVER_MAX_CLIENTS 16
#define NETWORK_SERVER_PACKETSIZE 256
#define NETWORK_PACKET_MAX_SIZE 32

namespace network {
    struct server_client {
        uint32_t id;
        uint32_t index;
        TCPsocket socket;
        IPaddress *IPadress;
    };

    enum packet_type {
        network_type_object_data = 0,
    };

    struct packet {
        packet_type type;
        uint8_t data[NETWORK_PACKET_MAX_SIZE];
        uint32_t length;
        uint8_t crc;
    };

    class interface {
        public:
            interface() {}
            ~interface() {}

            void sendPacket( packet packet);
    };

    class synchronisation {
        public:
            synchronisation() {}
            ~synchronisation() {}

            bool newClientCallback( server_client *client, interface *network_interface);
            void recvPacket( packet packet);
    };

    class server : public interface {
        public:
            server();
            ~server();

            bool begin();

            void addSync( synchronisation *sync);
            bool delSync( synchronisation *sync);
            
            bool addClient( TCPsocket socket);
            void delClient( server_client *client);
            uint8_t* recvData( server_client *client, uint16_t* length);

            void update();

            void close();
        private:
            int32_t getFreeClientIndex();
            server_client *getClient( uint32_t index) { return p_clients[index]; }
        private:
            TCPsocket p_server_socket;
            IPaddress p_ipadress;
            SDLNet_SocketSet p_server_socketset;
            uint16_t p_port;

            uint32_t p_id_counter;
            
            server_client *p_clients[NETWORK_SERVER_MAX_CLIENTS];
            std::vector<synchronisation*> p_sync_objects;
    };
};

#endif