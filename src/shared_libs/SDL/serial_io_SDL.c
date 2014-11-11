
#include <stdint.h>
#include "../../non_core/serial_io_transfer.h"
#include "../../non_core/logger.h"

#include <SDL/SDL.h>
#include "SDL_net.h"

static int is_client = 0;
static int is_server = 0;
static int connection_up = 0;
static TCPsocket client = NULL;
static TCPsocket server = NULL;

/* Setup TCP Client, and attempt to connect
 * to the server */
int setup_client(unsigned port) {
    
    is_client = 1;

    log_message(LOG_INFO, "Attempting to connect to server on port %u\n",port);
    //SDL_INIT(SDL_INIT_EVERYTHING); 
    SDLNet_Init();   

    IPaddress ip;
    //TODO, for now always connect to localhost, fix for any specified ip in
    //the future
    SDLNet_ResolveHost(&ip, "localhost", port);

    client =  SDLNet_TCP_Open(&ip);
    connection_up = 1;
    return 1;
}

/*  Setup TCP Server, and wait for a single
 *  client to connect */
int setup_server(unsigned port) { 
 
    is_server = 1;

    log_message(LOG_INFO, "Starting server on port %u\n",port);

    //SDL_INIT(SDL_INIT_EVERYTHING); 
    SDLNet_Init();   

    IPaddress ip;
    SDLNet_ResolveHost(&ip, NULL, port);

    server = SDLNet_TCP_Open(&ip);

    log_message(LOG_INFO, "Waiting for client to connect\n");
    
    while (client == NULL) {
        client = SDLNet_TCP_Accept(server);
        SDL_Delay(1000);
    }

    log_message(LOG_INFO, "Client successfully connected\n");
    connection_up = 1;
    return 1;
}

/*  Send and Recieved byte */
uint8_t transfer(uint8_t data) {

    uint8_t recieve;
    log_message(LOG_INFO, "Sending byte %x\n", data);
    if (is_server) {
        if (SDLNet_TCP_Send(client, &data, 1) != 1){
            log_message(LOG_ERROR, "Error sending byte to client\n");
        } 
        if (SDLNet_TCP_Recv(server, &recieve, 1) != 1) {
            log_message(LOG_ERROR, "Error recieving data back from client\n");
        } else {log_message(LOG_INFO, "Recieved byte %x\n", recieve);}

    } else if (is_client) {
        if (SDLNet_TCP_Recv(client, &recieve, 1) != 1){
            log_message(LOG_ERROR, "Error recieving data from the server\n");
        } else {log_message(LOG_INFO, "Recieved byte %x\n", recieve);}

        if (SDLNet_TCP_Send(client, &data, 1) != 1) {
            log_message(LOG_ERROR, "Error sending data back to server\n");
        }
    }


    return recieve;
}


void quit_io() {
    client = NULL;
    server = NULL;
    SDLNet_Quit();
}
