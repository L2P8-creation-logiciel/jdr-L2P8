/**
 * @file Graphics.c
 * Définition des fonctions liées à l'affichage
 */
#include "Graphics.h"

#include <assert.h>
#include <stdlib.h>

/**
 * `initGraphics` charge les ressources graphiques contenues dans
 * les dossiers Data et Img relatifs à l'exécutable du jeu.
 * Cette fonction attribue aussi aux diverse textures des position 
 * où elles seront désinées
 */
void initGraphics()
{
	Graphics.font = TTF_OpenFont( "Data/CL.ttf", 20 );
	if( !Graphics.font )
	{
		printf( "Font not found\n" );
		assert( 0 );
	}

	loadImage( "fond_start", &Graphics.texture[ START_BG ], &Graphics.rect[ START_BG ] );
	Graphics.rect[ START_BG ].x = 0;
	Graphics.rect[ START_BG ].y = 0;

	loadImage( "fond_4A", &Graphics.texture[ MENU_BG ], &Graphics.rect[ MENU_BG ] );
	Graphics.rect[ MENU_BG ].x = 0;
	Graphics.rect[ MENU_BG ].y = 497;

	loadImage( "fond_dialogues", &Graphics.texture[ MENU_DIALOG ], &Graphics.rect[ MENU_DIALOG ]);
	Graphics.rect[ MENU_DIALOG ].x = 0;
	Graphics.rect[ MENU_DIALOG ].y = 394;

	loadImage( "fond_texte", &Graphics.texture[ MENU_ATT ], &Graphics.rect[ MENU_ATT ] );
	Graphics.rect[ MENU_ATT ].x = Graphics.rect[ MENU_BG ].x + 150;
	Graphics.rect[ MENU_ATT ].y = Graphics.rect[ MENU_BG ].y + 10;

	loadImage( "fond_texte", &Graphics.texture[ MENU_TALK ], &Graphics.rect[ MENU_TALK ] );
	Graphics.rect[ MENU_TALK ].x = Graphics.rect[ MENU_ATT ].x;
	Graphics.rect[ MENU_TALK ].y = Graphics.rect[ MENU_ATT ].y + Graphics.rect[ MENU_ATT ].h + 5;

	loadImage( "fond_texte", &Graphics.texture[ MENU_ITEM ], &Graphics.rect[ MENU_ITEM ] );
	Graphics.rect[ MENU_ITEM ].x = Graphics.rect[ MENU_ATT ].x + Graphics.rect[ MENU_ATT ].w + 20;
	Graphics.rect[ MENU_ITEM ].y = Graphics.rect[ MENU_ATT ].y;

	loadImage( "fond_texte", &Graphics.texture[ MENU_MOVE ], &Graphics.rect[ MENU_MOVE ] );
	Graphics.rect[ MENU_MOVE ].x = Graphics.rect[ MENU_ATT ].x + Graphics.rect[ MENU_ATT ].w + 20;
	Graphics.rect[ MENU_MOVE ].y = Graphics.rect[ MENU_ATT ].y + Graphics.rect[ MENU_ATT ].h + 5;	

	loadImage( "hp_barre", &Graphics.texture[ HP_BARRE ], &Graphics.rect[ HP_BARRE ] );
	Graphics.rect[ HP_BARRE ].x = 10;
	Graphics.rect[ HP_BARRE ].y = 10;

	loadImage( "hp", &Graphics.texture[ HP ], &Graphics.rect[ HP ] );
	Graphics.rect[ HP ].x = Graphics.rect[ HP_BARRE ].x + 1;
	Graphics.rect[ HP ].y = Graphics.rect[ HP_BARRE ].y + 1;

	loadImage( "inven_bg", &Graphics.texture[ INVEN_BG ], &Graphics.rect[ INVEN_BG ] );
	Graphics.rect[ INVEN_BG ].x = Graphics.rect[ INVEN_BG ].y = 0;

	loadImage( "inven_highlight", &Graphics.texture[ INVEN_HIGHLIGHT ], &Graphics.rect[ INVEN_HIGHLIGHT ] );

	loadImage( "victory", &Graphics.texture[ VICTORY ], &Graphics.rect[ VICTORY ] );
	Graphics.rect[ VICTORY ].x = Graphics.rect[ VICTORY ].y = 0;

	loadImage( "game_over", &Graphics.texture[ GAME_OVER ], &Graphics.rect[ GAME_OVER ] );
	Graphics.rect[ GAME_OVER ].x = Graphics.rect[ GAME_OVER ].y = 0;
}

