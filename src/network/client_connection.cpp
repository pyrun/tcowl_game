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
    uint8_t l_temp_data[NETWORK_PACKET_MAX_SIZE];
    uint32_t l_offset = 2;

    l_temp_data[0] = packet.type;
    l_temp_data[1] = packet.length;
    memcpy( l_temp_data + l_offset, packet.data, packet.length);
    l_offset += packet.length;

    l_temp_data[l_offset] = getCRC8( packet);
    l_offset++;

    int l_send_length = SDLNet_TCP_Send( p_socket, l_temp_data, l_offset);
    if( l_send_length < l_offset) {
        engine::log( engine::log_error, "server::sendPacket SDLNet_TCP_Send: %s", SDLNet_GetError());
        close();
    }
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

uint8_t* client_connection::recvData( uint16_t *length) {
    uint8_t l_temp_data[ *length];
    int l_num_recv = SDLNet_TCP_Recv( p_socket, l_temp_data, *length);
 
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
    int32_t l_ready = 1;
    while( l_ready) {
        // check if we connected
        if( p_socketset == NULL)
            return;
        l_ready = SDLNet_CheckSockets( p_socketset, 0);
        if( SDLNet_SocketReady( p_socket)) {
            uint16_t l_length, flag;
            l_length = 2;
            uint8_t* data = recvData( &l_length);
            if( data == NULL ||
                l_length != 2) // min 3
                return;

            packet l_packet;

            // get type and length
            l_packet.type = (network::packet_type)data[0];
            l_packet.length = data[1];

            // get data and CRC
            free( data);
            l_length = l_packet.length + 1; // crc
            data = recvData( &l_length);

            // check if valid
            if( l_length != l_packet.length + 1)
                return;
            
            // copy data
            memcpy( l_packet.data, data, l_packet.length);
            l_packet.crc = data[ l_packet.length];
            free( data);

            // check CRC
            if( l_packet.crc != getCRC8( l_packet))
                return;
            
            if( l_packet.type == network_type_heartbeat) {
                sendHeartbeat( NULL);
                return;
            }

            // pocess packet
            for( uint32_t i = 0; i < p_sync_objects.size(); i++) {
                network::synchronisation *l_sync = p_sync_objects[i];
                l_sync->recvPacket( l_packet);
            }
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