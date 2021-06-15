#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "libcom.h"
#include "libflux.h"

/** Constantes **/

#define PORT_UDP		"4001"	// ou 5001
#define PORT_TCP		"4002"	// ou 5002

#define MAX_CONN        1024
#define CONNECTE        1
#define NON_CONNECTE    0

#define MAX_TAMPON      2048

#define BROADCAST       "255.255.255.255"
#define UDP_MESG        "bonjour"
#define STOP		"StOp"

/** Variables publiques **/

/** Variables statiques **/

int T[MAX_CONN];

/** Procedure principale **/

void quitter() {
    int i;
    for (i = 0; i < MAX_CONN; i++) {
        if (T[i] == CONNECTE)
	    close(i);
    }
    printf("bye!\n");
    exit(0);
}

void init_clients() {
    int i;
    for (i = 0; i < MAX_CONN; i++) {
        T[i] = NON_CONNECTE;
    }
}

void distribuer_messsage(char* message, int taille, int s) {
    int i;
    for (i = 0; i < MAX_CONN; i++) {
        if (T[i] == CONNECTE && i != s)
            write(i, message, taille);
    }
}

int traiter_message(void *arg) {
    int dialogue = *(int *)arg;
    char message[MAX_TAMPON];
    int nboctets;

    /* Client connece */
    T[dialogue] = CONNECTE;

    while (1) {
        /* Attente d'un message */
        nboctets = read(dialogue, message, MAX_TAMPON - 1);
        if (nboctets <= 0)
	    break;
        message[nboctets] = 0;

        printf("recieved message: %s\n", message);

        /* Traitement de la trame recue */
        distribuer_messsage(message, nboctets, dialogue);

	if (strcmp(message, STOP) == 0)
            quitter();
    }
    close(dialogue);
    /* Client deconnecte */
    T[dialogue] = NON_CONNECTE;
    return 0;
}

int gerer_client(int dialogue) {
    lanceFlux((void (*)(void *))traiter_message, (void*)&dialogue, sizeof(dialogue));
    return 0;
}

int main(void) {
    int udp = initialisationSocketUDP(PORT_UDP);
    void *adresse = creationAddresseUDP(BROADCAST, PORT_UDP);

    char* message = UDP_MESG;
    int taille = strlen(message);

    envoyerUDP(udp, adresse, (unsigned char*) message, taille);

    init_clients();

    int tcp = initialisationServeur(PORT_TCP, MAX_CONN);
    boucleServeur(tcp, gerer_client);

    return 0;
}
