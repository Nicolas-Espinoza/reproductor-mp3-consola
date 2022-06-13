#include "sound.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "portaudio.h"

static PaStreamParameters outputParameters;
static bool initialized = false;
static PaStream *stream;
static bool playing = false;
static const int FRAMES_PER_BUFFER = 64;

struct UserSoundData
{
    float *left;
    float *right;
    int length;
    int left_index;
    int right_index;
};

static struct UserSoundData sound_data;

static void StreamFinished( void* userData )
{
    (void)userData;
    playing = false;
}

static int updateStreamBuffer(  const void *inputBuffer, void *outputBuffer,
                                unsigned long framesPerBuffer,
                                const PaStreamCallbackTimeInfo* timeInfo,
                                PaStreamCallbackFlags statusFlags,
                                void *userData )
{
    struct UserSoundData *data = (struct UserSoundData*)userData;
    float *out = (float*)outputBuffer;
    unsigned long i;

    (void) timeInfo; /* Prevent unused variable warnings. */
    (void) statusFlags;
    (void) inputBuffer;

    if( ((data->left  != NULL) && (data->left_index  >= data->length)) ||
        ((data->right != NULL) && (data->right_index >= data->length))  )
        return paAbort;

    PaStreamCallbackResult retval = paContinue;
    for( i=0; i<framesPerBuffer; i++ )
    {
        if( data->left != NULL )
        {
            if( data->left_index < data->length )
                *out++ = data->left[data->left_index++];
            else
            {
                *out++ = 0.0f;
                retval = paComplete;
            }
        }
        if( data->right != NULL )
        {
            if( data->right_index < data->length )
                *out++ = data->right[data->right_index++];
            else
            {
                *out++ = 0.0f;
                retval = paComplete;
            }
        }
    }
    return (int)retval;
}

bool init_audio()
{
    if( initialized == true )
        return true;

    playing = false;
    bool retval = true;

    PaError err;
    err = Pa_Initialize();
    if( err != paNoError )
    {
        printf("Error: Could not init session with the sound card.\n");
        return false;
    }

    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    if (outputParameters.device == paNoDevice)
    {
        printf("Error: No default output device.\n");
        Pa_Terminate();
        return false;
    }
    initialized = true;

    return retval;
}

bool play_audio(float *left_channel,float *right_channel,int length,double sample_rate)
{
    bool retval = true;
    sound_data.left   = left_channel;
    sound_data.right  = right_channel;
    sound_data.length = length;
    sound_data.left_index  = 0;
    sound_data.right_index = 0;

    outputParameters.channelCount = 0;
    if( left_channel != NULL )
        outputParameters.channelCount++;
    if( right_channel != NULL )
        outputParameters.channelCount++;
    if( outputParameters.channelCount == 0 )
        return true;

    outputParameters.sampleFormat = paFloat32; /* 32 bit floating point output */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    PaError err;
    err = Pa_OpenStream(
              &stream,
              NULL, /* no input */
              &outputParameters,
              sample_rate,
              FRAMES_PER_BUFFER,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              updateStreamBuffer,
              &sound_data );
    if( err != paNoError )
    {
        printf("Error message: %s\n",Pa_GetErrorText(err));
        return false;
    }
    Pa_SetStreamFinishedCallback( stream, &StreamFinished );
    playing = true;
    err = Pa_StartStream( stream );
    if( err != paNoError )
    {
        printf("Error message: %s\n",Pa_GetErrorText(err));
        return false;
    }

    return retval;
}

bool playing_complete()
{
    return !playing;
}

void stop_audio()
{
    playing = false;
    Pa_StopStream(stream);
}

void close_audio()
{
    playing = false;
    Pa_Terminate();
    initialized = false;
    Pa_CloseStream(stream);
}

void test_audio()
{
    #define TABLE_SIZE 44100
    #define LENGTH 5*TABLE_SIZE

    float data[LENGTH];
    float datal[LENGTH];
    for( int i=0; i<LENGTH ; i++ )
    {
        data[i]  = (float) sin( 2.0*3.141592654*700.0*((double)i/(double)TABLE_SIZE)  );
        datal[i] = (float) sin( 2.0*3.141592654*500.0*((double)i/(double)TABLE_SIZE) );
    }

    if( init_audio() == false )
        return;

    printf("\nReproduciendo tonos de prueba stereo @44100Hz:\n");
    printf("  -> Canal izquierdo: tono senoidal de 700Hz\n");
    printf("  -> Canal derecho:   tono senoidal de 500Hz\n");
    play_audio(data,datal,LENGTH,44100);

    while( playing_complete() == false );
    printf("Reproduccion de prueba terminada.\n\n");

    close_audio();
}
