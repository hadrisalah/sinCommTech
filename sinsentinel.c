     #include <stdio.h>
     #include <stdlib.h>
     #include <unistd.h>
     #include <string.h>
     #include <stdbool.h>
     #include <limits.h>
     #include <errno.h>
     #include <sys/types.h>
     #include <sys/socket.h>
     #include <sys/inotify.h>
     #include <sys/epoll.h>
     #include <pthread.h>
     #include "sinsentinel.h"
     #include "sinxml.h"     
     #include "sinnet.h"
     
     #define SIZE 256
     #define PACKETSIZE 4
     #define MAXEVENT 1
     
     //psinliste myliste;
    /*
        Fonction d'initiation du monitoring d'un fichier à partir de son chemin.
        Cette fonction retourne un Void.
        Cette fonction doit agir sur un flags de la structure de sinnet.h identifiant 
        chaque flag doit renseigner à tout moment l'état du fichier 'Fermé après écriture'.
        Donc elle doit recevoir en paramètre:
                                            - Le chemin du fichier à monitorer
                                            - Le pointeur sur le flag
                                            - L'identifiant de la socket pour l'envoie des données
    */
   void sinsentinel(psinliste list, char *path, int *flag, int socket, pthread_mutex_t *mtex, pthread_t concurrent){
       int infd; // déclaration du descripteur du fichier de l'instance inotify
       int inwd; // déclaration du descripteur du watcher
       char buffer[sizeof(struct inotify_event)+NAME_MAX+1];
       xmlNodePtr node;
       char *dbContent;//[PACKETSIZE];
       char msg[PACKETSIZE];
       int retsend;// Le retour de la fonction send
       int retlockmut; // le retour de la fonction de verrouillage du mutex
       int position;
       
       flag=malloc(sizeof(int));
       infd=inotify_init();

       

       while(1)
       {  
            inwd=inotify_add_watch(infd,path,IN_CLOSE_WRITE);
            printf("\033[0;31m\t>Prise de mutex, thread d'écriture de de la socket %d, Attente d'écriture\033[0m\n", socket);
            if(read(infd,buffer, sizeof(buffer))<=0){
                // il y a une erreur
                //*flag=-1;
                printf("Impossible de lire sur le fichier '%s' \n", path);
                break;
            }else{
                printf("\033[0;31m\t>La modification du fichier a été détectée\033[0m\n");
                //*flag=1; // il y a une modification dans le fichier qui est monitoré
                // lecture du fichier objet du path et envoie des données sur la socket en argument
                // node est l'élément racine du fichier xml dont le chemin est passé en paramètre
                
                dbContent=sinReadAsXml(path, dbContent);
                printf("\033[0;31m\t>Le contenu à envoyer est : %s\033[0m\n", dbContent);
                // Le contenu à envoyer au DB de l'automate est maintenant lu et enregistré en mémoire dans
                // dbContent
                retlockmut=pthread_mutex_lock(mtex);
                if(retlockmut!=0){
                    printf("\033[0;31m\t>Impossible d'avoir le mutex\033[0m\n");
                }else{
                    printf("\033[0;31m\t>Le thread d'envoi des données possède le mutex\033[0m\n");
                }
                printf("\033[0;31m\t>Envoie de la donnée par appel sys send\033[0m\n");
                //sprintf(msg,dbContent);
                printf("\033[0;31m\t>%s\033[0m\n",dbContent);
                retsend=send(socket, dbContent, strlen(dbContent),0);
                pthread_mutex_unlock(mtex);
                switch (retsend)
                {
                case -1:
                    /* l'envoie a échoué */
                    printf("Envoie des données a échoué\n");
                    break;
                
                case 0:
                    /* la communication s'est arrétée */
                    printf("Envoie des données a échoué, la communication est coupée\n");
                    *flag=-1;
                    pthread_cancel(concurrent);
                    free(dbContent);
                    print_sinliste_sinclient(list);
                    position=findpositionbyatomicdata(&list, SINNET_SOCKET, &socket);
                    if(delbyposition(&list, position)){
                        printf("Thread de lecture arrêté\n");
                    }
                    print_sinliste_sinclient(list);
                    pthread_exit(NULL);
                    break;
                
                default:
                    break;
                }
                
                // en fin de traitement on libère le fichier
            }        
            free(dbContent);
       }
       
   }

   /*
        Fonction qui va surveiller les entrées du réseau provenant d'une station et dont les données 
        doivent être enregistées dans un fichier xml
        Cette fonction doit recevoir en parametre:
                                            - La socket sur laquelle il doit lire
                                            - Le chemin du fichier à écrire
   */
    void sinnetsentinel(psinliste list, int socket, int *flag, char *path, pthread_mutex_t *mtex, pthread_t concurrent){
        char buffer[SIZE];
        int retrecv;// Le retour de la fonction recv
        int retlockmut;
        // variables de gestion des appels systèmes epoll
        int epfd;
        int epoll_ret;
        struct epoll_event ev, events_report[MAXEVENT];
        int nbevent;
        int position;

        //allocation de mémoire pour l'entier flag
        flag=malloc(sizeof(int));

        // Création de l'instance epoll
        epfd=epoll_create1(0);
        // initialisation de la structure struct epoll_event
        ev.events=EPOLLIN;
        ev.data.fd=socket;
        // lancement de l'appel epoll_ctl
        epoll_ret=epoll_ctl(epfd,EPOLL_CTL_ADD,socket, &ev);
        if(epoll_ret!=0) printf("Impossible de configurer le scrutage de la socket\n");

        memset(&buffer,0x00, SIZE*sizeof(char));
        while(1){

            nbevent=epoll_wait(epfd, events_report, MAXEVENT, -1);
            if(events_report[0].data.fd==socket){
                // Il s'agit bien d'un événement de lecture sur la socket
                if(events_report[0].events & EPOLLIN){
                    // on ne peut pas lire sur la socket
                    printf("\t>Prise de mutex, thread de lecture de de la socket %d, Attente de lecture\n", socket);
            retlockmut=pthread_mutex_lock(mtex);
            if(retlockmut!=0){
                    printf("Impossible d'avoir le mutex\n");
                }else{
                    printf("Le thread de lecture réseau possède le mutex\n");
                }
            retrecv=recv(socket, buffer, sizeof(buffer), 0);
            pthread_mutex_unlock(mtex);
            printf("Le thread de lecture lache le mutex\n");
            switch (retrecv)
            {
            case -1:
                printf("Impossible de recevoir les données en provenance de la socket: %i \n", socket);
                break;

            case 0:
                printf("Impossible de recevoir les données en provenance de la socket: %i ; coupure\n", socket);
                *flag=-1;
                printf("Le drapeau que porte le client: %i\n",*flag);
                pthread_cancel(concurrent);
                print_sinliste_sinclient(list);
                position=findpositionbyatomicdata(&list, SINNET_SOCKET, &socket);
                if(delbyposition(&list, position)){
                    printf("Thread de lecture arrêté\n");
                }
                print_sinliste_sinclient(list);
                pthread_exit(NULL);
                break;
            
            default:
                printf("Lecture du paquet réalisée avec succès\nLe texte est:%s\n",buffer);
                
                sinWriteAsXml(path, buffer);
                printf("Ecriture réalisé dans le noeud\n");
                break;
            }
                }
            }
            
            //exit(-1);

        }
    }

    
    /*
        Fonction qui va géner les sentinelles selon les cas
            - Si la socket est nouvelle alors que l'adresse du PLC possède déjà un thread en marche
            - Si la socket est nouvelle alors que l'adresse du PLC ne possède pas de thread en marche
        Cette fonction doit recevoir en paramètre:
                - La socket d'écoute du serveur
                - La liste chainée qui contient les données à ajouter ou à modifier
    */

   void sinsentinelspawner(psinliste liste,int socket, int port, char *addresse, char *nomstation, char *path, char *pathin, char *pathout){
       
       
       psinClient client;
       psindatathread datathreadin, datathreadout;       
       int position;
       int threadinId, threadoutId;

       //myliste=liste;

       /*
         Le fonctionnement est reltivement simple 
         Nous devons créer un maillon de la liste sinliste
         Nous devons le mettre dans la liste chainée
         Nous devons par la suite créer un thread qui va s'occuper de gérer la mise des données
           dans le fichier aloué à cet objectif
       */
 
    
    liste=identifierclient(liste ,socket, addresse, port, nomstation); // Création du maillon et 
                                                                // attachement à la liste
    // Le nouveau maillon créé contient toutes ces informations ainsi q'une allocation en mémoire
    // La fonction identifierclient retourne un pointeur sur le dernier élément ajouté
    // d'un thread pour ce client 
    // Il faut maintenant isoler le thread créé en mémoire et le lancer grace à la fonction
    // pthread_create_t(), la fonction qui s'executera sera sinthreader
    printf("        >Le client a été créé en mémoire\n");
    client=(psinClient)liste->data;
    printf("            >Accès aux données du client\n");
    print_sinliste_sinclient(liste);
    
    // c'est la structure qui contient les threads à lancer et qui représente le client actuel
    // lancement des threads de lecture et ecriture

    // Provisoirement je vais nommer le nom du fichier XML contenant les données des stations 
    // à controler sous le nom: station1.xml
    //                          station1Out.xml
    //                          station1In.xml
    //client->nomStation="station1"; // à changer le plutot
    client->filePath=path;
    client->filepathIn=pathin;
    client->filepathOut=pathout;
    
    printf("       >Les fichiers de config sont:\n%s\n%s\n%s\n", client->filePath, client->filepathIn, client->filepathOut);

    client->mtex=malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(client->mtex, NULL);
    // On prépare le mutex pour les deux threads afin d'isoler l'accès à la socket
    printf(">Mutex créé avec succés\n");
    
    threadinId=pthread_create(&(client->threadin), NULL, sinthreaderin, client);
    if(threadinId!=0){
        // si tout se passe bien alors le retour de pthread_create() à savoir doit être de 0
        // sinon c'est que le thread est introuvable
        pthread_exit(NULL);
    }else{
        printf("Lancmement du premier thread\n");
    }
    
    threadoutId=pthread_create(&(client->threadout), NULL, sinthreaderout, client);
    if(threadoutId!=0){
        // si tout se passe bien alors le retour de pthread_create() à savoir doit être de 0
        // sinon c'est que le thread est introuvable
        pthread_exit(NULL);
    }else{
        printf("Lancmement du second thread\n");
    }
    
    // joiture des threads et annulation des noeuds dans la liste des clients
    /*
        Les deux thread ne doivent pas continuer à fonctionner si le processus principal
        est tué
    */
    
    
    
      
   }

    /*
        Fonction qui va se charger lors de l'execution du thread propre à chaque client
    */
   void *sinthreaderin(void *data){
       // cette fonction doit lancer la lecture des données depuis la socket 
       // puis ecrire dans le fichier
       // ce qui se resume à lancer sinnetsentinel
       psinClient client;
       client=(psinClient)data;
       sinnetsentinel(client->liste,client->socket,client->flag,client->filepathIn, client->mtex, client->threadout);
       print_sinliste_sinclient(client->liste);
   }

   void *sinthreaderout(void *data){
       // cette fonction doit lance l'ecriture vers les client dès que 
       // le fichier géré par le client de supervision
       // ce qui se résume aussi à lancer sinsentinel
       psinClient client;
       client=(psinClient)data;
       sinsentinel(client->liste,client->filepathOut, client->flag, client->socket, client->mtex, client->threadin);
       
   }

