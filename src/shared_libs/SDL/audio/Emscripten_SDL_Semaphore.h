#ifndef EMSCRIPTEN_SDL_SEMAPHORE_H
#define EMSCRIPTEN_SDL_SEMAPHORE_H

#ifdef EMSCRIPTEN

// Emscripten currently doesn't have SDL Semaphore structures/functions
// in its SDL Net API, so we build our own to use
#include "SDL.h"

  
struct SDL_semaphore {
    Uint32 value;
    SDL_mutex *mutex;
    SDL_cond *cond; 
        
}; 

#endif EMSCRIPTEN

#endif //EMSCRIPTEN_SDL_SEMAPHORE_H
