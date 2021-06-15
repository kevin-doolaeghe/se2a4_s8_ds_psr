#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <string.h>

#define MAX_NOM	1024

/* Initialisation d'une socket UDP */

int initialisationSocketUDP(char *service){
struct addrinfo precisions,*resultat,*origine;
int statut;
int s;

/* Construction de la structure adresse */
memset(&precisions,0,sizeof precisions);
precisions.ai_family=AF_UNSPEC;
precisions.ai_socktype=SOCK_DGRAM;
precisions.ai_flags=AI_PASSIVE;
statut=getaddrinfo(NULL,service,&precisions,&origine);
if(statut<0){ perror("initialisationSocketUDP.getaddrinfo"); exit(EXIT_FAILURE); }
struct addrinfo *p;
for(p=origine,resultat=origine;p!=NULL;p=p->ai_next)
  if(p->ai_family==AF_INET6){ resultat=p; break; }

/* Creation d'une socket */
s=socket(resultat->ai_family,resultat->ai_socktype,resultat->ai_protocol);
if(s<0){ perror("initialisationSocketUDP.socket"); exit(EXIT_FAILURE); }

/* Options utiles */
int vrai=1;
#if 0
struct timeval minuteur;
minuteur.tv_sec=0;
minuteur.tv_usec=100;
#endif
if(setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&vrai,sizeof vrai)<0){
  perror("initialisationSocketUDP.setsockopt (REUSEADDR)");
  exit(EXIT_FAILURE);
  }
if(setsockopt(s,SOL_SOCKET,SO_BROADCAST,&vrai,sizeof vrai)<0){
  perror("initialisationSocketUDP.setsockopt (BROADCAST)");
  exit(EXIT_FAILURE);
  }
#if 0
if(setsockopt(s,SOL_SOCKET,SO_RCVTIMEO,&minuteur,sizeof minuteur)<0){
  perror("initialisationSocketUDP.setsockopt (BROADCAST)");
  exit(EXIT_FAILURE);
  }
#endif

/* Specification de l'adresse de la socket */
statut=bind(s,resultat->ai_addr,resultat->ai_addrlen);
if(statut<0) {
  perror("initialisationSocketUDP.bind");
  close(s); freeaddrinfo(origine);
  return -1;
  }

/* Liberation de la structure d'informations */
freeaddrinfo(origine);

return s;
}

/* Recevoir un message UDP dans les paramètres de sortie, l'adresse est retournée */

char *recevoirUDP(int s,unsigned char *message,int *taille){
struct sockaddr_storage adresse;
socklen_t mtaille=sizeof(adresse);
int nboctets=recvfrom(s,message,*taille,0,(struct sockaddr *)&adresse,&mtaille);
if(nboctets<0){ *taille=-1; return NULL; }
*taille=nboctets;
char *nom=malloc(MAX_NOM);
int statut=getnameinfo((struct sockaddr *)&adresse,mtaille,nom,MAX_NOM,NULL,0,0);
if(statut<0){
  perror("recevoirUDP.getnameinfo");
  return NULL;
  }
return nom;
}

/* Création d'une adresse socket UDP pour transmission */

void *creationAddresseUDP(char *hote,char *service){
struct addrinfo precisions,*resultat,*origine;
int statut;

/* Creation de l'adresse de socket */
memset(&precisions,0,sizeof precisions);
precisions.ai_family=AF_UNSPEC;
precisions.ai_socktype=SOCK_DGRAM;
statut=getaddrinfo(hote,service,&precisions,&origine);
if(statut<0){ perror("creationAddressUDP.getaddrinfo"); exit(EXIT_FAILURE); }
struct addrinfo *p;
for(p=origine,resultat=origine;p!=NULL;p=p->ai_next)
  if(p->ai_family==AF_INET6){ resultat=p; break; }

/* Copie de la structure adresse */
struct sockaddr_storage *adresse=malloc(sizeof(struct sockaddr_storage));
if(adresse==NULL){ perror("creationAddressUDP.malloc"); exit(EXIT_FAILURE); }
memcpy(adresse,resultat->ai_addr,resultat->ai_addrlen);

/* Liberation de la structure d'informations */
freeaddrinfo(origine);

return adresse;
}

/* Envoyer un message UDP à l'adresse fournie en paramètre (récupérée de la réception de message ou de la création d'adresse) */

void envoyerUDP(int s,void *adresse,unsigned char *message,int taille){
int nboctets=sendto(s,message,taille,0,(struct sockaddr *)adresse,sizeof(struct sockaddr_storage));
if(nboctets<0){ perror("messageUDP.sento"); exit(EXIT_FAILURE); }
}

