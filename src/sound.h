#ifndef SOUND_H
#define SOUND_H

#include <stdbool.h>

void test_audio(void);

bool init_audio(void);

bool play_audio(float *left_channel,
                float *right_channel,
                int length,
                double sample_rate);

bool playing_complete();

void stop_audio(void);

void close_audio(void);

#endif // SOUND_H
