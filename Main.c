
/**
 * @file Main.c
 * Point d'entrée du programme. Alloue et détruit les ressources, et gère les
 * évènements utilisateur.
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>

#include <SDL2/SDL.h>

#include "Graphics.h"
#include "Gameplay.h"
#include "Inventory.h"
#include "Npc.h"

/// Largeur de la fenêtre 
#define WINDOW_WIDTH 800
/// Hauteur de la fenêtre
#define WINDOW_HEIGHT 600

/// @brief Ouvre la SDL et construit la fenêtre.
SDL_Window* initSDL();
/// @brief Ferme la SDL et libère ses ressources.
void closeSDL( SDL_Window* window );
/// @brief affiche le nombre d'images par seconde (fps).
void renderFramerate();

/// file des dialogues entre le joueur et les personnages du jeu.
char* DialogsQueue[ 3 ];

/**
 * @brief Initialisation du jeu, interaction avec l'utilisateur et libération
 * des ressources avant la fin d'exécution du programme.\n
 * - Initialise les différents modules par appel aux fonctions @ref initSDL,
 * @ref initGraphics, @ref initGameplay et @ref initItems. Alloue 3 buffers de
 * 1024 caractères dont les pointeurs respectifs sont stockés dans @ref
 * DialogsQueue.\n
 * - Une boucle d'interaction capture les événements utilisateurs (clavier et
 * souris) et modifie en conséquence les variables globales définissant l'état
 * courant du jeu (dont, en particulier, la structure @ref Gameplay).\n
 * - A la fin du jeu, détruit les ressources du programme, par libération des
 * pointeurs du tableau @ref DialogsQueue, et par appel aux fonctions @ref
 * closeItems, @ref destroyGraphics et @ref closeSDL.\n
 * @return le code de l'erreur en cas d'échec, sinon 0.
 */
int main()
{
	/* INIT ---------------------------------------- */

	srand( time( NULL ) );

	SDL_Window* window = initSDL();
	initGraphics();
	initGameplay();
	initItems();
	
	int run = 1;	
	SDL_Event event;

	int i;
	for( i = 0; i < 3; i++ )
	{
		DialogsQueue[ i ] = malloc( sizeof( char ) * 1024 );   /* pas de gestion des erreurs éventuelles */
		memset( DialogsQueue[ i ], 0, 1024 );
	}

	/* BOUCLE D'INTERACTION ---------------------------------------- */
	while( run )
	{





		while( SDL_PollEvent( &event ) )
		{
			/* sortie de boucle en fin de tour */
			if( event.type == SDL_QUIT )
				run = 0;
			/* souris */
			else if( event.type == SDL_MOUSEBUTTONDOWN ) 
			{
				/* clique gauche */
				if( event.button.button == SDL_BUTTON_LEFT )   
				{
					/* début */
					if( Gameplay.state == STATE_START )          
					{
						Gameplay.state = STATE_EXPLORATION;  /* passage en mode exploration; */
					}
					/* accès à l'inventaire */
					else if( Gameplay.state == STATE_INVENTORY )
					{
						SDL_Rect rects[ 10 ];
						getItemRects( rects );

						if( !itemTriggered( event.button.x, event.button.y, rects ) )
						{
							SDL_Rect buttonRects[ 4 ];
							getButtonRects( buttonRects );

							buttonTriggered( event.button.x, event.button.y, buttonRects );
						}
					}
					/* mode exploration */
					else if( Gameplay.state == STATE_EXPLORATION )
					{
						if( !elementTriggered( event.button.x, event.button.y ) )
						{
							SDL_Rect rects[ 4 ];
							getButtonRects( rects );

							buttonTriggered( event.button.x, event.button.y, rects );
						}
					}
					/* mode interaction ou conversation */
					else if( Gameplay.state == STATE_INTERACTION || Gameplay.state == STATE_TALK )
					{
						SDL_Rect rects[ 4 ];
						getButtonRects( rects );

						buttonTriggered( event.button.x, event.button.y, rects );
					}
					/* fin de partie */
					else if( Gameplay.state == STATE_WON || Gameplay.state == STATE_LOST )
					{
						Gameplay.state = STATE_START;
						initGameplay();
					}
				}
			}
		}





		SDL_RenderClear( Graphics.renderer );

		if( Gameplay.state == STATE_START )
		{
			renderStartScreen();
		}
		else if( Gameplay.state == STATE_INVENTORY )
		{
			renderInventoryBg();
			renderMenu( RENDER_INVENTORY, DialogsQueue );

			for( i = 0; i < MAX_ITEM; i++ )
			{
				SDL_Texture* tex = getItemTexture( i, 1 );
				if( tex )
				{
					renderItem( tex, i );
				}
			}

			for( i = 0; i < MAX_STUFF; i++ )
			{
				SDL_Texture* tex = getItemTexture( i, 0 );
				if( tex )
				{
					renderStuff( tex, i );
				}
			}

			renderGold( Gameplay.gold );

			char item_desc[ 300 ];
			getCurrentItemDesc( item_desc );
			renderItemDesc( item_desc );

			renderItemHighlighting( Gameplay.index_selected_item );
		}
		else if( Gameplay.state == STATE_EXPLORATION || Gameplay.state == STATE_INTERACTION || Gameplay.state == STATE_TALK )
		{
			int render_state;

			if( Gameplay.state == STATE_EXPLORATION )
			{
				renderImage( Gameplay.bg_tex[ 0 ], Gameplay.bg_rect[ 0 ] );

				for( i = 0; i < Gameplay.nb_elements; i++ )
				{
					int render = 1;

					if( Gameplay.elements[ i ].type == 0 )
					{
						int j;
						for( j = 0; j < Gameplay.nb_npc; j++ )
						{
							if( Gameplay.npcs[ j ].type == Gameplay.elements[ i ].value && Gameplay.npcs[ j ].unique_id == Gameplay.elements[ i ].value2 )
							{
								if( encounterEnd( Gameplay.npcs[ j ] ) )
								{
									render = 0;
									break;
								}
							}
						}
					}

					if( render )
						renderImage( Gameplay.elements[ i ].tex, Gameplay.elements[ i ].rect );
				}

				render_state = RENDER_EXPLORATION;
			}
			else if( Gameplay.state == STATE_INTERACTION )
			{
				renderImage( Gameplay.bg_tex[ 1 ], Gameplay.bg_rect[ 1 ] );

				int index = Gameplay.interaction_index;
				renderImage( Gameplay.elements[ index ].tex, Gameplay.elements[ index ].rect );

				render_state = RENDER_INTERACTION;
			}
			else if( Gameplay.state == STATE_TALK )
			{
				renderImage( Gameplay.bg_tex[ 1 ], Gameplay.bg_rect[ 1 ] );

				int index = Gameplay.interaction_index;
				renderImage( Gameplay.elements[ index ].tex, Gameplay.elements[ index ].rect );

				render_state = RENDER_TALK;
			}

			renderHp( Gameplay.player_current_life, Gameplay.player_max_life );
			renderMenu( render_state, DialogsQueue );
		}
		else
		{
			if( Gameplay.state == STATE_WON )
				renderEnd( 1 );
			else
				renderEnd( 0 );
		}

		renderFramerate();
		SDL_RenderPresent( Graphics.renderer );
	}

	/* LIBERATION DE LA MEMOIRE ---------------------------------------- */
	
	for( i = 0; i < 3; i++ )
		free( DialogsQueue[ i ] );
	
	closeItems();
	destroyGraphics( Graphics );
	closeSDL( window );
	return 0;
}

