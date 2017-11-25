#include "synchro.h"
#include "ensitheora.h"
#include "pthread.h"

bool fini;

/* les variables pour la synchro, ici */
pthread_mutex_t m;
pthread_mutex_t m_texture;
pthread_mutex_t m_consommation;
pthread_cond_t image;
pthread_cond_t c_texture;
pthread_cond_t consommation_texture;
bool fenetrePrete = false;

int nombreTextureDeposee = 0; //compte le nombre de texture qui sont déposées mais qui n'ont pas été consommé

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
  pthread_mutex_lock(&m_consommation);
  
  while(nombreTextureDeposee == 0){
    pthread_cond_wait(&consommation_texture, &m_consommation);
  }
  
  nombreTextureDeposee --;
  pthread_mutex_unlock(&m_consommation);

}

void finConsommerTexture() {
  pthread_mutex_lock(&m_consommation);
  
  if(nombreTextureDeposee < NBTEX) {
    c_texture.signal(); //on reveille un thread qui depose de la texture
  }
  
  pthread_mutex_unlock(&m_consommation);
}


void debutDeposerTexture() {
  pthread_mutex_lock(&m_texture);
  while (nombreTextureDeposee > NBTEX){
    pthread_cond_wait(&c_texture, &m_texture);
  }

  nombreTextureDeposee ++;
 
  pthread_mutex_unlock(&m_texture);
}

void finDeposerTexture() {
  pthread_mutex_lock(&m_texture);
  
  if(nombreTextureDeposee > NBTEX) {
    consommation_texture.broadcast(); //on peut consommer de la texture
  }
  else {
    c_texture.signal(); //on peut deposer de la texture
  }
  pthread_mutex_unlock(&m_texture);
}
