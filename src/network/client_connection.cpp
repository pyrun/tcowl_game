#include "client_connection.hpp"

#include <stdio.h>
#include <string.h>

#include <engine/log.hpp>

using namespace network;

client_connection::client_connection() {
    p_port = NETWORK_SERVER_DEFAULT_PORT;
    p_address = "localhost";
    p_input_length = 0;
}

client_connection::~client_connection() {

}

bool client_connection::begin() {
    ENetAddress l_address;
    ENetEvent l_event;

    if (enet_initialize() != 0) {
        engine::log( engine::log_error, "client_connection::begin An error occured while initializing ENet");
        return false;
    }

    p_client = enet_host_create( NULL, 1, 2, 0, 0);
    if( p_client == nullptr) {
        engine::log( engine::log_error, "client_connection::begin An error occurred while trying to create an ENet client host.");
        return false;
    }

    enet_address_set_host( &l_address, p_address.c_str());
    l_address.port = p_port;

    p_peer = enet_host_connect( p_client, &l_address, 2, 0);
    if( p_peer == nullptr) {
        engine::log( engine::log_error, "client_connection::begin An error occurred while trying to connect host %s.", p_address.c_str());
        return false;
    }

    if (enet_host_service (p_client, &l_event, 5000) > 0 &&
        l_event.type == ENET_EVENT_TYPE_CONNECT)
    {
        engine::log( engine::log_debug, "client_connection::begin connect was succeeded");
    } else  {
        engine::log( engine::log_error, "client_connection::begin Connection was not possible");
        enet_peer_reset( p_peer);
        p_peer = nullptr;
        return false;
    }
    return true;
}

void client_connection::sendPacket( packet packet, client *client) {
    uint8_t l_temp_data[NETWORK_PACKET_MAX_SIZE];
    uint32_t l_offset = 2;

    l_temp_data[0] = packet.type;
    l_temp_data[1] = packet.length;
    memcpy( l_temp_data + l_offset, packet.data, packet.length);
    l_offset += packet.length;

    l_temp_data[l_offset] = getCRC8( packet);
    l_offset++;

    ENetPacket *l_packet = enet_packet_create( l_temp_data, l_offset, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send ( p_peer, 0, l_packet);
}

void client_connection::addSync( synchronisation *sync) {
    p_sync_objects.push_back( sync);
}

bool client_connection::delSync( synchronisation *sync) {
    for( uint32_t i = 0; i < p_sync_objects.size(); i++) {
        if( sync == p_sync_objects[i]) {
            p_sync_objects.erase( p_sync_objects.begin()+i);
            return true;
        }
    }
    return false;
}

void client_connection::update() {
    ENetEvent l_event;
    while (enet_host_service (p_client, &l_event, 0) > 0) {
        switch( l_event.type) {
            case ENET_EVENT_TYPE_CONNECT: {
                engine::log( engine::log_warn, "client_connection::update unexpected new connection");
            } break;

            case ENET_EVENT_TYPE_DISCONNECT: {
                engine::log( engine::log_warn, "client_connection::update connection disconnected");
                enet_peer_reset ( p_peer);
                p_peer = nullptr;
            } break;

            case ENET_EVENT_TYPE_RECEIVE: {
                // recv routine
                uint16_t l_length, flag;
                l_length = l_event.packet->dataLength;

                if( l_length < 2) {
                    enet_packet_destroy( l_event.packet);
                    engine::log( engine::log_debug, "client_connection::update wrong length");
                    continue;
                }

                packet l_packet;
                // get type and length
                l_packet.type = (network::packet_type)l_event.packet->data[0];
                l_packet.length = l_event.packet->data[1];

                // check if valid
                if( l_length != l_packet.length + 3) {
                    enet_packet_destroy( l_event.packet);
                    engine::log( engine::log_debug, "client_connection::update packet was faulty");
                    continue;
                }

                // copy data
                memcpy( l_packet.data, l_event.packet->data+2, l_packet.length);
                l_packet.crc = l_event.packet->data[ l_packet.length+2];
                enet_packet_destroy( l_event.packet);

                // check CRC
                if( l_packet.crc != getCRC8( l_packet))
                    continue;
                if( l_packet.type == network_type_heartbeat) {
                    sendHeartbeat();
                    continue;
                }

                // pocess packet
                for( uint32_t i = 0; i < p_sync_objects.size(); i++) {
                    network::synchronisation *l_sync = p_sync_objects[i];
                    l_sync->recvPacket( l_packet);
                }
            } break;

            default:
                engine::log( engine::log_debug, "client_connection::update unknow message %d", l_event.type);
            break;
        }
    }
}

void client_connection::close() {
    if( p_client) {
        if( p_peer) {
            enet_peer_disconnect( p_peer, 0);

            ENetEvent l_event;
            bool l_quit = false;
            while( l_quit == false && enet_host_service (p_client, &l_event, 3000) > 0) {
                switch (l_event.type) {
                    case ENET_EVENT_TYPE_RECEIVE: {
                        enet_packet_destroy (l_event.packet);
                    } break;
                    case ENET_EVENT_TYPE_DISCONNECT: {
                        engine::log( engine::log_debug, "client_connection::close disconnection succeeded.");
                        l_quit = true;
                    } break;
                    default:
                    break;
                }
            }
            // falls wir keine antwort erhalten haben -> zwanghaft
            if( l_quit == false)
                enet_peer_reset( p_peer);
        }
        enet_host_destroy(p_client);
    }
}