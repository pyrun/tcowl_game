#include "network.hpp"

using namespace network;

uint8_t network::getCRC8( network::packet packet) {
    uint8_t l_crc = 0;

    l_crc += packet.type;
    l_crc += packet.length;

    for( uint32_t i = 0; i < packet.length; i++)
        l_crc += packet.data[i];
    
    return l_crc;
}

void connection::sendHeartbeat( client *client) {
    packet l_packet = { .type = network::packet_type::network_type_heartbeat, .length = 0, .crc = network::getCRC8( l_packet)};
    sendPacket( l_packet, client);
}