/** 
 * Initialise la SDL, crée une fenêtre et affiche un avertissement sur l'entrée
 * standard en cas d'erreur. Puis, crée un premier rendu dans cette fenêtre.
 * @return la fenêtre SDL créée, ou `NULL` en cas d'erreur.
 **/
SDL_Window* initSDL()
{
	SDL_Init( SDL_INIT_VIDEO );
	TTF_Init();

	SDL_Window* window = SDL_CreateWindow( "4A", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH, WINDOW_HEIGHT, 0 );
	if( !window )
	{
		printf( "SDL_CreateWindow failed" );
		return NULL;
	}

	Graphics.renderer = SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED ); // SDL_RENDERER_PRESENTVSYNC

	return window;
}

/** 
 * Détruit le rendu et la fenêtre, puis ferme et libère la mémoire de la SDL. 
 * @param window : la fenêtre SDL à fermer.
 **/
void closeSDL( SDL_Window* window )
{
	SDL_DestroyRenderer( Graphics.renderer );
	SDL_DestroyWindow( window );

	TTF_Quit();
	SDL_Quit();
}


/** 
 * Calcul le nombre d'images par seconde et réalise un rendu textuel qui met en
 * évidence ce nombre.
 **/
void renderFramerate()
{
	static int fps = 0;
	static int last_sec = 0;
	static char text[ 20 ] = "";

	int time = SDL_GetTicks();

	int delta = time - last_sec;
	fps++;

	if( delta >= 1000 )
	{
		sprintf( text, "fps : %d", fps );	
		fps = 0;
		last_sec = SDL_GetTicks() - ( delta - 1000 );
	}

	if( text[ 0 ] != '\0' )
	{
		SDL_Color color = { 0, 0, 0, 0 };
		renderText( text, WINDOW_WIDTH - 100, 0, color );
	}
}
