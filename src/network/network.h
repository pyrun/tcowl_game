#ifndef NETWORK_H
#define NETWORK_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_net.h>

#define NETWORK_SERVER_DEFAULT_PORT 22255
#define NETWORK_PACKET_MAX_SIZE 1024
#define NETWORK_INPUT_BUFFER 1024

namespace network {
    struct client {
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

            // Falls client == NULL wird es an alle versendet die verbunden sind
            virtual void sendPacket( packet packet, client *client) {}
    };

    class synchronisation {
        public:
            synchronisation() {}
            ~synchronisation() {}

            virtual bool newClientCallback( client *client, interface *network_interface) { return true; }
            virtual void recvPacket( packet packet) {}
    };

    uint8_t getCRC8( packet packet);
}

#endif