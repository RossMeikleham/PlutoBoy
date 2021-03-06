#include "../../non_core/joypad.h"
#include <unistd.h>
#include <pthread.h>

static int down_state, up_state, left_state, right_state, a_state, b_state, start_state, select_state;

static pthread_rwlock_t rwlock = PTHREAD_RWLOCK_INITIALIZER;

/*  Intialize the joypad, should be called before any other
 *  joypad functions */
void init_joypad() {
    
}

/* Check each individual GameBoy key. Returns 1 if
 * the specified key is being held down, 0 otherwise */
int down_pressed() {
    pthread_rwlock_rdlock(&rwlock);
    int result = down_state;
    pthread_rwlock_unlock(&rwlock);
    return result;
}
int up_pressed() {
    pthread_rwlock_rdlock(&rwlock);
    int result = up_state;
    pthread_rwlock_unlock(&rwlock);
    return result;
}
int left_pressed() {
    pthread_rwlock_rdlock(&rwlock);
    int result = left_state;
    pthread_rwlock_unlock(&rwlock);
    return result;
}

int right_pressed() {
    pthread_rwlock_rdlock(&rwlock);
    int result = right_state;
    pthread_rwlock_unlock(&rwlock);
    return result;
}

int a_pressed() {
    pthread_rwlock_rdlock(&rwlock);
    int result = a_state;
    pthread_rwlock_unlock(&rwlock);
    return result;
}

int b_pressed() {
    pthread_rwlock_rdlock(&rwlock);
    int result = b_state;
    pthread_rwlock_unlock(&rwlock);
    return result;
}

int start_pressed()  {
    pthread_rwlock_rdlock(&rwlock);
    int result = start_state;
    pthread_rwlock_unlock(&rwlock);
    return result;
    
}

int select_pressed() {
    pthread_rwlock_rdlock(&rwlock);
    int result = select_state;
    pthread_rwlock_unlock(&rwlock);
    return result;
}

// Set state of pressed keys
void set_down_pressed(int state) {
    pthread_rwlock_wrlock(&rwlock);
    down_state = state;
    pthread_rwlock_unlock(&rwlock);
}

void set_up_pressed(int state) {
    pthread_rwlock_wrlock(&rwlock);
    up_state = state;
    pthread_rwlock_unlock(&rwlock);
}

void set_left_pressed(int state) {
    pthread_rwlock_wrlock(&rwlock);
    left_state = state;
    pthread_rwlock_unlock(&rwlock);
}

void set_right_pressed(int state) {
    pthread_rwlock_wrlock(&rwlock);
    right_state = state;
    pthread_rwlock_unlock(&rwlock);
}

void set_a_pressed(int state) {
    pthread_rwlock_wrlock(&rwlock);
    a_state = state;
    pthread_rwlock_unlock(&rwlock);
}

void set_b_pressed(int state) {
    pthread_rwlock_wrlock(&rwlock);
    b_state = state;
    pthread_rwlock_unlock(&rwlock);
}

void set_start_pressed(int state) {
    pthread_rwlock_wrlock(&rwlock);
    start_state = state;
    pthread_rwlock_unlock(&rwlock);
}

void set_select_pressed(int state) {
    pthread_rwlock_wrlock(&rwlock);
    select_state = state;
    pthread_rwlock_unlock(&rwlock);
}

/* Returns 1 if any of the 8 GameBoy keys are being held down,
 * 0 otherwise */
int key_pressed() {
    pthread_rwlock_rdlock(&rwlock);
    int result = down_state || up_state || left_state  || right_state ||
            a_state    || b_state  || start_state || select_state;
    pthread_rwlock_unlock(&rwlock);
    return result;
}


// Given relative screen x and y positions and an on/off state
// sets any buttons those co-ordinates are in to the given state.
void check_keys_pressed(float x, float y, int state) {
}

// Given new x/y relative screen positions and x/y movement 
// determines which keys are now pressed
void check_keys_moved(float x, float y, float mv_x, float mv_y) {
}


void unset_keys() {
    pthread_rwlock_wrlock(&rwlock);
    down_state = 0;
    up_state = 0;
    left_state = 0;
    right_state = 0;
    a_state = 0;
    b_state = 0;
    start_state = 0;
    select_state = 0;
    pthread_rwlock_unlock(&rwlock);
}


/* Update current state of GameBoy keys as well as control
 * other external actions for the emulator */
int update_keys() {
    return 0;
}