/* Une fonction d’initialisation de serveur */

int initialisationServeur(char *service,int connexions){
struct addrinfo precisions,*resultat,*origine;
int statut;
int s;

/* Construction de la structure adresse */
memset(&precisions,0,sizeof precisions);
precisions.ai_family=AF_UNSPEC;
precisions.ai_socktype=SOCK_STREAM;
precisions.ai_flags=AI_PASSIVE;
statut=getaddrinfo(NULL,service,&precisions,&origine);
if(statut<0){ perror("initialisationServeur.getaddrinfo"); exit(EXIT_FAILURE); }
struct addrinfo *p;
for(p=origine,resultat=origine;p!=NULL;p=p->ai_next)
  if(p->ai_family==AF_INET6){ resultat=p; break; }

/* Creation d'une socket */
s=socket(resultat->ai_family,resultat->ai_socktype,resultat->ai_protocol);
if(s<0){ perror("initialisationServeur.socket"); exit(EXIT_FAILURE); }

/* Options utiles */
int vrai=1;
if(setsockopt(s,SOL_SOCKET,SO_REUSEADDR,&vrai,sizeof(vrai))<0){
  perror("initialisationServeur.setsockopt (REUSEADDR)");
  exit(EXIT_FAILURE);
  }
if(setsockopt(s,IPPROTO_TCP,TCP_NODELAY,&vrai,sizeof(vrai))<0){
  perror("initialisationServeur.setsockopt (NODELAY)");
  exit(EXIT_FAILURE);
  }

/* Specification de l'adresse de la socket */
statut=bind(s,resultat->ai_addr,resultat->ai_addrlen);
if(statut<0){ freeaddrinfo(origine); shutdown(s,SHUT_RDWR); return -1; }

/* Liberation de la structure d'informations */
freeaddrinfo(origine);

/* Taille de la queue d'attente */
statut=listen(s,connexions);
if(statut<0){ shutdown(s,SHUT_RDWR); return -2; }

return s;
}

/* Une fonction d’attente des clients TCP */

int boucleServeur(int ecoute,int (*traitement)(int))
{
int dialogue;

while(1){

    /* Attente d'une connexion */
    if((dialogue=accept(ecoute,NULL,NULL))<0) return -1;

    /* Passage de la socket de dialogue a la fonction de traitement */
    if(traitement(dialogue)<0){ shutdown(ecoute,SHUT_RDWR); return 0;}

    }
}

/* Connexion à un serveur TCP */

int connexionServeur(char *hote,char *service){
struct addrinfo precisions,*resultat,*origine;
int statut;
int s;

/* Creation de l'adresse de socket */
memset(&precisions,0,sizeof precisions);
precisions.ai_family=AF_UNSPEC;
precisions.ai_socktype=SOCK_STREAM;
statut=getaddrinfo(hote,service,&precisions,&origine);
if(statut<0){ perror("connexionServeur.getaddrinfo"); exit(EXIT_FAILURE); }
struct addrinfo *p;
for(p=origine,resultat=origine;p!=NULL;p=p->ai_next)
  if(p->ai_family==AF_INET6){ resultat=p; break; }

/* Creation d'une socket */
s=socket(resultat->ai_family,resultat->ai_socktype,resultat->ai_protocol);
if(s<0){ perror("connexionServeur.socket"); exit(EXIT_FAILURE); }

/* Connection de la socket a l'hote */
if(connect(s,resultat->ai_addr,resultat->ai_addrlen)<0)
  { freeaddrinfo(origine); shutdown(s,SHUT_RDWR); return -1; }

/* Liberation de la structure d'informations */
freeaddrinfo(origine);

return s;
}

/* Socket vers nom de client */

char *socketVersClient(int s){
struct sockaddr_storage adresse;
socklen_t taille=sizeof adresse;
int statut;

/* Recupere l'adresse de la socket distante */
statut=getpeername(s,(struct sockaddr *)&adresse,&taille);
if(statut<0){ perror("socketVersNom.getpeername"); exit(EXIT_FAILURE); }

/* Recupere le nom de la machine */
char *hote=malloc(MAX_NOM);
if(hote==NULL){ perror("socketVersClient.malloc"); exit(EXIT_FAILURE); }
getnameinfo((struct sockaddr *)&adresse,sizeof adresse,hote,MAX_NOM,NULL,0,0);
return hote;
}
