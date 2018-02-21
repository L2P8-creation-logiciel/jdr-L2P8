#ifndef __INVENTORY_H__
#define __INVENTORY_H__

/**
 * \file Inventory.h
 */

#include <SDL2/SDL.h>

/// Nombre maximal d'élément dans l'inventaire du joueur
#define MAX_ITEM 8

/// Quantité d'or en possession du joueur au début de la partie. 
#define START_GOLD 50

/// Enumeration des trois statuts possibles pour chaque objet : auncun, attaque ou défense.
enum{ STAT_NONE, STAT_ATK, STAT_DEF };

/// Structure definissant un objet
typedef struct
{
	char name[ 30 ];          ///< nom de l'objet
	int id;                   ///< identifiant de l'objet
	int value_stat;           ///< valeur du gain offert au joueur, en consommation, en attaque ou en défense, suivant le statut de l'objet.
	int stat;                 ///< statut de l'objet : 0 = aucun / 1 = attaque / 2 = défense.
	int price;                ///< prix de l'objet
	char description[ 300 ];  ///< description de l'objet
	SDL_Texture * tex;        ///< texture de l'objet
} Item;

/// Tableau référençant tout les objets du jeu.
Item* Items;

/// Nombre d'objets du jeu.
int NbItems;

/// \brief Initialise les variables globales \ref Items et \ref NbItems suivant le contenu du fichier `"Data/equipement.txt"`
void initItems();
/// \brief Libère la mémoire de chaque élément du tableau global \ref Items puis libère le pointeur sur \ref Items 
void closeItems();
/// \brief Initialise l'inventaire du joueur \ref InvJoueur
int * inventoryInit ();
/// \brief Accède à l'objet d'identifiant `id` dans l'inventaire du joueur \ref InvJoueur.
Item* getItemFromID(int id);
/// \brief Ajoute un objet à l'inventaire du joueur 
int * inventoryAdd(int id_obj);
/// \brief Modifie la réserve d'or du joueur. 
int inventoryAddGold(int gold_more);
/// \brief Retir un objet à l'inventaire du joueur 
int * inventoryDel(int id_obj);
/// \brief Modifie la réserve d'or du joueur. 
int inventoryDelGold(int gold_less);
/// \brief Prédicat indiquant si le joueur a suffisamment d'or pour obtenir un certain objet.
int enoughtGold(int id_obj);
/// \brief Affiche une description de l'équipement.
void equipementContenu();
/// \brief Demande à l'utilisateur quel objet il souhaite équiper.
void equiper();
/// \brief Demande à l'utilisateur quel objet il souhaite déséquiper.
void desequiper();

/// \brief Fonction non définie.
int getGold();

/// \brief Remplace chaque caractère '_' par un espace dans la chaîne de caractères passée en argument. 
void removeUnderscore( char* description );

#endif
