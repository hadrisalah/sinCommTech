     
#ifndef _MYSIN_SENTINEL_
    #define _MYSIN_SENTINEL_
    #include "sinliste.h"

    /*
        Structure de donnée à envoyer aux fonctions qui seront executées dans chaque thread
    */

    typedef struct datathread{
        int socket;
        char *path;
        int flag;
        pthread_mutex_t *mtex;        
    }sindatathread, *psindatathread;


    /*
        Fonction d'initiation du monitoring d'un fichier à partir de son chemin.
        Cette fonction retourne un Void.
        Cette fonction doit agir sur un flags de la structure de sinnet.h identifiant 
        chaque flag doit renseigner à tout moment l'état du fichier 'Fermé après écriture'.
        Donc elle doit recevoir en paramètre:
                                            - Le chemin du fichier à monitorer
                                            - Le pointeur sur le flag
    */
   void sinsentinel(psinliste list, char *path, int *flag, int socket, pthread_mutex_t *mtex, pthread_t concurrent);
   /*
        Fonction qui va surveiller les entrées du réseau provenant d'une station et dont les données 
        doivent être enregistées dans un fichier xml
        Cette fonction doit recevoir en parametre:
                                            - La socket sur laquelle il doit lire
                                            - Le chemin du fichier à écrire
   */
    void sinnetsentinel(psinliste liste, int socket, int *flag, char *path, pthread_mutex_t *mtex, pthread_t concurrent);

    /*
        Fonction qui va géner les sentinelles selon les cas
            - Si la socket est nouvelle alors que l'adresse du PLC possède déjà un thread en marche
            - Si la socket est nouvelle alors que l'adresse du PLC ne possède pas de thread en marche
        Cette fonction doit recevoir en paramètre:
                - La socket d'écoute du serveur
                - La liste chainée qui contient les données à ajouter ou à modifier
    */

   void sinsentinelspawner(psinliste liste, int socket, int port, char *addresse, char *nomstation, char *path, char *pathin, char *pathout);
    
    

    /*
        Fonction qui va s'executer dans le thread de chaque client connecté
    */

   void *sinthreaderin(void *data);

    /*
        Fonction qui va s'executer dans le thread de chaque client connecté
    */
   void *sinthreaderout(void *data);

#endif
