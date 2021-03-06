#ifndef SOUND_QUEUE_WATCHOS_H
#define SOUND_QUEUE_WATCHOS_H

#define SOUND_QUEUE_BUFFER_COUNT 3
#define SOUND_QUEUE_BUFFER_SIZE 8192

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>

typedef short sample_t;

int sound_queue_start(void);
void sound_queue_write(const sample_t* samples_in, int count);
void sound_queue_read(sample_t *data_out, uint32_t count);

void disable_sound_queue(void);


#ifdef __cplusplus
}
#endif

#endif
