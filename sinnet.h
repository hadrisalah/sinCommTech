#ifndef _SIN_NET_
	#define _SIN_NET_
	#include <pthread.h>
	#include <stdbool.h>
	#define SINNET_ADDRESSE 0
	#define SINNET_PORT 1
	#define SINNET_THREAD 2
	#define SINNET_SOCKET 3

	typedef struct sinClient
	{
		char *addresse;
		int port;
		int socket;
		pthread_t threadin;
		pthread_t threadout;
		int *flag;
		char *nomStation;
		char *filePath;	
		char *filepathIn;
		char *filepathOut;
		int threadFlag;
		bool isJoined;
		pthread_mutex_t *mtex;
		psinliste liste;
	}sinClient, *psinClient;

	/*******************************************************************************************************/
	/*Les fonctions listées ci-après sont dédiées à compléter le module sinliste.h
	  Le module sinliste.h contient des fonctions relativement basiques pour la manipulation de listes
	  Dont on ne connait pas à l'avance le type de données qu'elles emmagasinent 
	  Nous avons fait le choix de les faire pointer sur des données void pour laisser la flexibilité au
	  développeur de lister tous les types de données  qu'il souhaite, donc pour chercher la position d'une 
	  donnée dans la liste à partir de sa valeur il faut créer des fonctions au cas par cas, ainsi que pour 
	  effectuer la suppression, et l'impression des données à l'écran selon le type de structure de donnée 
	  choisi.*/
	/*******************************************************************************************************/

	/*Fonction qui recherche l'adresse d'une structure dans une liste de type psinliste en partant de son id socket*/
	int findpositionbydata(psinliste *ppliste, int critirea, psinClient ptclient);

	/*Fonction qui recherche l'adresse d'une structure dans une liste de type psinliste en partant de son id socket*/
	int findpositionbyatomicdata(psinliste *ppliste, int critirea, void *data);

	/*alloue un espace mémoire à la structure qui va identifier de façon précise le client, et le rajoute en début de
	 la liste chainée*/
	psinliste identifierclient(psinliste pliste, int socket, char *addresse, int port, char *nomstation);

	/*Fonctions qui suppriment un élément de la liste selon un critère bien donné*/
	int delByParam(psinliste *ppliste, int critirea, psinClient pclient);

	/*Fonction d'impression des données que contient la liste */
	void print_sinliste_sinclient(psinliste pliste);

	/*Fonction d'initialisation d'une structure psinclient*/
	void newpclient(psinClient client,char *addresse, int port, int socket, char *path);

	/*Fonction qui va traverser la liste et se positionner sur un élément en particulier connaissant sa position
	  Pour en extraire la structure psinClient
	*/ 
	psinClient sinextractclient(psinliste liste, int position);

#endif
