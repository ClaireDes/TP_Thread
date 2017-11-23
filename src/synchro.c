#include "synchro.h"
#include "ensitheora.h"
#include "pthread.h"

bool fini;

/* les variables pour la synchro, ici */
pthread_mutex_t m;
pthread_cond_t image;
bool fenetrePrete = false;

/* l'implantation des fonctions de synchro ici */
void envoiTailleFenetre(th_ycbcr_buffer buffer) {
  pthread_cond_init(&image, NULL);
  pthread_mutex_lock(&m);
  windowsx = buffer->width;
  windowsy = buffer->height;
}

void attendreTailleFenetre() {
  pthread_mutex_lock(&m);
  while(windowsx) { //tant que la taille fenetre n'est pas récupérer
    pthread_cond_wait(&image, &m);
  }
  pthread_mutex_unlock(&m);
}

void signalerFenetreEtTexturePrete() {
  pthread_mutex_lock(&m);
  fenetrePrete = true;
  pthread_cond_signal(&image); //on reveille le thread qui attendait la fenetre
  pthread_mutex_unlock(&m);
}

void attendreFenetreTexture() {
  pthread_mutex_lock(&m);
  
  while(fenetrePrete == false){ //tant que la fenetre et la texture ne sont pas prete on attent
    pthread_cond_wait(&image, &m);
  }

  pthread_mutex_unlock(&m);
}

void debutConsommerTexture() {
}

void finConsommerTexture() {
}


void debutDeposerTexture() {
}

void finDeposerTexture() {
}
