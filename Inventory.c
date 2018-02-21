/**
 * @file Inventory.c
 * Gestion de l'inventaire et de l'équipement du joueur :\n
 * - Définition de l'ensemble @ref Items des objets du jeu à partir du fichier
 * `"Data/equipement.txt"`.\n
 * - Gestion dynamique de l'inventaire @ref InvJoueur du joueur.\n
 * - Gestion dynamique de l'équipement @ref EquipementCourant du joueur.\n
 */

#include "Inventory.h"
#include "Graphics.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

/** type entier non signé qui n'est jamais utilisé. */
typedef unsigned int uint;

/** 
 * Tableau de deux entiers représentant l'équipement courant du joueur. La
 * première case doit contenir l'identifiant d'un objet de type attaque, et la
 * seconde, l'identifiant d'un objet de type défense.\n
 * Les identifiants par défaut (24 et 18) ne correspondent à aucun objet dans
 * le fichier `"Data/equipement.txt"`.
 */
int EquipementCourant[2]={24,18};

/** 
 * Inventaire du joueur.\n
 * Tableau statique de 8 valeurs contenant les identifiants des objets détenus
 * par le joueur.
 */
int InvJoueur[8];
/** Inventaire du marchand d'arme.\n
	Tableau statique de 8 valeurs, jamais utilisé **/ 
int InvMarchandArme[8] = {20, 21, 22, 23, 19, 0, 0, 0};
/** Inventaire du marchand d'armure.\n
	Tableau statique de 8 valeurs, jamais utilisé **/ 
int InvMarchandArmure[8] =  {16, 17, 18, 0, 0, 0, 0, 0};

/** Réserve d'or du joueur */
int Gold;

/** 
 * Modifie les variables globales @ref Items et @ref NbItems :\n
 * - @ref NbItems reçoit le nombre de lignes du fichier `"Data/equipement.txt"`\n
 * - Le tableau Items est alloué.\n
 * - Chaque élément du tableau est initialisé aux valeurs définies à la ligne
 * corespondante du fichier `"Data/equipement.txt"`, suivant le format [nom]
 * [id] [value_stat] [stat] [price] [description].\n
 * Si @ref Items est déjà alloué avant l'appel à la fonction @ref initItems(),
 * une fuite de mémoire se produit.\n
 * L'accès au fichier `"Data/equipement.txt"` n'est pas sécurisé et il n'y a
 * pas de gestion d'erreurs. C'est pourquoi chaque ligne du fichier
 * `"Data/equipement.txt"` doit respecter scupuleusement le format demandé.
 */
void initItems()
{
	FILE* file = fopen( "Data/equipement.txt", "r" );
	if( !file )
	{
		printf( "equipement.txt not found" );
		return;
	}

	NbItems = 0;
	Items = NULL;

	while( !feof( file ) )
	{
		Item item;
		fscanf( file, "%s %d %d %d %d %s\n", item.name, &item.id, &item.value_stat, &item.stat, &item.price, item.description );
		removeUnderscore( item.description );

		SDL_Rect rect;
		loadImage( item.name, &item.tex, &rect );

		Items = realloc( Items, sizeof( *Items ) * ( NbItems + 1 ) );
		Items[ NbItems ] = item;

		item.tex = NULL;

		NbItems++;
	}

	fclose( file );
}

/** 
 * Libère le champs `.tex` de chacune des structures @ref Item du tableau @ref
 * Items, puis libère le pointeur sur @ref Items.
 */
void closeItems()
{
	int i;
	for( i = 0; i < NbItems; i++ )
		SDL_DestroyTexture( Items[ i ] .tex );

	free( Items );
}

/** 
 * Initialise l'inventaire du joueur @ref InvJoueur.
 * @return Pointeur @ref InvJoueur sur l'inventaire du joueur
 */
int * inventoryInit () {
	int incrementation;                        // pas utilisé
	memset(InvJoueur, 0, sizeof(InvJoueur));
	Gold = START_GOLD;
	return InvJoueur;
}

/** 
 * Cherche l'objet d'identifiant `id` dans l'inventaire du joueur @ref
 * InvJoueur. Produit une erreur `assert(0)` si l'objet demandé n'appartient
 * pas à l'inventaire.
 * @param id : l'identifiant d'un objet
 * @return pointeur sur l'objet d'indentifiant `id`.
 */
Item* getItemFromID(int id)
{
	int i;                           // nom...
	for( i = 0; i < NbItems; i++ )
	{
		if( id == Items[ i ].id )
		{
			return &Items[ i ];
		}
	}

	printf( "Item missing %d\n", id );
	assert( 0 );
}

