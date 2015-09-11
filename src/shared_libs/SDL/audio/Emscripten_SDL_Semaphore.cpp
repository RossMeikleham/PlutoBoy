#ifdef EMSCRIPTEN

#include "Emscripten_SDL_Semaphore.h"
#include <stdlib.h>

SDL_sem *SDL_CreateSemaphore(Uint32 initialValue) {
    SDL_mutex *m;
    SDL_sem *s; 
    SDL_cond *c; 
    
    if ((s = (SDL_sem *)malloc(sizeof(SDL_sem))) == NULL) {
        goto failSem;
    }
    if ((c = SDL_CreateCond()) == NULL) {
        goto failCond;
    }    
    if ((m = SDL_CreateMutex()) == NULL) {
        goto failMut;
    }

    // Successfully allocated memory, set the semaphore values and
    // return it
    s->mutex = m;
    s->cond = c;
    s->value = initialValue;   
 
    return s;

    // Error occured while creating , destroy/free any allocated resources
    failMut: 
        SDL_DestroyCond(c);
    failCond: 
        free(s);
    failSem:

    return NULL;
}

void SDL_DestroySemaphore(SDL_sem *sem) {
    if (sem != NULL) {
        SDL_DestroyMutex(sem->mutex);
        SDL_DestroyCond(sem->cond);
        free(sem);
    }
}

Uint32 SDL_SemValue(SDL_sem *sem) {
    return sem->value;
}

int SDL_SemWait(SDL_sem *sem) {
    SDL_LockMutex(sem->mutex);
    while (sem->value <= 0) {
        SDL_CondWait(sem->cond, sem->mutex);
    }
    sem->value--;
    SDL_UnlockMutex(sem->mutex);
    return 0;
}

int SDL_SemPost(SDL_sem *sem) {
    SDL_LockMutex(sem->mutex);
    sem->value++;
    SDL_CondSignal(sem->cond);
    SDL_UnlockMutex(sem->mutex);
    return 0;
}

#endif //EMSCRIPTEN 
