/*****************************************************/
/** Ce fichier centralise toutes les actions au     **/
/** sujet des threads                               **/
/*****************************************************/

/** Fichiers d'inclusion **/

#include <stdio.h> 
#include <pthread.h> 
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include "libflux.h" 

/** Constantes **/

/** Variables globales **/

/** Structure privee **/

typedef struct{
  void *parametre;
  void (*comportement)(void *parametre);
  } parametresThread;

/** Lancement d'un thread client **/

static void *interfaceFlux(void *generique){
parametresThread *argument=(parametresThread *)generique;
argument->comportement(argument->parametre);
if(argument->parametre!=NULL) free(argument->parametre);
free(argument);
return NULL;
}

int lanceFlux(void (*comportement)(void *parametre),void *parametre,int taille){
pthread_t tid;
pthread_attr_t attribut;
parametresThread *argument=(parametresThread *)malloc(sizeof(parametresThread));
if(taille>0 && parametre!=NULL){
  argument->parametre=malloc(taille);
  memcpy(argument->parametre,parametre,taille);
  }
else argument->parametre=NULL;
argument->comportement=comportement;
pthread_attr_init(&attribut);
pthread_attr_setdetachstate(&attribut,PTHREAD_CREATE_DETACHED);
if(pthread_create(&tid,&attribut,interfaceFlux,(void *)argument)<0){
  perror("lanceFlux.pthread_create");
  return(-1);
  }

return 0;
}
