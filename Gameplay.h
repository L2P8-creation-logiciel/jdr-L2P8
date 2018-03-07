/**
 *   \file Gameplay.h
 *   \brief Header du fichier gérant la logique de jeu
 */


#ifndef __GAMEPLAY_H__
#define __GAMEPLAY_H__

#include "Inventory.h"
#include "Npc.h"
#include <SDL2/SDL.h>

/// Nombre d’éléments maximal que le joueur peut avoir d’équipé
#define MAX_STUFF 2

/// Indique l’action courante du joueur
enum {
  STATE_START, ///< Le joueur commence le jeu
  STATE_EXPLORATION, ///< Le joueur est en train de se déplacer
  STATE_INTERACTION, ///< Le joueur interagit avec un NPC
  STATE_INVENTORY, ///< Le joueur consulte son inventaire
  STATE_TALK, ///< Le joueur parle à un NPC
  STATE_WON, ///< Le joueur a gagné
  STATE_LOST ///< Le joueur a perdu
};

/// Indique l’action du personnage du joueur
enum {
  ACTION_ATTACK, ///< Le personnage attaque un NPC
  ACTION_TALK, ///< Le personnage parle avec un NPC
  ACTION_INVENTORY, ///< Le personnage consulte son inventaire
  ACTION_MOVE, ///< Le personnage se déplace
  ACTION_INV_USE, ///< Le personnage tente d’utiliser un objet de son inventaire
  ACTION_INV_EQUIP, ///< Le personnage s’équipe d’un objet de son inventaire
  ACTION_INV_THROW, ///< Le personnage jette un objet de son inventaire
  ACTION_INV_QUIT, ///< Le personnage quitte son inventaire
  ACTION_TALK_YES, ///< Le personnage dit “oui” au NPC avec qui il interagit
  ACTION_TALK_NO, ///< Le personnage dit “non” au NPC avec qui il interagit
  ACTION_TALK_THREAT, ///< Le personnage menace le NPC avec qui il interagit
  ACTION_TALK_QUIT ///< Le personnage arrête de parler au NPC
};

/// Objets que le personnage peut avoir
enum {
  ITEM_NONE, ///< Objet vide
  ITEM_BEER, ///< Bière
  ITEM_CLETTER, ///< Lettre
  ITEM_PLETTER, ///< Lettre
  ITEM_ARMOR, ///< Armure
  ITEM_AXE, ///< Hache
  ITEM_POISON, ///< Poison
  ITEM_CUPCAKE = 203
};

/// Une srtucture pour représenter un objet de l’inventaire
typedef struct {
  int type; ///< Type de l’objet

  char name[30]; ///< Nom de l’objet
  int value; ///< Type de l’objet
  int value2; ///< Identifiant unique de l’objet

  SDL_Rect rect; ///<
  SDL_Texture *tex; ///< Texture de l’objet
} Element;

/// Une structure stockant l’état du jeu
typedef struct {
  int state; ///< État actuel du jeu
  Element *elements; ///< Objets disponibles dans le jeu
  int nb_elements; ///< Nombre d’objets disponibles

  SDL_Rect bg_rect[2]; ///< Canevas pour les images de fond
  SDL_Texture *bg_tex[2]; ///< Images de fond

  int player_max_life; ///< Nombre maximum de points de vie du joueur
  int player_current_life; ///< Nombre actuel de points de vie du joueur
  int player_atk; ///< Points d’attaque du joueur
  int player_def; ///< Points de défense du joueur

  int old_state; ///< État précédent du jeu

  int interaction_index; ///< État d’interaction du joueur

  int *items; ///< Inventaire du joueur
  int stuff[2]; ///< Objets que porte le joueur (armure et arme)

  int selected_item; ///< Objet actuellement sélectionné
  int index_selected_item;

  int gold; ///< Nombre de pièces d’or du joueur
  char name[30]; ///< Nom du joueur

  npc_stats *npcs; ///< Tableau contenant les NPCs du jeu
  int nb_npc; ///< Nombre de NPCs
  int index_current_npc; ///< Identifiant du NPC actif

  int no_leave; ///< Booléen pour si le joueur peut quitter ou non la zone
  int area; ///< Identifiant de la zone où se trouve le joueur
} Gameplay_s;

/// Variable globale pour l’état du jeu
Gameplay_s Gameplay;

/// Initialise l’état du jeu
void initGameplay();

/// Dé-charge une zone
void cleanArea();

/// Charge une zone
void loadArea(int area);

/// Ajoute un élément à la zone actuelle
void addElement(Element elem);

/// Teste si la souris se trouve sur un élément spécifié
int intersects(SDL_Rect rect, int x_mouse, int y_mouse);

/// Teste si un élément de l’interface est activé
int elementTriggered(int x, int y);
/// Teste si un bouton de l’interface est activé
int buttonTriggered(int x, int y, SDL_Rect rects[]);
/// Teste si un objet de l’interface est activé
int itemTriggered(int x, int y, SDL_Rect rects[]);

/// Gère l’interaction avec l’élément activé par le joueur
void processElement(int i);
/// Gère l’action du joueur
void processAction(int i);
/// Gère l’action de l’objet sur le jeu
void processItem(int item_id);

/// Renvoie la texture de l’objet demandé
SDL_Texture *getItemTexture(int i, int inventory);

/// Termine le jeu
void EndGame(int successful);

/// Permet au joueur d’acquérir un objet
void buyItem(int item, int gold);
#endif
