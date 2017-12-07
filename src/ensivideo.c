#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <SDL2/SDL.h>

#include "stream_common.h"
#include "oggstream.h"
#include "pthread.h"


int main(int argc, char *argv[]) {
    int res;
    pthread_t decodeTheora, decodeVorbis;

    printf("Salut\n");

    if (argc != 2) {
	fprintf(stderr, "Usage: %s FILE", argv[0]);
	exit(EXIT_FAILURE);
    }
    assert(argc == 2);


    // Initialisation de la SDL
    res = SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO|SDL_INIT_EVENTS);
    atexit(SDL_Quit);
    assert(res == 0);
    
    //printf("Bonjour\n");

    // start the two stream readers
    pthread_create(&decodeTheora, NULL, theoraStreamReader, argv[1]); //creation du thread theora
    pthread_create(&decodeVorbis, NULL, vorbisStreamReader, argv[1]); //creation du thead vorbis
    
    //printf("Hey\n");

    // wait audio thread
    pthread_join(decodeVorbis, NULL); //on attend la terminaison du thread vorbis
    
    //  printf("Hello World\n");

    // 1 seconde de garde pour le son,
    sleep(1);

    // tuer les deux threads videos si ils sont bloqués
    pthread_cancel(decodeTheora);
    pthread_cancel(threadAffichage);

    // attendre les 2 threads videos

    
    exit(EXIT_SUCCESS);    
}
