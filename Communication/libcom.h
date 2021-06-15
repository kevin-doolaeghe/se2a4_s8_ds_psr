int initialisationSocketUDP(char *service);
char *recevoirUDP(int s,unsigned char *message,int *taille);
void *creationAddresseUDP(char *hote,char *service);
void envoyerUDP(int s,void *adresse,unsigned char *message,int taille);
int initialisationServeur(char *service,int connexions);
int boucleServeur(int ecoute,int (*traitement)(int));
int connexionServeur(char *hote,char *service);
char *socketVersClient(int s);