/**
 * `destroyGraphics` libère les ressources précedemment acquises
 */
void destroyGraphics()
{
	TTF_CloseFont( Graphics.font );

	int i;
	for( i = 0; i < NB_TEXTURES; i++ )
		SDL_DestroyTexture( Graphics.texture[ i ] );
}

/**
 * `loadImage` charge une image dans le dossier Img à partir de son nom
 * sans extension. Le fichier doit exister sous la forme d'un fichier png
 * L'appelant doit passer en paramètres un pointeur vers un pointeur vers
 * un SDL_Texture, qui pointera vers la texture ainsi créée à la fin de la 
 * fonction, ainsi qu'un pointeur vers un SDL_Rect qui contiendra les dimensions
 * de la textures à la fin de l'éxécution de cette fonction.
 * @param filename Nom du fichier dans le dossier Img sans extension de l'image à charger
 * @param texture Pointeur vers Pointeur vers SDL_Texture qui pointera vers la texture chargée
 * @param rect Pointeur vers rectangle qui contiendra les dimensions de la texture chargée
 */
void loadImage( char* fileName, SDL_Texture** texture, SDL_Rect* rect )
{
	char path[ 64 ];
	sprintf( path, "Img/%s.png", fileName );

	*texture = IMG_LoadTexture( Graphics.renderer, path );
	if( *texture == NULL )
	{
		printf( "%s not found\n", fileName );
		assert( 0 );
	}

	SDL_QueryTexture( *texture, NULL, NULL, &rect->w, &rect->h );
}

/**
 * `renderImage` affiche une texture à l'écran à une position et avec
 * des dimensions données.
 * @param texture La texture à afficher
 * @param rect Le rectangle où placer la texture
 */
void renderImage( SDL_Texture* texture, SDL_Rect rect )
{
	SDL_RenderCopy( Graphics.renderer, texture, NULL, &rect );
}

/**
 * `renderText` affiche du texte à l'écran à une position et avec
 * une couleur donné.
 * @param text Le texte à afficher
 * @param x L'abscisse où afficher le texte
 * @param y L'ordonnée où afficher le texte
 * @param color La couleur du texte à afficher
 */
void renderText( char* text, int x, int y, SDL_Color color )
{
	SDL_Surface* surface = TTF_RenderText_Solid( Graphics.font, text, color );
	SDL_Texture* texture = SDL_CreateTextureFromSurface( Graphics.renderer, surface );

	SDL_FreeSurface( surface );

	SDL_Rect rect;
	SDL_QueryTexture( texture, NULL, NULL, &rect.w, &rect.h );
	rect.x = x;
	rect.y = y;

	renderImage( texture, rect );

	SDL_DestroyTexture( texture );
}

/**
 * `getButtonRects` écrit dans le tableau rects les rectangles des 4 boutons du bas de l'écran
 * @param rects Le tableau où copier les coordonnées
 */
void getButtonRects( SDL_Rect rects[] )
{
	int i;
	for( i = 0; i < 4; i++ )
		rects[ i ] = Graphics.rect[ MENU_ATT + i ];
}

/**
 * `renderStartScreen` affiche l'écran de démarrage
 */
void renderStartScreen()
{
	renderImage( Graphics.texture[ START_BG ], Graphics.rect[ START_BG ] );
}

/**
 * `renderMenu` affiche un écran et les éléments de l'interface utilisateur
 * selon l'état de jeu `render_state`, ainsi que les 3 dernieres lignes de dialogue
 * avec un NPC `dialogs`.
 * @param render_state L'état du jeu à afficher.
 * @param dialogs Les lignes de dialogues à afficher.
 */
