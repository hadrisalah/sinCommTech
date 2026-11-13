/*
   Code pour projet SinCommTech
   commServer.c
   Fichier du processus principal de réception des connexions et génération des threads et 
   gestions des événements sur les fichiers xml à monitorer

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include "sinliste.h"
#include "sinnet.h"
#include "sinxml.h"
#include "sinsentinel.h"
#include <sys/socket.h>
#include <sys/select.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <pthread.h>


#define PORT 50000
#define LG_MSG 256

int main(){
// Définition des variables nécessaires à l'établiessement d'un serveur de base
int sock;
int incoming_socket;
int bind_status;
int bind_count;
int size_addresse;
int status_ecoute;
struct sockaddr_in locAddresse, distAddresse;
char *ipClient;
char msg_in[LG_MSG];
char msg_out[LG_MSG];
int ret_read;
int ret_write;
short int portClient;
fd_set master_set, working_set;
int nselset;
int retour_select;
int i; 
psinliste liste;
xmlDocPtr doc;
xmlNodePtr node, cur;
char *nomstation;
char *filePath, *filepathIn, *filepathOut;

/***************************************************************************************************/
/* master_set va contenir le set de seocket à monitorer de façon durable                           */
/* workin_set va contenir le set qui sera manipulé et changer à la volée avec le fonction select() */
/***************************************************************************************************/
// Mise à zéro des set de sockets à monitorer par le système

bind_count=0;
FD_ZERO(&master_set);//FD_ZERO prends en parametre un pointeur sur le set à RAZ
FD_ZERO(&working_set);
nselset=0;//Le nombre de descripteurs à monitorer par la fonction select
		  //A chaque fois qu'une nouvelle connexion apparait il faudra l'incrémenter
		  //A chaque fois qu'une connexion est rompue il faudra la décrémenter

size_addresse=sizeof(struct sockaddr_in);
// création de la socket de base
sock=socket(PF_INET,SOCK_STREAM,0);
if(sock<0){
	// la socket n'a pas été créée il faudra sortir du programme
	printf(">La socket n\'a pas ete creee\n");
	exit(-1);
}
printf(">Socket creee avec succes\n");

// Remplissage de la structure de données contenant l'adresse et le port du serveur
memset(&locAddresse,0x00, sizeof(locAddresse));
locAddresse.sin_family=PF_INET;
locAddresse.sin_addr.s_addr=htonl(INADDR_ANY);
locAddresse.sin_port=htons(PORT);

// Préparation de la liste qui va contenir les données sur les client connectés

liste=new_sinliste();

/**********************************************************************************
*****************Ouverture du fichier de configuration XML*************************
***********************************************************************************/
doc=NULL;
node=NULL;

doc=xmlParseFile("config.xml");
if(doc==NULL){
	// le fichier de configuration est vide
	printf("Le fichier de configuration est vide\n");
	//xmlFreeDoc(doc);
	exit(-1);
}

node=xmlDocGetRootElement(doc);
if(node==NULL){
	// le fichier existe mais aucun élément n'est trouvable
	printf("Le fichier config.xml existe mais reste vide\n");
	//xmlFeeDoc(doc);
	exit(-1);
}



// Attachement de la socket avec la fonction bind()
// Si la socket a bien été liée avec l'appel système bind alors on doit la rajouter au set master_set
// Et incrémenrtation des descripteurs à monitorer par select
//BIND:
//do{
bind_status=bind(sock,(struct sockaddr*)&locAddresse,sizeof(struct sockaddr_in));
if(bind_status<0){
	bind_count++;
	printf(">La reservation du port a echoue\n>Tentative N°:%d",bind_count);		
}
//}while(bind_status<0);
printf(">La reservation du port est un succes\n");
FD_SET(sock,&master_set);
memcpy(&working_set,&master_set, sizeof(master_set));
nselset=sock;
// Mise sur écoute de la socket attachée
// Et établissement d'une queue de 10 messages
//LISTEN:
status_ecoute=listen(sock,10);
if(status_ecoute<0){
 printf(">La mise sur ecoute a echouee\n");
 exit(0);
}
printf(">Serveur en mode ecoute passive\n");
// Réception des connexions depuis les différents clients qui vont s'y attacher
while(1){
	printf(">Attente de connexion\n");
	/*
	test select 
	*/
	//L'appel système select() permet de monitorer un set de descripteur de socket
	//SELECT:

	//retour_select=select(nselset+1,&working_set,NULL,NULL,NULL);
	//if(retour_select>0){
			//printf("L'appel select est réalisé\n le retour est: %d\n la socket attandu est %d\n",retour_select, nselset);
			/*Il y a un signal apparu sur un des éléments du set à monitorer*/
			/*Recherche de l'élément qui est à l'origine de l'enclenchement de select()*/
			//for(i=0;i<=nselset && retour_select>0; i++){
				//printf("boucle for enclenchée l'indice i=%d\n",i);
				// test si le descripteur i est présent dans la liste working_set
				 //if(FD_ISSET(i, &working_set)){
					 //printf("test du working set\n");
						// la socket i est bien sollicitée à cet instant
						// Si la socket i est bien la socket d'écoute sock alors on doit initié une séquence
						// accept() et lancer un thread pour la gestion du client entrant
						 //if(i==sock){
							// un client se connecte pour la première fois
							// lancement de l'appel accept()
							//ACCEPT:
							incoming_socket=accept(sock,(struct sockaddr*)&distAddresse, & size_addresse);
							if(incoming_socket<0){
								printf(">La connexion a ete refusee\n");
								
								exit(-1);
							}
							// Ajout de la nouvelle socket au set monitoré par select
							//FD_SET(incoming_socket,&master_set);
							//memcpy(&working_set,&master_set, sizeof(master_set));
							ipClient=inet_ntoa(distAddresse.sin_addr);
							portClient=ntohs(distAddresse.sin_port);		
							printf(">Demande de connexion accpetee\n\t>Le client demandeur est:%s\n>Num de port:%hu\n",ipClient,portClient);
							cur=node->xmlChildrenNode;// on se positionne sur le premier élément
							while (cur!=NULL)
							{
								if(!xmlStrcmp(xmlGetProp(cur, "addresse"), (xmlChar*)ipClient)){
									// Les deux chaines sont identiques
									filePath=xmlGetProp(cur, "filePath");
									filepathIn=xmlGetProp(cur, "filepathIn");
									filepathOut=xmlGetProp(cur, "filepathOut");
									nomstation=xmlNodeGetContent(cur);
									break;
								}
								cur=cur->next;
							}
							printf("   >Les fichiers de configuration: Path=>%s, Path in=>%s, Path out=>%s\n",filePath, filepathIn, filepathOut);
							sinsentinelspawner(liste, incoming_socket, portClient, ipClient, nomstation, filePath, filepathIn, filepathOut);
						//}
							// lancement de la procédure sinthreader() 
							// Après accept() on doit test si la socket entrante est supérieure à la sock en terme de valeur
						 //if(incoming_socket>sock){nselset=incoming_socket;}
						 //retour_select--;
						//}
						
						 
						//}
			//}
			//break;
		}
	//Gestion des sockets d'entrées sur incoming_socket renvoyées par accept()
	
	

close(sock);
return 0;
}
