
#include <stdint.h>
#include "../../non_core/serial_io_transfer.h"
#include "../../non_core/logger.h"

#include <SDL/SDL.h>
#include "SDL_net.h"

static int is_server = 0;
static TCPsocket client = NULL;
static TCPsocket server = NULL;

/* Setup TCP Client, and attempt to connect
 * to the server */
int setup_client(unsigned port) {

    is_server = 0;

    //SDL_INIT(SDL_INIT_EVERYTHING); 
    SDLNet_Init();   

    IPaddress ip;
    //TODO, for now always connect to localhost, fix for any specified ip in
    //the future
    SDLNet_ResolveHost(&ip, "127.0.0.1", port);
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
    return 1;
}

/*  Send and Recieved byte */
uint8_t transfer(uint8_t data) {return 0;}


void quit_io() {
    client = NULL;
    server = NULL;
    SDLNet_Quit();
}