void renderMenu( int render_state, char* dialogs[] )
{	
	SDL_Color color = { 0, 0, 0, 0 };

	renderImage( Graphics.texture[ MENU_BG ], Graphics.rect[ MENU_BG ] );
	renderImage( Graphics.texture[ MENU_DIALOG ], Graphics.rect[ MENU_DIALOG ] );

	if( render_state == RENDER_EXPLORATION )
	{
		renderImage( Graphics.texture[ MENU_ITEM ], Graphics.rect[ MENU_ITEM ] );
		renderText( "Inventory", Graphics.rect[ MENU_ITEM ].x + 70, Graphics.rect[ MENU_ITEM ].y + 10, color );
	}
	else
	{
		renderImage( Graphics.texture[ MENU_ATT ], Graphics.rect[ MENU_ATT ] );
		renderImage( Graphics.texture[ MENU_TALK ], Graphics.rect[ MENU_TALK ] );
		renderImage( Graphics.texture[ MENU_ITEM ], Graphics.rect[ MENU_ITEM ] );
		renderImage( Graphics.texture[ MENU_MOVE ], Graphics.rect[ MENU_MOVE ] );

		if( render_state == RENDER_INTERACTION )
		{
			renderText( "Attack", Graphics.rect[ MENU_ATT ].x + 70, Graphics.rect[ MENU_ATT ].y + 10, color );
			renderText( "Talk", Graphics.rect[ MENU_TALK ].x + 70, Graphics.rect[ MENU_TALK ].y + 10, color );
			renderText( "Inventory", Graphics.rect[ MENU_ITEM ].x + 70, Graphics.rect[ MENU_ITEM ].y + 10, color );
			renderText( "Leave", Graphics.rect[ MENU_MOVE ].x + 70, Graphics.rect[ MENU_MOVE ].y + 10, color );
		}
		else if( render_state == RENDER_INVENTORY )
		{
			renderText( "Use", Graphics.rect[ MENU_ATT ].x + 70, Graphics.rect[ MENU_ATT ].y + 10, color );
			renderText( "Equip", Graphics.rect[ MENU_TALK ].x + 70, Graphics.rect[ MENU_TALK ].y + 10, color );
			renderText( "Throw", Graphics.rect[ MENU_ITEM ].x + 70, Graphics.rect[ MENU_ITEM ].y + 10, color );
			renderText( "Quit", Graphics.rect[ MENU_MOVE ].x + 70, Graphics.rect[ MENU_MOVE ].y + 10, color );
		}
		else
		{
			renderText( "Yes", Graphics.rect[ MENU_ATT ].x + 70, Graphics.rect[ MENU_ATT ].y + 10, color );
			renderText( "No", Graphics.rect[ MENU_TALK ].x + 70, Graphics.rect[ MENU_TALK ].y + 10, color );
			renderText( "Threat", Graphics.rect[ MENU_ITEM ].x + 70, Graphics.rect[ MENU_ITEM ].y + 10, color );
			renderText( "Quit", Graphics.rect[ MENU_MOVE ].x + 70, Graphics.rect[ MENU_MOVE ].y + 10, color );
		}
	}

	int i;
	int offset = 30;

	for( i = 0; i < 3; i++ )
		renderText( dialogs[ i ], 10, 402 + offset * i, color );
}

/**
 * `renderHp` la barre de vie du joueur avec une proportion de 
 *  rouge et de gris approximant la proportion entre `hp_remains` et
 * `hp_all` avec la relation définie par arithméthique entière
 * `hp_remains * 100 / hp_all`. Le comportement est indéfini
 * si le joueur amasse une certaine quantité de points de vie.
 * La représentation textuelle de la vie est alors superposée
 * sur la barre de vie.
 * @param hp_remains Le nombre de points de vie restants.
 * @param hp_all Le nombre de points de vie maximum.
 */
void renderHp( int hp_remains, int hp_all )
{
	int pourcent_life = ( hp_remains * 100 ) / hp_all;

	Graphics.rect[ HP ].w = pourcent_life;
	renderImage( Graphics.texture[ HP_BARRE ], Graphics.rect[ HP_BARRE ] );
	renderImage( Graphics.texture[ HP ], Graphics.rect[ HP ] );

	SDL_Color color = { 0, 0, 0, 0 };
	char life[ 20 ];
	sprintf( life, "%d / %d", hp_remains, hp_all );
	renderText( life, Graphics.rect[ HP_BARRE ].x + 20, Graphics.rect[ HP_BARRE ].y, color );
}

