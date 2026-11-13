#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pthread.h>
#include "sinliste.h"
#include "sinnet.h"

/*
Fichier contenant les déscriptions et corps des fonctions 
utiles à la manipualtion des données des clients réseau
à enregistrer dans une liste chainée
*/

/*
Fonction qui recherche l'adresse d'une structure dans une 
liste de type psinliste en partant de son id socket
*/
	int findpositionbydata(psinliste *ppliste,int critirea, psinClient ptclient){
		psinliste pliste=*ppliste;
		int position=0;
		psinliste tmp=pliste;
		psinClient ptdata;
		ptdata=NULL;
		switch(critirea){

			case SINNET_ADDRESSE:
				// Dans ce cas la donnée à rechercher est une Adresse sous forme d'une chaine de caractères
				while(tmp){
					ptdata=tmp->data;
					if(ptdata->addresse==ptclient->addresse){
						return position;
					}
					tmp=tmp->nxt;
					position++;
				}
			case SINNET_PORT:
				// Dans ce cas la donnée à rechercher est un numéro de port de client sous forme d'un entier
				while(tmp){
					ptdata=tmp->data;
					if(ptdata->port==ptclient->port){
						return position;
					}
					tmp=tmp->nxt;
					position++;
				}
			case SINNET_SOCKET:
				// Dans ce cas la donnée à rechercher est un identifiant de socket sous forme d'un entier
				while(tmp){
					ptdata=tmp->data;
					if(ptdata->socket==ptclient->socket){
						return position;
					}
					tmp=tmp->nxt;
					position++;
				}
			
			default:
				printf("Critère de recherche invalide\n");
				return -1;
		}
	}

/*alloue un espace mémoire à la structure qui va identifier de façon précise le client, et le rajoute en début de
	 la liste chainée*/
	psinliste identifierclient(psinliste pliste, int socket, char *addresse, int port, char *nomstation){
		psinClient ptclientinfo=(psinClient)malloc(sizeof(sinClient));
		ptclientinfo->addresse=addresse;
		ptclientinfo->port=port;
		ptclientinfo->socket=socket;
		ptclientinfo->nomStation=nomstation;
		
		//ptclientinfo->mtex=malloc(sizeof(pthread_mutex_t));

		pliste=addelement(pliste, ptclientinfo);
		ptclientinfo->liste=pliste;
		return pliste;
	}

/*Fonctions qui suppriment un élément de la liste selon un critère bien donné*/
	int delByParam(psinliste *ppliste, int critirea, psinClient pclient){
		psinliste pliste=*ppliste;
		int position=0;
		position=findpositionbydata(ppliste, critirea, pclient);
		if(position==-1)
			return position;
		if(delbyposition(ppliste, position))
			return 0;
		return -2;
	}

/*Fonction d'impression des données que contient la liste */
	void print_sinliste_sinclient(psinliste pliste){
		
		
		psinClient donnee;
		if(isempty(pliste)){
			printf("La liste est vide.\n");
		}else{
			while(!isempty(pliste)){
				donnee=(psinClient)pliste->data;
				printf("Station: %s\nAddresse: %s\nPort: %hu\nSocket: %i\n",donnee->nomStation, donnee->addresse, donnee->port, donnee->socket);
				printf("--------------------------------\n");
				printf("   |\n");
				printf("   |\n");
				printf("   V\n");
				printf("--------------------------------\n");
				pliste=pliste->nxt;
			}
		}

	}

	/*
Fonction qui recherche l'adresse d'une structure dans une 
liste de type psinliste en partant de son id socket
*/
	int findpositionbyatomicdata(psinliste *ppliste,int critirea, void *data){
		psinliste pliste=*ppliste;
		int position=0;
		psinliste tmp=pliste;
		psinClient ptdata;
		ptdata=NULL;
		switch(critirea){

			case SINNET_ADDRESSE:
				// Dans ce cas la donnée à rechercher est une Adresse sous forme d'une chaine de caractères
				while(tmp){
					ptdata=tmp->data;
					if(ptdata->addresse==(char*)data){
						return position;
					}
					tmp=tmp->nxt;
					position++;
				}
			case SINNET_PORT:
				// Dans ce cas la donnée à rechercher est un numéro de port de client sous forme d'un entier
				while(tmp){
					ptdata=tmp->data;
					if(ptdata->port==*((int *)data)){
						return position;
					}
					tmp=tmp->nxt;
					position++;
				}
			case SINNET_SOCKET:
				// Dans ce cas la donnée à rechercher est un identifiant de socket sous forme d'un entier
				while(tmp){
					ptdata=tmp->data;
					if(ptdata->socket==*((int *)data)){
						return position;
					}
					tmp=tmp->nxt;
					position++;
				}
			
			default:
				printf("Critère de recherche invalide\n");
				return -1;
		}
	}

	/*Fonction d'initialisation d'une structure ptclient*/
	void newpclient(psinClient client, char *addresse, int port, int socket, char *path){
		client->addresse=addresse;
		client->filePath=path;
		client->port=port;
		client->socket=socket;
	}


	/*Fonction qui va traverser la liste et se positionner sur un élément en particulier connaissant sa position
	  Pour en extraire la structure psinClient
	*/ 
	psinClient sinextractclient(psinliste liste, int position){
		int i;
		psinClient client;
		i=0;
		while(i<position){
			liste=liste->nxt;
			i++;
		}

		client=(psinClient)liste->data;
		return client;
	}