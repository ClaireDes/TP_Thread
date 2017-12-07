#include "synchro.h"
#include "ensitheora.h"
#include "pthread.h"

bool fini;

/* les variables pour la synchro, ici */
pthread_mutex_t m;
pthread_mutex_t m_texture;
pthread_mutex_t m_consommation;
pthread_cond_t tailleFenetre;
pthread_cond_t attenteTexture;
pthread_cond_t c_texture;
pthread_cond_t consommation_texture;

bool fenetrePrete = false;
bool texturePrete = false;

int nombreTextureDeposee = 0; //compte le nombre de texture qui sont déposées mais qui n'ont pas été consommé

/* l'implantation des fonctions de synchro ici */
void envoiTailleFenetre(th_ycbcr_buffer buffer) {
  //printf("envoiTailleFenetre\n");
  //pthread_cond_init(&tailleFenetre, NULL);
  //pthread_mutex_lock(&m);
  windowsx = buffer->width;
  windowsy = buffer->height;
  //printf("windowx: %d\n", windowsx);
  //printf("windowy: %d\n", windowsy);
  fenetrePrete = true;

  pthread_cond_broadcast(&tailleFenetre);

  //signalerFenetreEtTexturePrete();
  //attendreTailleFenetre();
}

void attendreTailleFenetre() {
  //printf("attendreTailleFenetre\n");
  pthread_mutex_lock(&m);
  //printf("windowxAttendre: %d\n", windowsx);
  //printf("windowyAttendre: %d\n", windowsy);
  while(!fenetrePrete) { //tant que la taille fenetre n'est pas récupérer
    pthread_cond_wait(&tailleFenetre, &m);
  }
  
  //printf("windowxRecuperation: %d\n", windowsx);
  //printf("windoyRecuperation: %d\n", windowsy);

  //signalerFenetreEtTexturePrete();

  pthread_mutex_unlock(&m);
}

void signalerFenetreEtTexturePrete() {
  //printf("signalerFenetreEtTexturePrete\n");
  pthread_mutex_lock(&m);
  
  fenetrePrete = true;
  texturePrete = true;

  pthread_cond_signal(&attenteTexture);

  //while (fenetrePrete == false && texturePrete == false) {
  //pthread_cond_signal(&image); //on reveille le thread qui attendait la fenetre
  //}

  pthread_mutex_unlock(&m);
}

void attendreFenetreTexture() {

  // printf("attendreFenetreTexture\n");

  pthread_mutex_lock(&m);
  
  while(!texturePrete){ //tant que la fenetre et la texture ne sont pas prete on attent
    pthread_cond_wait(&attenteTexture, &m);
  }
  //printf("FIN\n");
  //pthread_cond_signal(&c_texture); //on reveille un thread qui depose de la texture
  pthread_mutex_unlock(&m);
  

}

void debutConsommerTexture() {

  //printf("debutConsommerTexture\n");

  //pthread_mutex_lock(&m_consommation);
  pthread_mutex_lock(&m_texture);

  while(nombreTextureDeposee == 0){
    pthread_cond_wait(&consommation_texture, &m_texture);
  }
  
  nombreTextureDeposee --;
  //pthread_mutex_unlock(&m_consommation);
  pthread_mutex_unlock(&m_texture);
}

void finConsommerTexture() {

  //printf("finConsommerTexture\n");

  pthread_mutex_lock(&m_consommation);
  
  /*while(nombreTextureDeposee < NBTEX) {
  pthread_cond_signal(&c_texture); //on reveille un thread qui depose de la texture
  }*/

  while(nombreTextureDeposee <= 0) {
    pthread_cond_signal(&c_texture); //on reveille un thread qui depose de la texture
  }
  
  pthread_mutex_unlock(&m_consommation);
}


void debutDeposerTexture() {

  //printf("debutDeposerTexture\n");

  pthread_mutex_lock(&m_texture);
 
  while(nombreTextureDeposee > 0) {
    pthread_cond_wait(&c_texture, &m_texture);
  }

  /*while (nombreTextureDeposee > NBTEX){
    pthread_cond_wait(&c_texture, &m_texture);
    }*/
 
  //printf("La\n");
  nombreTextureDeposee ++;
  //printf("nombreTextureDeposee = %d\n", nombreTextureDeposee);
  pthread_mutex_unlock(&m_texture);
  //printf("Ici\n");
}

void finDeposerTexture() {

  //printf("finDeposerTexture\n");
  
  pthread_mutex_lock(&m_texture);
  
  if(nombreTextureDeposee > 0) {
    pthread_cond_broadcast(&consommation_texture);
  }
  else {
    pthread_cond_signal(&c_texture);
  }


  /*if(nombreTextureDeposee > NBTEX) {
    pthread_cond_broadcast(&consommation_texture); //on peut consommer de la texture
  }
  else {
    pthread_cond_signal(&c_texture); //on peut deposer de la texture
    }*/
  pthread_mutex_unlock(&m_texture);
}