/**
 * `renderStartScreen` affiche le fond de l'écran de l'inventaire
 */
void renderInventoryBg()
{
	renderImage( Graphics.texture[ INVEN_BG ], Graphics.rect[ INVEN_BG ] );
}

/**
 * `renderItem` affiche la texture d'un équipement dans la page d'inventaire
 * sur la case donnée par un index.
 * @param texture La texture à afficher
 * @param index La case de l'équipement à remplir
 */
void renderStuff( SDL_Texture* texture, int index )
{
	SDL_Rect rect;
	rect.w = rect.h = 54;

	rect.x = 72;
	rect.y = 215 + 81 * index;
	renderImage( texture, rect );
}

/**
 * `renderItem` affiche la texture d'un item dans la page d'inventaire
 * sur la case donnée par un index.
 * @param texture La texture à afficher
 * @param index La case de l'inventaire à remplir
 */
void renderItem( SDL_Texture* texture, int index )
{
	SDL_Rect rect;
	rect.w = rect.h = 54;
	rect.x = 415 + ( ( rect.w + 48 ) * ( index%4 ) );
	rect.y = 88 + ( ( rect.h + 55 ) * ( index/4 ) );

	renderImage( texture, rect );
}

/**
 * `getItemRects` écrit dans le tableau rects les rectangles 
 * des 10 objets qui peuvent tenir dans l'inventaire
 * @param rects Le tableau où copier les coordonnées
 */
void getItemRects( SDL_Rect rects[] )
{
	int i;

	for( i = 0; i < 10; i++ )
	{
		rects[ i ].w = rects[ i ].h = 54;
		rects[ i ].x = 415 + ( ( rects[ i ].w + 48 ) * ( i%4 ) );
		rects[ i ].y = 88 + ( ( rects[ i ].h + 55 ) * ( i/4 ) );
	}
}

/**
   `renderGold` affiche une quantité d'argent obtenu.
   \param gold La quantité de gold affiché.
 */
void renderGold( int gold )
{
	// D'étrange choses se passerons à plus de 100 million de gold
	char gold_str[ 8 ];
	sprintf( gold_str, "%d", gold );

	SDL_Color color = { 0, 0, 0, 0 };
	renderText( gold_str, 550, 355, color );
}

/**
 * `renderItemDesc` affiche la description d'un objet
 * dans l'inventaire, générallement un objet séléctionné.
 * @param item_desc Une chaine représentant la description d'un objet
 */
void renderItemDesc( char* item_desc )
{
	SDL_Color color = { 0, 0, 0, 0 };
	int x = 200;
	int y;
	
	char tmp[ 300 ];
	int i;
	int compteur = 0; int nb_lignes = 0;

	while( compteur < strlen( item_desc ) )
	{
		for( i = 0; item_desc[ compteur ] != ';' && item_desc[ compteur ] != '\0' ; i++ )
		{
			tmp[ i ] = item_desc[ compteur ];
			compteur++;

		}
		compteur++;
		y = 110 + nb_lignes * 30;
		nb_lignes++;
		tmp[ i ] = '\0';
		renderText( tmp, x, y, color );
		memset( tmp, 0, 300 );
	}
	
}

/**
 * `renderItemHighlighting` affiche une décoration autour d'un objet
 * dans l'inventaire, générallement un objet séléctionné.
 * @param index L'indice dans l'inventaire de l'objet à décorer
 */
void renderItemHighlighting( int index )
{
	if( index == -1 )
		return;

	SDL_Rect rect = Graphics.rect[ INVEN_HIGHLIGHT ];

	rect.x = 405 + ( ( 54 + 48 ) * ( index%4 ) );
	rect.y = 78 + ( ( 54 + 55 ) * ( index/4 ) );	

	renderImage( Graphics.texture[ INVEN_HIGHLIGHT ], rect );
}

/**
 * `renderEnd` affiche l'écran de fin du jeu selon si le joueur 
 * a gagné ou non.
 * @param won Non nul si le joueur a gagné.
 */
void renderEnd( int won )
{
	if( won )
		renderImage( Graphics.texture[ VICTORY ], Graphics.rect[ VICTORY ] );
	else
		renderImage( Graphics.texture[ GAME_OVER ], Graphics.rect[ GAME_OVER ] );
}
