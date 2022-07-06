#include "server.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>

#include "../engine/log.h"

using namespace network;

server::server() {
    p_port = NETWORK_SERVER_DEFAULT_PORT;
    p_id_counter = 0;
    for( uint32_t i = 0; i < NETWORK_SERVER_MAX_CLIENTS; i++)
        p_clients[i] = nullptr;
    p_server = nullptr;
}

server::~server() {
    close();
}

bool server::begin() {
    if (enet_initialize() != 0) {
        engine::log( engine::log_error, "server::begin An error occured while initializing ENet");
        return false;
    }

    engine::log( engine::log_debug, "server::begin starte Server auf Port %d", p_port);

    p_address.host = ENET_HOST_ANY;
    p_address.port = p_port;

    p_server = enet_host_create ( &p_address, 32, 2, 0, 0);

    if(p_server == nullptr) {
        engine::log( engine::log_error, "server::begin enet_host_create failed");
        return false;
    }
    return true;
}

void server::sendPacket( packet packet, client *client) {
    uint8_t l_temp_data[NETWORK_SERVER_PACKETSIZE];
    uint32_t l_offset = 2;

    l_temp_data[0] = packet.type;
    l_temp_data[1] = packet.length;
    memcpy( l_temp_data + l_offset, packet.data, packet.length);
    l_offset += packet.length;

    l_temp_data[l_offset] = getCRC8( packet);
    l_offset++;

    // Termination
    l_temp_data[l_offset>=NETWORK_SERVER_PACKETSIZE?NETWORK_SERVER_PACKETSIZE-1:l_offset] = 0;
    if( client == nullptr) {
        bool l_clients = false;
        for( uint32_t i = 0; i < NETWORK_SERVER_MAX_CLIENTS; i++) {
            if( p_clients[i] != nullptr) {
                l_clients = true;
                p_clients[i]->ready = false;
            }
        }

        if( l_clients == false)
            return;

        ENetPacket *l_packet = enet_packet_create( l_temp_data, l_offset, ENET_PACKET_FLAG_RELIABLE);
        enet_host_broadcast( p_server, 0, l_packet);
    } else {
        if( client->peerID >= p_server->peerCount) {
            delClient( client);
            return;
        }
        
        ENetPacket *l_packet = enet_packet_create( l_temp_data, l_offset, ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send ( &p_server->peers[client->peerID], 0, l_packet);
        client->ready = false;
    }
}

void server::addSync( synchronisation *sync) {
    p_sync_objects.push_back( sync);
}

bool server::delSync( synchronisation *sync) {
    for( uint32_t i = 0; i < p_sync_objects.size(); i++) {
        if( sync == p_sync_objects[i]) {
            p_sync_objects.erase( p_sync_objects.begin()+i);
            return true;
        }
    }
    return false;
}

bool server::addClient( ENetPeer *peer) {
    uint32_t ipaddr;
    int32_t l_index = getFreeClientIndex();
    client *l_client = nullptr;

    if( l_index == -1) {
        engine::log( engine::log_warn, "server::addClient no space for new connection");
        return false;
    }

    // set up the client
    p_clients[ l_index] = new client;
    l_client = getClient( l_index);
    l_client->peerID = peer->incomingPeerID;
    l_client->index = l_index;
    l_client->id = p_id_counter++;
    l_client->ready = true;

    // print out the clients IP and port number 
    engine::log( engine::log_debug, "server::addClient Accepted a connection with id %d from %d.%d.%d.%d port %hu", l_client->id,
        peer->address.host&0xff, (peer->address.host>>8)&0xff, (peer->address.host>>16)&0xff, peer->address.host>>24,
        peer->address.port); 

    for( uint32_t i = 0; i < p_sync_objects.size(); i++) {
        network::synchronisation *l_sync = p_sync_objects[i];
        l_sync->newClientCallback( l_client, this);
    }
    
    // heatbeat start
    sendHeartbeat( l_client);
    return true;
}

void server::delClient( client *client_ptr) {
    engine::log( engine::log_debug, "server::delClient close connection %d", client_ptr->id);
    uint32_t l_index = client_ptr->index;
    free( client_ptr);
    p_clients[ l_index] = NULL;
}

void server::update() {
    ENetEvent l_event;
    while (enet_host_service (p_server, & l_event, 0) > 0) {
        switch( l_event.type) {
            case ENET_EVENT_TYPE_CONNECT: {
                addClient( l_event.peer);
            } break;

            case ENET_EVENT_TYPE_DISCONNECT: {
                for( uint32_t i = 0; i < NETWORK_SERVER_MAX_CLIENTS; i++)
                    if( getClient( i) != nullptr && getClient( i)->peerID == l_event.peer->incomingPeerID)
                        delClient(getClient( i));
            } break;

            case ENET_EVENT_TYPE_RECEIVE: {
                // recv routine
                uint16_t l_length, flag;
                l_length = l_event.packet->dataLength;

                if( l_length < 2) {
                    enet_packet_destroy( l_event.packet);
                    engine::log( engine::log_debug, "server::update wrong length");
                    continue;
                }

                packet l_packet;
                // get type and length
                l_packet.type = (network::packet_type)l_event.packet->data[0];
                l_packet.length = l_event.packet->data[1];

                // check if valid
                if( l_length != l_packet.length + 3) {
                    enet_packet_destroy( l_event.packet);
                    engine::log( engine::log_debug, "server::update packet was faulty");
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
                    for( uint32_t i = 0; i < NETWORK_SERVER_MAX_CLIENTS; i++)
                        if( getClient( i) != nullptr && getClient( i)->peerID == l_event.peer->incomingPeerID)
                            getClient( i)->ready = true;
                    continue;
                }

                // pocess packet
                for( uint32_t i = 0; i < p_sync_objects.size(); i++) {
                    network::synchronisation *l_sync = p_sync_objects[i];
                    l_sync->recvPacket( l_packet);
                }

            } break;

            default:
                engine::log( engine::log_debug, "server::update unknow message %d", l_event.type);
            break;
        }
    }

    // network update
    bool l_ready = true, l_heartbeat = false;
    for( uint32_t i = 0; i < NETWORK_SERVER_MAX_CLIENTS; i++) {
        if( getClient( i) != NULL &&
            getClient( i)->ready == false)
            l_ready = false;
    }
    for( uint32_t i = 0; (i < p_sync_objects.size()) && l_ready; i++) {
        network::synchronisation *l_sync = p_sync_objects[i];
        l_sync->network_update( this);
        l_heartbeat = true;
    }
    
    // heatbeat
    if( l_heartbeat)
        sendHeartbeat();
}

void server::close() {
    if( p_server) {
        for( uint32_t i = 0; i < NETWORK_SERVER_MAX_CLIENTS; i++) {
            client *l_client = getClient(i);
            if( l_client == nullptr)
                continue;
            
            if( l_client->peerID >= p_server->peerCount) {
                delClient( l_client);
                return;
            }

            enet_peer_disconnect( &p_server->peers[l_client->peerID], 0);
        }
        ENetEvent l_event;
        bool l_quit = false;
        while( l_quit == false && enet_host_service( p_server, &l_event, 3000) > 0) {
            switch (l_event.type) {
                case ENET_EVENT_TYPE_RECEIVE: {
                    enet_packet_destroy (l_event.packet);
                } break;
                case ENET_EVENT_TYPE_DISCONNECT: {
                    for( uint32_t i = 0; i < NETWORK_SERVER_MAX_CLIENTS; i++)
                        if( getClient( i) != nullptr && getClient( i)->peerID == l_event.peer->incomingPeerID)
                            delClient(getClient( i));
                } break;
                default:
                break;
            }
            l_quit = true;
            for( uint32_t i = 0; i < NETWORK_SERVER_MAX_CLIENTS; i++)
                if( getClient( i) != nullptr)
                    l_quit = false;
        }

        enet_host_destroy( p_server);
    }
}

// Search for free space and if it is not available, a negative number is returned
int32_t server::getFreeClientIndex() {
    for( uint32_t i = 0; i < NETWORK_SERVER_MAX_CLIENTS; i++) {
        if( p_clients[i] == NULL)
            return i;
    }
    return -1;
}