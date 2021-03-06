#include "sound_queue_WatchOS.h"
#include "../../non_core/logger.h"

#include <pthread.h>
#include <dispatch/dispatch.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>


// Callback that constantly outputs audio from the buffer
// AVAudio
static sample_t* bufs = 0;
static int write_buf = 0;
static int write_pos = 0;
static int read_buf = 0;
static int sem_counter =0; // For some reason dispatch_semaphore_t has no way in its API of
                        // being able to retrieve its actual value. So we'll have to track
                        // it ourselves, yay.
static dispatch_semaphore_t free_sem;
static int disable_sound = 0;

int sound_queue_start()
{
    
    write_buf = 0;
    write_pos = 0;
    read_buf = 0;
    
    bufs = (sample_t*)calloc(sizeof(sample_t), SOUND_QUEUE_BUFFER_SIZE * SOUND_QUEUE_BUFFER_COUNT);
    
    if ( !bufs ) {
        log_message(LOG_ERROR, "Run out of memory starting sound queue\n");
        return 0;
    }
    
    free_sem = dispatch_semaphore_create(SOUND_QUEUE_BUFFER_COUNT - 1);
    if (free_sem == NULL) {
        log_message(LOG_ERROR, "Couldn't create semaphore starting sound queue\n");
        return 0;
    }
    sem_counter = SOUND_QUEUE_BUFFER_COUNT - 1;
    
    return 1;
}

static sample_t* sound_queue_buf( int index )
{
    return bufs + (long) index * SOUND_QUEUE_BUFFER_SIZE;
}

void sound_queue_write(const sample_t* samples_in, int count)
{
    if (!disable_sound) {
        
    while ( count )
    {
        int n = SOUND_QUEUE_BUFFER_SIZE - write_pos;
        if ( n > count )
            n = count;
        
        memcpy( sound_queue_buf( write_buf ) + write_pos, samples_in, n * sizeof (sample_t) );
        samples_in += n;
        write_pos += n;
        count -= n;
        
        if ( write_pos >= SOUND_QUEUE_BUFFER_SIZE )
        {
            write_pos = 0;
            write_buf = (write_buf + 1) % SOUND_QUEUE_BUFFER_COUNT;
            dispatch_semaphore_wait(free_sem, DISPATCH_TIME_FOREVER);
            sem_counter--;
        }
    }
    }
}

void sound_queue_read(sample_t *data_out, uint32_t count) {

    if (!disable_sound) {
    
    if (sem_counter < (SOUND_QUEUE_BUFFER_COUNT - 1)) {
        memcpy(data_out, sound_queue_buf(read_buf), count);
        read_buf = (read_buf + 1) % SOUND_QUEUE_BUFFER_COUNT;
        dispatch_semaphore_signal(free_sem);
        sem_counter++;
    }
    else{
        memset(data_out, 0, count * sizeof(sample_t));
    }
        
    }
}


void disable_sound_queue(void) {
    disable_sound = 1;
    dispatch_semaphore_signal(free_sem);
}
