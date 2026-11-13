#ifndef _sin_xml_
	#define _sin_xml_
/*Ceci est le fichier header qui va permettre de gérer et de créer des ficjiers xml pour chaque automate afin de le décrire en faisant absytarction de tout ce qui n'intéresse pas l'application
 plusieurs opérations sont rélisées:
#### La création du fichier
#### La lecture d'un élément 
#### L'ecriture dans un élémnt
*/
#include <libxml/parser.h>
#include <libxml/tree.h>

#define PACKETSIZE 4
/*Déclaration des fonctions nécessaires*/

/*Fonction d'ouverture de fichier selon le nom de la station et placement à la racine*/
xmlNodePtr sinOpenXml(const xmlChar* name);

/*Fonction de recherche d'un élément*/
xmlNodePtr sinSeekXml(xmlNodePtr root, const xmlChar* name);

/*Fonction de lecture du contenu d'un élément*/
xmlChar* sinReadXml(const xmlChar *path);

/*Fonction d'écriture du contenu d'un élément, ce qui va écraser l'ancien contenu*/
void sinWriteXml(const xmlChar *path, char* content);

/*Fonction de lecture du contenu d'un élément dans un fichier*/
char* sinReadAsXml(const char *path, char *content);

/*Fonction d'écriture du contenu d'un élément, ce qui va écraser l'ancien contenu*/
void sinWriteAsXml(const char* path, char* content);

#endif
