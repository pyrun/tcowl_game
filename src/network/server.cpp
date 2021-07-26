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

void server::addSync( synchronisation *sync) {
    p_sync_objects.push_back( sync);
}

bool server::delSync( synchronisation *sync) {
    for( uint32_t i = 0; i < p_sync_objects.size(); i++) {
        //if( sync)
    }
}

bool server::addClient( TCPsocket socket) {
    uint32_t ipaddr;
    int32_t l_index = getFreeClientIndex();
    server_client *l_client = NULL;
    if( l_index == -1) {
        engine::log( engine::log_warn, "server::addClient no space for new connection");
        return false;
    }

    // set up the client
    p_clients[ l_index] = new server_client;
    l_client = getClient( l_index);
    l_client->socket = socket;
    l_client->index = l_index;
    l_client->id = p_id_counter++;

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

    return true;
}

void server::delClient( server_client *client) {
    if( SDLNet_TCP_DelSocket( p_server_socketset, client->socket) == -1) {
        engine::log( engine::log_error, "%s", SDLNet_GetError());
        exit(-1);
    }
 
    engine::log( engine::log_debug, "server::delClient close connection %d", client->id);
    SDLNet_TCP_Close( client->socket);
    uint32_t l_index = client->index;
    free( client);
    p_clients[ l_index] = NULL;
}

uint8_t* server::recvData( server_client *client, uint16_t* length) {
    uint8_t temp_data[ NETWORK_SERVER_PACKETSIZE];
    int num_recv = SDLNet_TCP_Recv( client->socket, temp_data, NETWORK_SERVER_PACKETSIZE);
 
    if(num_recv <= 0) {
        delClient( client);
        return NULL;
    } else {
        *length = num_recv;
 
        uint8_t* data = (uint8_t*) malloc(num_recv*sizeof(uint8_t));
        memcpy(data, temp_data, num_recv);
 
        return data;
    }
}

void server::update() {
    int len;
    TCPsocket l_client_socket;

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
        server_client *l_client = getClient( i);
        if( l_client == NULL)
            continue;
        if( !SDLNet_SocketReady( l_client->socket))
            continue;
        
        l_num_active_sockets--;
        
        uint16_t length, flag;
        uint8_t* data = recvData( l_client, &length);
        if( data == NULL)
            continue;

        engine::log( engine::log_debug, "Test: %.*s", length, data);

        free( data);
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