/** 
 * Modifie la chaîne de caractères `description` passée en argument en
 * remplaçant chaque caractère '_' par un espace. Cette fonction est de
 * complexité quadratique quelque soit son argument.
 * @param description : une chaîne de caractère quelconque.
 */

void removeUnderscore( char* description )
{
	int i;
	for( i = 0; i < strlen( description ); i++ )
	{
		if( description[ i ] == '_' )
			description[ i ] = ' ';
	}
}



/** 
 * Modifie l'inventaire du joueur @ref InvJoueur en lui ajoutant l'objet
 * d'identifiant `id_obj`. Si l'inventaire est plein, l'objet n'est pas ajouté
 * et un affichage le signale. Si la constante @ref MAX_ITEM est supérieure à
 * 8, un débordement de mémoire se produira.
 * @param id_obj : identifiant de l'objet à ajouter à l'inventaire.
 * @return pointeur @ref InvJoueur sur l'inventaire du joueur
 */
int * inventoryAdd(int id_obj) {
	int incrementation, plein =0;
	
	for (incrementation = 0; incrementation < MAX_ITEM; incrementation ++) {
		if (InvJoueur[incrementation] == 0) {
			InvJoueur[incrementation] = id_obj;
			return InvJoueur;
		}
		else plein ++;
		if (plein == MAX_ITEM){
			printf("Ton inventaire est plein. What a shame! Supprime un objet ou oublie\n");
			return InvJoueur;
		}
	}

	 return InvJoueur;
}

/** 
 * Modifie la variable globale @ref Gold en lui ajoutant la valeur de son
 * argument `gold_more`, laquelle peut être négative.
 * @param gold_more : la quantité d'or à ajouter à @ref Gold
 * @return la nouvelle valeur de la variable @ref Gold
 */
int inventoryAddGold(int gold_more) {
	Gold += gold_more;
	return Gold;
}

/** 
 * Modifie l'inventaire du joueur @ref InvJoueur en lui retranchant l'objet
 * d'identifiant `id_obj`. Le cas échéant, un affichage signale que l'objet
 * `id_obj` n'apparaît pas dans @ref InvJoueur. Si la constante @ref MAX_ITEM
 * est supérieur à 8, un débordement de mémoire se produira.
 * @param id_obj : identifiant de l'objet à retirer à l'inventaire.
 * @return Pointeur @ref InvJoueur sur l'inventaire du joueur
 */
int * inventoryDel(int id_obj) {
	int incrementation, present;
	
   for(incrementation = 0; incrementation < MAX_ITEM; incrementation ++) {
	  if (InvJoueur[incrementation] == id_obj) {
	  	InvJoueur[incrementation] = 0;
		if (id_obj == EquipementCourant[0]) EquipementCourant[0] = 0;
		if (id_obj == EquipementCourant[1]) EquipementCourant[1] = 0;
	  	return InvJoueur;
	  }
	  else present ++;
	}
	if (present == MAX_ITEM) {
		printf("Ton inventaire ne contient pas l'objet\n");
	 	return InvJoueur;
	 }
return InvJoueur;
}

/** 
 * Modifie la variable globale @ref Gold en lui retranchant la valeur de son
 * argument `gold_less`, laquelle peut être négative.
 * @param gold_more : la quantité d'or à retrancher à @ref Gold
 * @return la nouvelle valeur de la variable @ref Gold
 */
int inventoryDelGold(int gold_less) {
	Gold -= gold_less;
	return Gold;
}


/**
  * Teste si la valeur de @ref Gold est supérieure au prix de l'objet
  * d'identifiant `id_obj`
  * @param id_obj : identifiant d'un objet
  * \return 
  * - `1` si la valeur de @ref Gold est supérieure au prix de l'objet
  * d'identifiant `id_obj`\n
  * - `-1` en cas d'erreur\n
  * - `0` sinon.\n
  */
int enoughtGold(int id_obj) {
	int incrementation;
	char nom[15], type[4];
	int equi, ajout_stat, temporaire, cout;
	FILE * equipement;
	equipement=fopen("equipement.txt", "r+");
	if(equipement == NULL) 
   {
      printf("Error in opening file");
      return -1;
   }
    while (!feof(equipement)) {
		fscanf(equipement, "%s %d %d %s %d\n", nom, &equi, &ajout_stat, type, &cout);
		if (id_obj == equi) {
			if (cout < Gold) {
				inventoryDelGold(cout);
				return 1;
			}
			else return 0;
		}
	}
}

