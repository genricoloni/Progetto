#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "costanti.h"

typedef enum { false, true } bool;

//Funzione che permette di trovare la port
int findPort(int argc, char* argv[]){
    int port = 4242;
    if(argc >= 2){
        port = atoi(argv[1]);
    }

    return port;
}

//Converte il comando testuale in un codice per permettere l'uso dei define
//Se il codice non viene riconosciuto, viene assegnato un codice di errore
int cmd_to_code(char* code){
    if(strcmp(code, "signup") == 0)
        return SIGNUP_CODE;
    else if (strcmp(code, "in") == 0)
        return IN_CODE;
    //da completare con tutti gli altri codici
    else return -1;
    
}

//data una stringa, la divide in altre 4 stringhe utili per
//le funzionalità di signup e login
void extractor(char* buffer, char* cmd, char* port, char* user, char* pass){
    cmd = strtok(buffer, " ");
    port = strtok(NULL, " ");
    user = strtok (NULL, " ");
    pass = strtok(NULL, " ");
}




//la funzione si occupa di estrarre credenziali e password dalla stringa,
//inviarle al server e aspettare una conferma di avvenuta registrazione
void singup(char* buffer[]){
    

    //invio dell'username al server

    //invio della password al server

    //aspetto la conferma dal server 
    printf("credenziali registrate correttmente!\n");
    return;
}

int login(char* buffer, int port){
    /*devo inviare i dati e 
    una volta concluso correttamente
    il processo di login, visualizzare
    il menu principale con le nuove opzioni*/

    return 1;
}
