/**
   @file Graphics.h
   @brief Déclaration des types et constantes liées à l'affichage
 */
#ifndef __GRAPHICS_H__
#define __GRAPHICS_H__

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>

/**
   Constantes correspondantes à des composants d'interface utilisateur
 */
enum {
	START_BG, ///< Écran de début
	MENU_BG, ///< du Menu
	MENU_DIALOG, ///< Boite de dialogue
	MENU_ATT, ///< Bouton d'attaque
	MENU_TALK, ///< Bouton de discussion 
	MENU_ITEM, ///< Bouton d'objet
	MENU_MOVE, ///< Bouton de déplacement
	HP_BARRE, ///< Fond de la barre de vie
	HP, ///< Partie remplie de la barre de vie
	INVEN_BG, ///< Fond du Menu d'inventaire
	INVEN_HIGHLIGHT, ///< Décoration du choix séléctionné
	VICTORY, ///< Écran de victoire
	GAME_OVER, ///< Écran de défaite
	NB_TEXTURES ///< Nombre de textures
};

/**
   Définie les différent état d'affichage
 */
enum {
	RENDER_EXPLORATION, ///< État original d'exploration de la carte
	RENDER_INTERACTION, ///< Intération avec un NPC
	RENDER_INVENTORY, ///< Le joueur visionne son inventaire
	RENDER_TALK ///< Le joueur parle avec un NPC
};

/**
 * @struct Graphics_s
 * @brief Structure maintenant une référence vers le contexte
 * d'affichage du jeu ainsi que ses ressources
 */
typedef struct
{
	SDL_Renderer* renderer;
	TTF_Font* font;

	SDL_Texture* texture[NB_TEXTURES];
	SDL_Rect rect[NB_TEXTURES]; 
} Graphics_s;

/// @brief Instance unique de \ref Graphics_s
Graphics_s Graphics;

/// @brief Initialise la variable globale Graphics
void initGraphics();
/// @brief Libère les ressources associées à la variable globale Graphics
void destroyGraphics();

/// @brief Charge une image à partir d'un nom de fichier
void loadImage( char* fileName, SDL_Texture** texture, SDL_Rect* rect );
/// @brief Blit une image dans un rectangle donné
void renderImage( SDL_Texture* texture, SDL_Rect rect );
/// @brief Affiche un texte à une position donnée avec une couleur donnée
void renderText( char* text, int x, int y, SDL_Color color );

/// @brief Récupère les surface clickable
void getButtonRects( SDL_Rect rects[] );

/// @brief Affiche l'écran de début
void renderStartScreen();

/// @brief Affiche le menu du joueur, avec un log des conversation du joueur
void renderMenu( int render_state, char* dialogs[] );
/// @brief Affiche la barre de vie du joueur
void renderHp( int hp_restants, int hp_totaux );

/// @brief Affiche l'image de fond de l'inventaire
void renderInventoryBg();
/// @brief Affiche l'image d'un objet
void renderItem( SDL_Texture* texture, int index );

/// @brief Affiche la quantité d'or du joueur
void renderGold( int gold );
/// @brief Affiche la description d'un objet
void renderItemDesc( char* item_desc );

/// @brief Met en évidence un objet
void renderItemHighlighting( int index );
/// @brief Affiche l'écran de fin du jeu
void renderEnd( int won );
#endif