/**
  * Affiche une description de l'équipement @ref EquipementCourant du joueur.
  * Les deux entiers du tableau global @ref EquipementCourant sont les
  * identifiants de deux objets décrits dans le fichier `"equipement.txt"`.
  * Lors de l'appel à cette fonction, le fichier "equipement.txt" est ouvert
  * en mode lecture et écriture. Si l'ouverture du fichier échoue, un affichage
  * le signale.
  */
void equipementContenu() {
	int incrementation;
	int id_obj;
	char nom[15], type[4];
	int equi, ajout_stat, cout;
	FILE * equipement;
	equipement=fopen("equipement.txt", "r+");
	if(equipement == NULL) 
   {
      printf("Error in opening file");
      return ;
   }
	while (!feof(equipement)) {
		fscanf(equipement, "%s %d %d %s %d\n", nom, &equi, &ajout_stat, type, &cout);
 		if (EquipementCourant[0] == equi) printf("Vous êtes équipé d'un(e) %s qui rajoute %d en %s (id : %d)\n", nom, ajout_stat, type, equi);
		if (EquipementCourant[1] == equi) printf("Vous êtes équipé d'un(e) %s qui rajoute %d en %s (id : %d)\n", nom, ajout_stat, type, equi);
	}
}

/** 
 * Modifie le tableau global @ref EquipementCourant.\n
 * Demande à l'utilisateur l'identifiant de l'objet qu'il souhaite prendre en
 * main, via l'entrée standard (non protégée) :\n
 * - Si l'objet désiré est de type "attaque", l'identifiant de l'objet est
 * affecté à la première case du tableau.\n
 * - Si l'objet désiré est de type "défense", l'identifiant de l'objet est
 * affecté à la deuxième case du tableau.\n
 * - Si le tableau `EquipementCourant` contient déjà l'identifiant d'un objet
 * de type "attaque" ou "défense", l'utilisateur est invité à confirmer qu'il
 * souhaite en effet remplacer cet objet par le nouveau, en saissisant via
 * l'entrée standard (non protégée), le nombre `1` (pour Oui) ou le nombre `2`
 * (pour Non).
 */
void equiper() {
	int reponse;
	char * type_att = "att";
	char * type_def = "def";
	int incrementation;
	int id_obj;
	char nom[15], type[4];
	int equi, ajout_stat, temporaire, cout;
	FILE * equipement;
	equipement=fopen("equipement.txt", "r+");
	if(equipement == NULL) 
   {
      printf("Error in opening file");
      return ;
   }
	printf("Entrez un equipement que vous voulez equiper (vous ne pouvez avoir qu'une seule arme et une seule armure d'equiper en meme temps):");
	scanf("%d", &id_obj);
    while (!feof(equipement)) {
		fscanf(equipement, "%s %d %d %s %d\n", nom, &equi, &ajout_stat, type, &cout);
		for (incrementation = 0; incrementation < MAX_ITEM; incrementation ++) {
	 		if (equi == id_obj && !strcmp(type, type_att)) {
				if (EquipementCourant[0] == 0) EquipementCourant[0] = id_obj;
				else {
					printf("Il y a deja un objet en attaque. Le remplacer? (1 pour oui, 2 pour non) \n");
					scanf("%d", &reponse);
					if (reponse == 1) {
						EquipementCourant[0] = id_obj;
						return;
					}
					else return;
				}
			}
			if (equi == id_obj  && !strcmp(type, type_def)) {
				if (EquipementCourant[1] == 0) {
					EquipementCourant[1] = id_obj;
					return;
				}
				else {
					printf("Il y a deja un objet en defense. Le remplacer? (1 pour oui, 2 pour non) \n");
					scanf("%d", &reponse);
					if (reponse == 1) {
						EquipementCourant[1] = id_obj;
						return;
					}
					else return;
				}
			}
		}
	}
} 

/** 
 * Modifie le tableau global @ref  EquipementCourant.\n
 * Invite l'utilisateur en déséquiper un objet en saisissant l'identifiant de
 * l'objet via l'entrée standard (non protégée).\n
 * - Si l'identifiant saisi égale la valeur de la première case du tableau, le
 * contenu de cette case est remplacé par l'entier 0.\n
 * - Si l'identifiant saisi égale la valeur de la deuxième case du tableau, le
 * contenu de cette case est remplacé par l'entier 0.\n
 * - Si l'identifiant saisi n'apparaît pas dans le tableau, un affichage le
 * signale.
 */
