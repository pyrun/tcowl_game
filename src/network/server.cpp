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
        p_clients[i] = NULL;
}

server::~server() {
    close();
}

bool server::begin() {
    engine::log( engine::log_debug, "server::begin starte Server auf Port %d", p_port);

    // Resolve the argument into an IPaddress type
    if(SDLNet_ResolveHost( &p_ipadress, NULL, p_port)) {
        engine::log( engine::log_error, "server::begin SDLNet_ResolveHost: %s",SDLNet_GetError());
        return false;
    }

    // open socket 
    p_server_socket = SDLNet_TCP_Open( &p_ipadress); 
    if( p_server_socket == NULL) { 
        engine::log( engine::log_error, "server::begin SDLNet_TCP_Open: %s",SDLNet_GetError());
        return false;
    }

    p_server_socketset = SDLNet_AllocSocketSet(NETWORK_SERVER_MAX_CLIENTS+1);
    if(p_server_socketset == NULL) {
        engine::log( engine::log_error, "server::begin SDLNet_AllocSocketSet: %s", SDLNet_GetError());
        return false;
    }

    if( SDLNet_TCP_AddSocket( p_server_socketset, p_server_socket) == -1) {
        engine::log( engine::log_error, "server::begin SDLNet_TCP_AddSocket: %s", SDLNet_GetError());
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

    if( client == NULL) {
        for( uint32_t i = 0; i < NETWORK_SERVER_MAX_CLIENTS; i++) {
            if( p_clients[i] == NULL)
                continue;
            int l_send_length = SDLNet_TCP_Send( p_clients[i]->socket, l_temp_data, l_offset);
            if( l_send_length < l_offset) {
                engine::log( engine::log_error, "server::sendPacket SDLNet_TCP_Send: %s", SDLNet_GetError());
                delClient( p_clients[i]);
            } else {
                p_clients[i]->ready = false;
            }
        }
    } else {
        int l_send_length = SDLNet_TCP_Send( client->socket, l_temp_data, l_offset);
        if( l_send_length < l_offset) {
            engine::log( engine::log_error, "server::sendPacket SDLNet_TCP_Send: %s", SDLNet_GetError());
            delClient( client);
        }
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

bool server::addClient( TCPsocket socket) {
    uint32_t ipaddr;
    int32_t l_index = getFreeClientIndex();
    client *l_client = NULL;
    if( l_index == -1) {
        engine::log( engine::log_warn, "server::addClient no space for new connection");
        return false;
    }

    // set up the client
    p_clients[ l_index] = new client;
    l_client = getClient( l_index);
    l_client->socket = socket;
    l_client->index = l_index;
    l_client->id = p_id_counter++;
    l_client->ready = true;

    // get the ip of the client and port number
    l_client->IPadress = SDLNet_TCP_GetPeerAddress( l_client->socket);
    if( !l_client->IPadress) {
        engine::log( engine::log_warn, "server::addClient SDLNet_TCP_GetPeerAddress: %s",SDLNet_GetError()); 
        return false;
    }

    // print out the clients IP and port number 
    ipaddr = SDL_SwapBE32( l_client->IPadress->host);
    engine::log( engine::log_debug, "server::addClient Accepted a connection with id %d from %d.%d.%d.%d port %hu", l_client->id, ipaddr>>24, (ipaddr>>16)&0xff, (ipaddr>>8)&0xff, ipaddr&0xff, l_client->IPadress->port); 
    
    int32_t l_numused = SDLNet_TCP_AddSocket( p_server_socketset, l_client->socket);
    if( l_numused == -1) {
        engine::log( engine::log_warn, "SDLNet_AddSocket: %s\n", SDLNet_GetError());
        // perhaps you need to restart the set and make it bigger...
    }

    for( uint32_t i = 0; i < p_sync_objects.size(); i++) {
        network::synchronisation *l_sync = p_sync_objects[i];
        l_sync->newClientCallback( l_client, this);
    }
    
    // heatbeat start
    sendHeartbeat( NULL);

    return true;
}

void server::delClient( client *client_ptr) {
    if( SDLNet_TCP_DelSocket( p_server_socketset, client_ptr->socket) == -1) {
        engine::log( engine::log_error, "%s", SDLNet_GetError());
        exit(-1);
    }

    engine::log( engine::log_debug, "server::delClient close connection %d", client_ptr->id);
    SDLNet_TCP_Close( client_ptr->socket);
    uint32_t l_index = client_ptr->index;
    free( client_ptr);
    p_clients[ l_index] = NULL;
}

uint8_t* server::recvData( client *client_ptr, uint16_t* length) {
    uint8_t l_temp_data[ *length];
    int l_num_recv = SDLNet_TCP_Recv( client_ptr->socket, l_temp_data, *length);
 
    if( l_num_recv <= 0) {
        delClient( client_ptr);
        return NULL;
    } else {
        *length = l_num_recv;
        uint8_t* l_data = (uint8_t*) malloc( l_num_recv*sizeof(uint8_t));
        memcpy( l_data, l_temp_data, l_num_recv);
        return l_data;
    }
}

void server::update() {
    TCPsocket l_client_socket;

    // network update
    bool l_ready = true;
    for( uint32_t i = 0; i < NETWORK_SERVER_MAX_CLIENTS; i++) {
        if( getClient( i) != NULL &&
            getClient( i)->ready == false)
            l_ready = false;
    }
    for( uint32_t i = 0; (i < p_sync_objects.size()) && l_ready; i++) {
        network::synchronisation *l_sync = p_sync_objects[i];
        l_sync->network_update( this);

        // heatbeat
        sendHeartbeat( NULL);
    }

    //try to accept a connection 
    l_client_socket = SDLNet_TCP_Accept( p_server_socket); 
    if( l_client_socket) {
        if( addClient( l_client_socket) == false) {
           SDLNet_TCP_Close( l_client_socket); 
        }
    }
    
    // check for recv
    int32_t l_num_active_sockets = SDLNet_CheckSockets( p_server_socketset, 0);
    for( uint32_t i = 0; (i < NETWORK_SERVER_MAX_CLIENTS) && l_num_active_sockets; i++) {
        client *l_client = getClient( i);
        if( l_client == NULL)
            continue;
        if( !SDLNet_SocketReady( l_client->socket))
            continue;

        // found one
        l_num_active_sockets--;

        // recv routine
        uint16_t l_length, flag;
        l_length = 2;
        uint8_t* data = recvData( l_client, &l_length);
        if( data == NULL ||
            l_length != 2) // min 2
            continue;

        packet l_packet;

        // get type and length
        l_packet.type = (network::packet_type)data[0];
        l_packet.length = data[1];

        // get data and CRC
        free( data);
        l_length = l_packet.length + 1; // crc
        data = recvData( l_client, &l_length);

        // check if valid
        if( l_length != l_packet.length + 1)
            continue;
        
        // copy data
        memcpy( l_packet.data, data, l_packet.length);
        l_packet.crc = data[ l_packet.length];
        free( data);

        // check CRC
        if( l_packet.crc != getCRC8( l_packet))
            continue;
        
        if( l_packet.type == network_type_heartbeat) {
            l_client->ready = true;
            continue;
        }

        // pocess packet
        for( uint32_t i = 0; i < p_sync_objects.size(); i++) {
            network::synchronisation *l_sync = p_sync_objects[i];
            l_sync->recvPacket( l_packet);
        }
    }
}

void server::close() {
    SDLNet_TCP_Close( p_server_socket); 
}

// Search for free space and if it is not available, a negative number is returned
int32_t server::getFreeClientIndex() {
    for( uint32_t i = 0; i < NETWORK_SERVER_MAX_CLIENTS; i++) {
        if( p_clients[i] == NULL)
            return i;
    }
    return -1;
}