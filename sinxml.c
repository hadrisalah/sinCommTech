
#include "sinxml.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define PACKETSIZE 4


/*Fichier contenant les défintions des fonctions dont le prototype est déclaré dans sinxml.h*/

/*Fonction d'ouverture de fichier selon le nom de la station et placement à la racine*/
/*En cas d'erreur d'ouverture du fichier ou d'indisponibilité d'un élément racine le retour sera NULL*/
xmlNodePtr sinOpenXml(const xmlChar* name){

    xmlNodePtr root;
    xmlDocPtr doc;

    // les pointeurs sont initialiés à NULL
    root=NULL;
    doc=NULL;

    if((doc=xmlParseFile(name))==NULL){
        return NULL;
    }

    if((root=xmlDocGetRootElement(doc))==NULL){
        return NULL;
    }

    return root;
}


/*Fonction de recherche d'un élément à partir de son nom*/
/*Dans le cas où le noeud recherché n'est pas retrouvé alors la fonction retourne NULL*/
xmlNodePtr sinSeekXml(xmlNodePtr root, const xmlChar* name){

    xmlNodePtr node;
    node=NULL;
    node=root->xmlChildrenNode;
    while(node!=NULL){
        if(node->name==name){
            // nous avons trouvé l'élément recherché
            return node;
        }
        node=node->next;
    }
    return NULL;
}

/*Fonction de lecture du contenu d'un élément*/
xmlChar* sinReadXml(const xmlChar *path){

    xmlDocPtr doc;
    xmlNodePtr node;
    xmlChar *content;
    content=NULL;
    
    printf("\033[0;31m\t>Traitement de lecture de fichier\033[0m\n");
    if((doc=xmlParseFile(path))==NULL){
        printf("Ouverture du document: '%s' est impossible", path);
        return NULL;
    }

    if((node=xmlDocGetRootElement(doc))==NULL){
        printf("Impossible d'ouvrir l'élément root du document: '%s'\n", path);
        return NULL;
    }

    content=xmlNodeGetContent(node);
    xmlFreeDoc(doc);
    return content;
}

/*Fonction d'écriture du contenu d'un élément, ce qui va écraser l'ancien contenu*/
void sinWriteXml(const xmlChar *path, char *content){

    xmlNodePtr node, root;
    xmlDocPtr doc;
    xmlChar *str;
    
    printf("\t>Ecriture du buffer dans le fichier xml: '%s'\n", path);
    if((doc=xmlParseFile(path))==NULL){
        printf("Ouverture du document: '%s' est impossible", path);
    }

    if((root=xmlDocGetRootElement(doc))==NULL){
        printf("Impossible d'ouvrir l'élément root du document: '%s'\n", path);
    }

    if((node=root->xmlChildrenNode)==NULL){
        printf("Impossible d'ouvrir l'élément node du document: %s\n", path);
    }
    str = xmlEncodeEntitiesReentrant(doc, BAD_CAST content);
    xmlNodeSetContent(node, BAD_CAST str);
    printf("Ecriture terminée de: %s dans le fichier: '%s' au noeud: %s\n", str, path, node->name);
    xmlFreeDoc(doc);
    
}

/*Fonction de lecture du contenu d'un élément dans un fichier*/
char* sinReadAsXml(const char *path, char *content){
    
        
    FILE *fp;
    char *lecture;
    char c;
    int i;

    fp=NULL;
    i=0;

    content=malloc(sizeof(char)*PACKETSIZE);
    printf("\033[0;31m\t>Traitement de lecture de fichier\033[0m\n");

    fp=fopen(path, "r");
    if(fp==NULL){
        printf("Impossible d'ouvrir ce fichier en lecture\n");
    }
    printf("\033[0;31m\t>Le fichier out est ouvert\033[0m\n");
    //fscanf(fp,"%s",content);
    //fgets(content,PACKETSIZE+20, fp);
    
    while ((c=fgetc(fp))!=EOF)
        {
            if(i>8 && i<13)
            {
                strncat(content,&c,1);
                //printf("%c",c);
            }            
            i++;
        }    
    
    printf("\033[0;31m\t>Le contenu retrouvé est %s\033[0m\n", content);
    i=0;
    fclose(fp);
    //strcpy(content,lecture);
    return content;
}

/*Fonction d'écriture du contenu d'un élément, ce qui va écraser l'ancien contenu*/
void sinWriteAsXml(const char* path, char* content){
    FILE *fp;

    fp=NULL;

    fp=fopen(path,"w");
    if(fp==NULL){
        printf("Impossible d'ouvrir ce fichier en écriture\n");
    }

    fprintf(fp, "<in>\n<DB>\n%s</DB>\n</in>",content);

    fclose(fp);
}