void desequiper() {
	int id_obj;
	printf("Entrez un equipement que vous voulez desequiper :");
	scanf("%d", &id_obj);
	if(EquipementCourant[0] == id_obj) { EquipementCourant[0] = 0; return; }
	if(EquipementCourant[1] == id_obj) { EquipementCourant[1] = 0; return; }
	else printf("vous n'avez pas cet objet equipé");
}


// ===============================================================================
// Pas utilisé 																										*/
// ===============================================================================

/*int * inventoryContenu(int npc) {
	int incrementation;
	char nom[15], type[4];
	int equi, ajoutStat, temporaire, cout;
	FILE * equipement;
	equipement=fopen("equipement.txt", "r+");
	if(equipement == NULL) 
   {
      printf("Error in opening file");
      return(invJoueur);
   }
   if (npc == 0) {
	for (incrementation = 0; incrementation < MAX_ITEM; incrementation ++) {
		rewind(equipement);
		while (!feof(equipement)) {
			fscanf(equipement, "%s %d %d %s %d\n", nom, &equi, &ajoutStat, type, &cout);
			temporaire = invJoueur[incrementation];
	 		if (temporaire == equi) printf("Vous avez un(e) %s qui rajoute %d %s (id : %d)\n", nom, ajoutStat, type, equi);
	 	}
	 }
	printf("Vous avez %d d'or", gold);
   }
   else if(npc == 1) {
	for (incrementation = 0; incrementation < MAX_ITEM; incrementation ++) {
		rewind(equipement);
		while (!feof(equipement)) {
			fscanf(equipement, "%s %d %d %s %d\n", nom, &equi, &ajoutStat, type, &cout);
			temporaire = invMarchandArme[incrementation];
	 		if (temporaire == equi) printf("Le marchand a un(e) %s qui rajoute %d %s au prix %d (id : %d)\n", nom, ajoutStat, type, cout, equi);
	 	}
	 }
   }
   else if(npc == 2) {
	for (incrementation = 0; incrementation < MAX_ITEM; incrementation ++) {
		rewind(equipement);
		while (!feof(equipement)) {
			fscanf(equipement, "%s %d %d %s %d\n", nom, &equi, &ajoutStat, type, &cout);
			temporaire = invMarchandArmure[incrementation];
	 		if (temporaire == equi) printf("Le marchand a un(e) %s qui rajoute %d %s au prix %d (id : %d)\n", nom, ajoutStat, type, cout, equi);
	 	}
	 }
   }
   else printf("Ceci n'est pas un npc correct");
   
	 fclose(equipement);
	 return invJoueur;
}*/



/*void main() {
	int choix = 0;
	int idObj, incrementation;
	int choixED;
	inventoryInit();
	while (choix != 1 && choix !=2 && choix != 3 && choix != 4 && choix !=5) {
		boucle:
		printf("\n Veuillez choisir votre action: achat arme, jeter, afficher, quitter, equiper ou desequiper : (1, 2, 3, 4, 5): ");
		scanf("%d", &choix);
	}
	if (choix == 5) {
		printf("Entrez 1 pour equiper ou 2 pour desequiper:");
		scanf("%d", &choixED);
		if(choixED == 1) {
			equiper();
			equipementContenu();
			printf("\n");
		}
		else {
			desequiper();
			equipementContenu();
			printf("\n");
		}
		goto boucle;
	}
	if (choix == 3) {
		inventoryContenu(0);
		printf("\n");
		goto boucle;
	}
	if(choix == 1) {
		inventoryContenu(1);
		printf("Entrez le num de l'objet à acheter :");
		scanf("%d", &idObj);
		if (enoughtGold(idObj) == 1) {
			inventoryAdd(idObj);
			printf("\n");
			inventoryContenu(0);
			printf("\n");
			for(incrementation = 0; incrementation < MAX_ITEM; incrementation ++) 
				if (invMarchandArme[incrementation] == idObj)	invMarchandArme[incrementation] = 0;
			goto boucle;
		}
		else {
			printf("vous n'avez pas assez d'argent. Vous avez seulement %d", gold);
			goto boucle;
		}
	}
	if(choix == 2) {
		printf("Entrez le num de l'objet à supprimer (entre 14 et 26 et pour l'or, mettre 1 puis la quantité):");
		scanf("%d", &idObj);
		if (idObj == 1) {
			scanf("%d", &idObj);
			inventoryDelGold(idObj);
		}
		else inventoryDel(idObj);
		printf("\n");
		inventoryContenu(0);
		printf("\n");
		goto boucle;
	}
	if(choix == 4) {
		return;
	}
}
*/
