#include "server.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "../engine/log.h"

using namespace network;

server::server() {
    p_port = HTTP_SERVER_DEFAULT_PORT;
    p_id_counter = 0;
}

server::~server() {
    close();
}

bool server::begin() {
    engine::log( engine::log_debug, "server::begin starte Server auf Port %d\n", p_port);

    // Resolve the argument into an IPaddress type
    if(SDLNet_ResolveHost( &p_ipadress, NULL, p_port)) {
        engine::log( engine::log_error, "server::begin SDLNet_ResolveHost: %s\n",SDLNet_GetError());
        return false;
    }

    // open socket 
    p_server = SDLNet_TCP_Open( &p_ipadress); 
    if( p_server == NULL) { 
        engine::log( engine::log_error, "server::begin SDLNet_TCP_Open: %s\n",SDLNet_GetError());
        return false;
    }

    return true;
}

void server::update() {
    int len;
    TCPsocket l_client_socket;

    //try to accept a connection 
    l_client_socket = SDLNet_TCP_Accept( p_server); 
    if ( l_client_socket) {
        server_client l_client;
        l_client.socket = l_client_socket;
        // get the clients IP and port number        
        l_client.IPadress = SDLNet_TCP_GetPeerAddress( l_client.socket); 
        if ( l_client.IPadress) {
            // print out the clients IP and port number 
            uint32_t ipaddr;
            ipaddr = SDL_SwapBE32( l_client.IPadress->host);
            engine::log( engine::log_debug, "server::update Accepted a connection from %d.%d.%d.%d port %hu\n", ipaddr>>24, (ipaddr>>16)&0xff, (ipaddr>>8)&0xff, ipaddr&0xff, l_client.IPadress->port); 

            char buffer[1024];
            SDLNet_TCP_Recv( l_client.socket, (void*)buffer, 1024);
            engine::log( engine::log_debug, "Recv:------------------\n%s\n-------------------\n", buffer);

            len = SDLNet_TCP_Send( l_client.socket, "Hello World", strlen("Hello World"));

            if(len < strlen("Hello World")) 
                engine::log( engine::log_debug, "SDLNet_TCP_Send: %s\n", SDLNet_GetError()); 
            
            // close socket        
            SDLNet_TCP_Close( l_client.socket); 
            engine::log( engine::log_debug, "server::update request and send the page\n");


        } else {
            engine::log( engine::log_warn, "SDLNet_TCP_GetPeerAddress: %s\n",SDLNet_GetError()); 
        }
    }
}

void server::close() {
    SDLNet_TCP_Close( p_server); 
}