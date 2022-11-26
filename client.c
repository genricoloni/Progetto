#include "utils.c"

int DEVICE_PORT;
char OWN_USER[USERN_CHAR];

int main(int argc, char* argv[]){
    
    char command[MAXCMD], port[MAXPORT];
    char buffer[BUFSIZE];
    char username[USERN_CHAR], *filename = NULL;
    int code, cl_socket;
    uint32_t server_com, cl_listener, ret, fdmax, i;
    uint32_t addrlen, newfd;
    char path[100];

    FILE *fp;

    struct credenziali credenziali;

    bool connected = false, conn_error = false, cmd_err = false, su = true, reg = false, in = false, login = true;
    bool client_offline = false, in_chat = false;;

    // strutture per indirizzi
    struct sockaddr_in server_addr, client_addr, cl_listener_addr, gp_addr;

    fd_set master, readfds;

    system("clear");

	FD_ZERO(&master);
	FD_ZERO(&readfds);

    OWN_PORT = findPort(argc, argv);

    //pulizia memoria e gestione indirizzi del SERVER
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

    //socket col server

    server_com = socket(AF_INET, SOCK_STREAM, 0);

    //aggiungp il socket appena creato alla lista dei socket da monitorare
    FD_SET(STDIN, &master);

	

    //pulizia memoria e gestione indirizzi del CLIENT
	memset(&client_addr, 0, sizeof(client_addr));
	cl_listener_addr.sin_family = AF_INET;
	cl_listener_addr.sin_port = htons(atoi(argv[1]));
	inet_pton(AF_INET, "127.0.0.1", &cl_listener_addr.sin_addr);

	cl_listener = socket(AF_INET, SOCK_STREAM, 0);
	bind(cl_listener, (struct sockaddr *)&cl_listener_addr, sizeof(cl_listener_addr));
	listen(cl_listener, 50);

	FD_SET(cl_listener, &master);
	fdmax = cl_listener;

 

   
   /*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<MENU DI LOGIN/SIGNUP>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
    
    while (1)
    {   
        printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<AREA DI ACCESSO>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
        printf("1)signup port user password -->per registrarsi al servizio\n");
        printf("2)in     port user password -->per  accedere   al servizio\n");

        if(conn_error == true)
            perror("Errore in fase di connessione col server, verificare la porta;\n");

        if(cmd_err == true)
            printf("+++Comando [%s] non riconosciuto+++\n", command);

        if(reg == true)
            printf("Utente già registrato!\n");

        if(su == false)
            printf("credenziali registrate correttmente!\n");

        if(login == false)
            printf("credenziali di accesso errate o utente non registrato!\n");


        conn_error = false;
        cmd_err = false;
        reg = false;
        fgets(buffer, 1024 - 1, stdin);
		sscanf(buffer, "%s %s %s %s", command, port, credenziali.username, credenziali.password);
		
        code = cmd_to_code(command);

        if(code == -1){
            cmd_err = true;
            system("clear");
            continue;
        }

        if(connected == false){
            //mi connetto al server usando la porta inserita in input
            server_addr.sin_port = htons(atoi(port));

            ret = connect(server_com, (struct sockaddr*)&server_addr, sizeof(server_addr));

            if (ret < 0){
                conn_error = true;
                system("clear");
                continue;            
            }
            
            connected = true;
            printf("<<<<<<<<<<<<<<<<<<<<<<<<CONNESSIONE RIUSCITA>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
        }
       
        switch (code){
            case SIGNUP_CODE: 
                printf("debug: dentro registrazione\n");           
                reg = signup_c(code, credenziali, server_com);
                su = reg;
                //system("clear");
                break;

            case IN_CODE:
                in = login_c(code, credenziali, server_com, atoi(argv[1]));
                login = in;
                system("clear");
                break;

            default:
                cmd_err = true;
                system("clear");
                break;
    }   
    if(in == true){
        strcpy(OWN_USER, credenziali.username);
        break;  
    } 
    }

/*<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<MENU PRINCIPALE>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>*/
        printf("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<Menu principale>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
        printf("Connesso come %s\n", credenziali.username);
        printf("1)hanging                   -->ricevi i messaggi pendenti quando eri offline\n");
        printf("2)show  username            -->mostra i messaggi inviati da username\n");
        printf("3)chat  username            -->avvia una chat con username\n");
        printf("4)share username file_name  -->condividi il file file_name con tutti gli utenti connessi\n");
        printf("5)out                       -->disconnetti l'utente\n");

    fflush(stdin);

    code = -4;
    client_offline = true;

    while(1){
        readfds = master;
        select(fdmax + 1, &readfds, NULL, NULL, NULL);

        for(i = 0; i <= fdmax; i++){

            if(FD_ISSET(i, &readfds)){
                        fflush(stdin);

                if(i == STDIN){

                    if(client_offline == false && code == 0 && in_chat == true){
                        //devo inviare il messaggio al server
                        printf("Debug: invio messaggio al server\n");
                        fflush(stdin);
                        uint32_t code_t = htonl(MSG_CODE);
                        

                        printf("Path del file: %s\n", path);
                        //chiamo una funzione che stampa il contenuto di un file
                        fgets(buffer, 1024 - 1, stdin);
                        
                        if(access(path, F_OK) == 0){
                            printf("Il file esiste\n");
                            append_msg_c(path, buffer, OWN_USER, true);
                        }
                        else{
                            printf("Il file non esiste\n");
                            append_msg_c(path, buffer, OWN_USER, false);
                        }
                        send(server_com, &code_t, sizeof(uint32_t), 0);
                        send(server_com, &destinatari->username, sizeof(destinatari->username), 0);
                        send(server_com, &buffer, sizeof(buffer), 0);
                        //system("clear");
                        print_chat(path);
                        
                        

                        //printf("CHAT CON %s\n", destinatari);
                        fflush(stdin);
                        break;
                    }
                    if(client_offline == true && code > 0 && in_chat == true){
                        struct destinatari *tmp = destinatari;
                        printf("Debug: invio messaggio al client\n");

                        //devo inviare il messaggio al client
                        fgets(buffer, 1024 - 1, stdin);
                        
                        //invio il messaggio a tutti i destinatari
                        printf("Debug: invio messaggio a tutti i destinatari\n");
                        while(tmp != NULL){
                            //printf("debug: invio messaggio a %s\n", destinatari->username);
                            sprintf(path, "./%s/chat/%s.txt", OWN_USER, destinatari);
                            printf("path: %s\n", path);
                            printf("Destinatario: %s\n", destinatari);
                            if(access(path, F_OK) == 0){
                                printf("Il file esiste\n");
                                append_msg_c(path, buffer, OWN_USER, true);
                            }
                            else{
                                printf("Il file non esiste\n");
                                append_msg_c(path, buffer,OWN_USER, false);
                            }
                            printf("Dopo append, prima di send\n");
                            send(tmp->socket , buffer, strlen(buffer), 0);
                            printf("Dopo send\n");
                            // scorro la lista dei destinatari
                            tmp = tmp->next;                        }
                        break;

                    }

                        fflush(stdin);

                    //è un comando dallo stdin
                    memset(&buffer, 0, sizeof(buffer));
                    memset(&command, 0, sizeof(command));
                    memset(&username, 0, sizeof(username));
                    fgets(buffer, 1024 - 1, stdin);
                    printf("buffer: %s\n", buffer);
                    sscanf(buffer, "%s %s", command, username);
                    printf("comando: %s\n", command);
                    printf("username: %s\n", username);
                    code = cmd_to_code(command);
                    
                    //switch case con tutti i casi per i diversi comandi
                    switch (code){
                        case HANG_CODE:
                            hanging_c(code, server_com);
                            break;

                        case SHOW_CODE:
                            //prelevo username dal buffer
                            //sscanf(buffer, "%s %s", command, username);
                            show_c(code, username, server_com);
                            break;

                        case CHAT_CODE:
                            code = chat_init_c(code, username, server_com);
                            //gestione struct e memoria
                        fflush(stdin);

                            printf("debug: porta = %d\n", code);
                            if (code == -1){
                                client_offline = false;
                                printf("L'utente non esiste\n");
                                break;
                            }
                            if (code == 0){
                                client_offline = false;
                                //system("clear");
                                printf("CHAT VIA SERVER %s\n", destinatari);
                                //sprintf(path, "./%s/chat/%s", OWN_USER, destinatari->username);
                                sprintf(path, "./%s/chat/%s.txt", OWN_USER, destinatari->username);

                                printf("path: %s\n", path);
                                //print_chat(path);
                                //printf("Dopo print_chat\n");
                                in_chat = true;
                                break;
                            }
                            printf("CHAT CON %s\n", destinatari);
                            memset(&client_addr, 0, sizeof(client_addr));
                            client_addr.sin_family = AF_INET;
                            client_addr.sin_port = htons(code);
                            inet_pton(AF_INET, "127.0.0.1", &client_addr.sin_addr);
                            cl_socket = socket(AF_INET, SOCK_STREAM, 0);

                            sprintf(path, "./%s/chat/%s.txt", OWN_USER, destinatari->username);

                            ret = connect(cl_socket, (struct sockaddr*)&client_addr, sizeof(client_addr));
                            in_chat = true;
                            break;

                        case SHARE_CODE:
                            //prelevo username e filename dal buffer
                            sscanf(buffer, "%s %s %s", command, username, filename);
                            //share_c(code, server_com, buffer);
                            break;

                        case OUT_CODE:
                            //il client notifica al server la disconnessione
                            //out_c(code, server_com);
                            exit(0);
                            break;

                        default:
                            printf("+++Comando [%s] non riconosciuto+++\n", command);
                            break;
                    }

                    

                
                    
                } else{
                    if(i == cl_listener){
                        //è una nuova connessione
                        addrlen = sizeof(gp_addr);
                        ret = accept(i, (struct sockaddr*)&gp_addr, (socklen_t*)&addrlen);

                        fdmax = (ret > fdmax) ? ret : fdmax;

                        FD_SET(ret, &master);

                        printf("Nuova connessione \n");
                        
                    } else{
                        //è un messaggio da un client già connesso
                        memset(&buffer, 0, sizeof(buffer));
                        ret = recv(i, buffer, sizeof(buffer), 0);
                        if(ret <= 0){
                            if(ret == 0){
                                printf("Connessione chiusa da %s:%d\n", inet_ntoa(cl_listener_addr.sin_addr), ntohs(cl_listener_addr.sin_port));
                            } else{
                                perror("Errore in fase di ricezione del messaggio");
                            }
                            close(i);
                            FD_CLR(i, &master);
                        } else{
                            printf("è un messaggio da un client già connesso\n");
                            /// è un messaggio ricevuto
                        }
                    }
                }
            }
        }
    }



}