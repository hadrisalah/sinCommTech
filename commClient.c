#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>



#ifdef _WIN32
    #include <winsock2.h>
#elifdef __LINUX__ || __UNIX__
    #include <sys/socket.h>
#endif 

#define PORT 50000
#define LG_MSG 256

int main()
{
    int sock;// variable contenant le descripteur de la socket
    int connexion; // Variable de retour de la fonction connect()
    struct sockaddr_in adresServ; // variable de type structure contenant l'adresse et le num�ro de port du serveur � appeler
    int taille_adresse=sizeof(adresServ);
    int tentative_connexion; // Compteur de tentatives de connexion
    char msg[LG_MSG];// variable contenant les messages � envoyer d'une logueur fixe;
    int sending_status;// valeur de renvoie de la fonction send()
    int receiving_status; // valeur de renvoie de la fonction recv()
    char c; 
        
    
    
    // Mise du compteur de connexion � z�ro
    tentative_connexion=0;
    
    // viadge des cases m�moires contenant le message � envoyer
    memset(&msg, 0x00, sizeof(char)*LG_MSG);
    
    // initialisation des valeurs d'adresse et de port 
    adresServ.sin_family=AF_INET;
    adresServ.sin_port=htons(PORT);
    inet_aton("192.168.1.106", &adresServ.sin_addr);
    printf(">Valeurs de l'adresse du serveur entree\n");
    
    // Cr�ation de la socket	
    sock=socket(AF_INET,SOCK_STREAM,0);
    if(sock<0){
        printf(">Erreur lors d\'initiation de la communication\n");
    }else{
        printf(">Creation de la socket reussie\n");
    }
    
    // Demande de connexion au serveur
    while(tentative_connexion<2){
    	printf("\t>Tentative N:%i\n",tentative_connexion+1);
    	connexion=connect(sock,(struct sockaddr *)&adresServ,taille_adresse);
    if(connexion<0){
    	printf("\t\tLa demande de connexion n\'a pas aboutie\n");
    }else{
    	printf("\t\tLa demande de connexion est acceptee\n");
    	break;
    }
    tentative_connexion++;
    }
    if(tentative_connexion==2 && connexion<0){
    	close(sock);
    	exit(-1);
    }
    
    // La connexion s'est bien pass�e
    // envoie de message au serveur
    // Utilisation de la fonction send()
    // initialisation du message � envoyer

    do{
    
    // Envoi
    sprintf(msg,"Hello socketman\n");
    sending_status=send(sock, msg, strlen(msg),0);
    if(sending_status<=0){
    	if(sending_status==-1){    	
    		printf(">Impossible d\'envoyer le message\n");
    	}
    	if(sending_status==0){
    		printf(">Le serveur a termine la connexion\n");
    	}
    }else{
    	printf(">Message envoye avec succes\n\t>Nombre d\'octets envoyes: %d\n",sending_status);
    }
    
    // Reception
    // Vidage du buffer en m�moire msg
    printf(">Attente de réception réseau\n");
    memset(&msg,0x00, LG_MSG*sizeof(char));
    // attente de r�ception de message
    receiving_status=recv(sock,msg,LG_MSG*sizeof(char),0);
    if(receiving_status<=0){
    	if(receiving_status==-1){    	
    		printf(">Impossible de recevoir le message\n");
    	}
    	if(receiving_status==0){
    		printf(">Le serveur a termine la connexion\n");
    	}
    }else{
    	printf(">Message recu avec succes\n\t>Nombre d\'octets envoyes: %d\n",receiving_status);
    	printf(">Serveur:%s\n",msg);
    }
    
    c=getchar();
    }while(c!='a');   
    // Fermeuture de la socket
    close(sock);    
return 0;
}
