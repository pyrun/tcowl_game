#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <stdint.h>
#include <SDL2/SDL.h>
#include <enet/enet.h>

#define NETWORK_SERVER_DEFAULT_PORT 22255
#define NETWORK_PACKET_MAX_SIZE 1024
#define NETWORK_INPUT_BUFFER 1024

namespace network {
    struct client {
        uint32_t id;
        uint32_t index;
        uint32_t peerID;

        bool ready;
    };

    // 1...127 system
    // 128-255 user
    enum packet_type : uint32_t {
        network_type_heartbeat = 1
    };

    struct packet {
        uint32_t type; // packet_type
        uint8_t data[NETWORK_PACKET_MAX_SIZE];
        uint32_t length;
        uint8_t crc;
    };

    class connection {
        public:
            virtual bool begin() = 0;
            
            // Falls client == NULL wird es an alle versendet die verbunden sind
            virtual void sendPacket( packet packet, client *client) {}

            virtual void update() = 0;

            virtual void close() = 0;

            // client == null -> all clients
            void sendHeartbeat( client *client = nullptr);

            virtual bool isConnected() = 0;
    };

    class synchronisation {
        public:
            virtual void network_update( network::connection *network_interface) = 0;
            virtual bool newClientCallback( network::client *client, network::connection *network_interface) = 0;
            virtual void recvPacket( network::packet packet) = 0;
    };

    class service : public connection {
        public:
            virtual void addSync( synchronisation *sync) = 0;
            virtual bool delSync( synchronisation *sync) = 0;
    };

    uint8_t getCRC8( packet packet);
}

#endif