#include "client_connection.h"

#include <stdio.h>
#include <string.h>
#include "../engine/log.h"

using namespace network;

client_connection::client_connection() {
    p_port = NETWORK_SERVER_DEFAULT_PORT;
    p_address = "localhost";
    p_input_length = 0;
}

client_connection::~client_connection() {

}

bool client_connection::begin() {
    IPaddress l_ip;

    if( SDLNet_ResolveHost( &l_ip, p_address.c_str(), p_port) == -1) {
        engine::log( engine::log_error, "client_connection::begin SDLNet_ResolveHost: %s",SDLNet_GetError());
        return false;
    }
 
    p_socket = SDLNet_TCP_Open(&l_ip);
    if( p_socket == NULL) {
        engine::log( engine::log_error, "client_connection::begin SDLNet_TCP_Open: %s",SDLNet_GetError());
        return false;
    }
 
    p_socketset = SDLNet_AllocSocketSet(1);
    if( p_socketset == NULL) {
        engine::log( engine::log_error, "client_connection::begin SDLNet_AllocSocketSet: %s",SDLNet_GetError());
        return false;
    }
 
    if( SDLNet_TCP_AddSocket( p_socketset, p_socket) == -1) {
        engine::log( engine::log_error, "client_connection::begin SDLNet_TCP_AddSocket: %s",SDLNet_GetError());
        return false;
    }
    return true;
}

void client_connection::sendPacket( packet packet, client *client) {

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

uint8_t* client_connection::recvData( uint16_t* length) {
    uint8_t l_temp_data[ NETWORK_PACKET_MAX_SIZE];
    int l_num_recv = SDLNet_TCP_Recv( p_socket, l_temp_data, NETWORK_PACKET_MAX_SIZE);
 
    if( l_num_recv <= 0) {
        close();
        return NULL;
    } else {
        *length = l_num_recv;
        uint8_t* l_data = (uint8_t*) malloc( l_num_recv*sizeof(uint8_t));
        memcpy( l_data, l_temp_data, l_num_recv);
        return l_data;
    }
}

void client_connection::update() {
    // check for recv
    SDLNet_CheckSockets( p_socketset, 0);
    if( SDLNet_SocketReady( p_socket)) {
        uint16_t l_length, flag;
        uint8_t* data = recvData( &l_length);
        if( data == NULL)
            return;
        
        if( p_input_length) {
            memcpy( p_input_stream + p_input_length, data, l_length);
            free( data);
            l_length = l_length+p_input_length;
            data = (uint8_t*) malloc( l_length*sizeof(uint8_t));
            memcpy( data, p_input_stream, l_length);
            p_input_length = 0;
        }

        // todo stream
        packet l_packet;
        l_packet.type = (network::packet_type)data[0];
        l_packet.length = data[1];
        memcpy( l_packet.data, data + 2, l_packet.length);
        l_packet.crc = data[ 2 + l_packet.length];

        l_length = l_length - (2 + l_packet.length);

        if( l_length) {
            memcpy( p_input_stream, data + 2 + l_packet.length + 1, l_length);
            p_input_length = l_length;
        }

        free( data);

        for( uint32_t i = 0; i < p_sync_objects.size(); i++) {
            network::synchronisation *l_sync = p_sync_objects[i];
            l_sync->recvPacket( l_packet);
        }
    }
}

void client_connection::close() {
    if(SDLNet_TCP_DelSocket( p_socketset, p_socket) == -1) {
        engine::log( engine::log_error, "client_connection::close SDLNet_TCP_DelSocket: %s",SDLNet_GetError());
        exit(-1);
    }
 
    SDLNet_FreeSocketSet( p_socketset);
    SDLNet_TCP_Close( p_socket);
}