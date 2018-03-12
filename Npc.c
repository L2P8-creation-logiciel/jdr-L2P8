/**
 * \file Npc.c
 * \brief Définition des types et des fonctions centrées autour
 * de l'interaction avec les NPC
 */

/*
	This is the second draft
	of the npc_response function
	for the srping 2015 program project
	by Maximilien GROULT
	26/03/2015
*/

#include "Npc.h"
#include "Gameplay.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

/* Prototypes
int encounter_init (uint npc_type, npc_stats * npc, char * npc_name);
int npc_response (uint * situation, npc_stats * npc, uint * action_type, uint * action_value, char * npc_name);
int encounter_end (npc_stats npc);
void attaque (int p, npc_stats * npc);
int max_0 (int n);
*/

/// \brief Pointeur vers la vie du joueur
int* PlayerLife;
/// \brief Pointeur vers la statistique d'attaque du joueur
int* PlayerAta;
/// \brief Pointeur vers la statistique de défense du joueur
int* PlayerDef;

/**
 * `pushQueue` permet d'ajouter une ligne de dialogue dans la queue
 * des dialogues à afficher.
 * @param dialogue Le texte à ajouter à la file
 */
void pushQueue( char* dialog )
{
	int i;
	for( i = 0; i < 3; i++ )
	{
		if( strcmp( DialogsQueue[ i ], "" ) == 0 )
		{
			strcpy( DialogsQueue[ i ], dialog );
			return;
		}
	}

	strcpy( DialogsQueue[ 0 ], DialogsQueue[ 1 ] );
	strcpy( DialogsQueue[ 1 ], DialogsQueue[ 2 ] );
	strcpy( DialogsQueue[ 2 ], dialog );
}

/**
 * `addDialog` permet d'ajouter une ligne de dialogue dans la queue
 * des dialogues à afficher sous la forme d'un message formatté par
 * les fonctions de la famille printf.
 * @param format La chaine de format
 * @param format Les paramêtres de formattage
 */
void addDialog( char* format, ... )
{
	char buffer[ 1024 ];
	va_list args;
	va_start( args, format );
	vsnprintf( buffer, 1024, format, args );
	va_end( args );

	int i;
	for( i = 0; i < 1024; i++ )
	{
		if( buffer[ i ] == '\n' )
			buffer[ i ] = 0;
	}

	pushQueue( buffer );
}


/**
 * `max_0` renvoie la valeur maximale entre 0 et `n`
 * @param n La valeur à ajuster
 * @return La valeur ajustée
 */
int max_0 (int n) {
	if (n > 0)
		return n;
	return 0;
}

/**
 * `attaque` effectue l'intéraction d'attaque entre le joueur et un NPC
 * @param p La direction de l'intéraction. Si 0 alors le NPC attaque le joueur, sinon l'inverse
 * @param npc Le NPC concerné par l'intéraction
 */
void attaque (int p, npc_stats * npc) {
	if (p) {
		npc->life -= max_0(*PlayerAta - npc->def);
		addDialog( "You did %d damages to your opponent!", *PlayerAta - npc->def );
	} else {
		*PlayerLife -= max_0(npc->ata - *PlayerDef);
		addDialog( "Your opponent did %d damages!", npc->ata - *PlayerDef );
	}
}

/**
 * `encounterEnd` termine l'intéraction avec un NPC.
 * @param npc Les statistiques du NPC concerné par l'intéraction
 * @return 0 si le NPC a survécu à l'intéraction, sinon 1.
 */
int encounterEnd (npc_stats npc) {
	if (npc.life <= 0)
		return 1;
	return 0;
}

/**
 * `encounterInit` démarre une intéraction avec un NPC désigné par `npc_type`.
 * La fonction remplit les champs concerné de la structure `npc` et renseigne
 * le nom du NPC dans le tampon pointé par `npc_name` partant du principe
 * qu'il soit assez grand pour contenir le nom.
 * @param npc_type Le type du NPC à charger
 * @param npc Pointeur vers les stats du NPC à renseigner
 * @param npc_name Pointeur où le nom du NPC sera écrit
 * @return 0 tout le temps.
 */
int encounterInit (uint npc_type, npc_stats * npc, char * npc_name) {
	// Inutilisé
	size_t nbytes = 10;
	char * fname;
	FILE * fichier;

	fname = malloc (sizeof(char)*(4+5));

	// Ceci écrit en dehors de ce qui est alloué à fname
	// si npc_type nécéssite au moins 1 caractère pour être représenté
	sprintf(fname, "Data/%u.txt", npc_type);

	fichier = fopen(fname, "r");

	npc->type = npc_type;
	// Un nom assez long peut faire crasher le jeu
	fscanf(fichier, "%s\n", (npc_name));
	fscanf(fichier, "%d %d %d %d\n", &(npc->ata), &(npc->def), &(npc->life), &(npc->status));

	fclose(fichier);
	// fname n'est pas libéré
	return 0;
}

/**
 * \typedef `npc_dialog` représente le comportement du NPC `Grande fée`
 * lorsque le joueur clique dessus
 * @param dial pointeur vers le type de NPC associé
 */
typedef struct npc_dialog npc_dialog;

/**
 * \struct npc_line
 * \brief Associe un type de dialogue à un comportement
 */
typedef struct npc_line {
	uint val; ///< Le type du dialogue
	void (*behavior)(npc_dialog *dial); ///< Le comportement associé
} npc_line;

/**
 * \struct npc_dialog
 * \brief Définie un NPC avec ses dialogue associés
 */
struct npc_dialog  {
	uint type; ///< type du NPC
	uint size; ///< Le nombre de ligne de comportements associées
	npc_line lines[PASS + 1]; ///< Comportements associés
	void *userdata; ///< Pointeur définie par l'utilisateur pour que le NPC puisse maintenir un état interne
};

/**
 * \struct fairy_state
 * \brief Représente l'état interne de la Grande Fée
 */
typedef struct {
	int item_given;
} fairy_state;

/**
 * `fairy_intro` représente le comportement du NPC `Grande fée`
 * lorsque le joueur clique dessus
 * @param dial pointeur vers le type de NPC associé
 */
void fairy_intro(npc_dialog *dial) {
	if (!dial->userdata)
		dial->userdata = calloc(1, sizeof(fairy_state));
	fairy_state *self = dial->userdata;
	if(!self->item_given) {
		addDialog("Great Fairy - It's dangerous to go alone");
		addDialog("Great Fairy - Take this!");
		addDialog("Great Fairy - Keep it a secret from everyone!");
		buyItem(ITEM_CUPCAKE, 0);
		self->item_given = 1;
	} else {
		addDialog("Great Fairy - Haha nope");
	}
}

/**
 * `fairy_intimidated` représente le comportement du NPC `Grande fée`
 * lorsque le joueur l'intimide
 * @param dial pointeur vers le type de NPC associé
 */
void fairy_intimidated() {
	addDialog("Grande fée - Ah, ne me fait pas de mal, tiens ceci");
	buyItem(ITEM_CUPCAKE, 0);
}

/**
 * `paysanne_intro` représente le comportement du NPC `Paysanne`
 * lorsque le joueur clique dessus
 * @param dial pointeur vers le type de NPC associé
 */
void paysanne_intro(npc_dialog *dial) {
	addDialog("Paysane : Aurais-tu peur de quelque poison?");
}

/**
 * `paysanne_said_yes` représente le comportement du NPC `Paysanne`
 * lorsque le joueur lui dit "Oui"
 * @param dial pointeur vers le type de NPC associé
 */
void paysanne_said_yes(npc_dialog *dial) {
	addDialog("Paysane : Prend cette pomme et je te donnerai tout mon or.");
	buyItem(ITEM_APPLE, -10000);
}

/**
 * `nos_perso` dispatch le comportement associé au type de dialogue donné en paramètre
 * pour les comportement du npc donné en paramètre
 * @param nd Pointeur vers la définition du NPC
 * @param v \ref diag_val ou \ref talk_type correspondant à l'intéraction
 */
void nos_perso(npc_dialog *nd, uint v) {
	for (uint i = 0; i < nd->size; ++i) {
		if(nd->lines[i].val == v) {
			nd->lines[i].behavior(nd);
		}
	}
}

/**
 * Cette enum définie les nouveaux types de NPC ajoutés au jeu
 */
enum {
	FAIRY = 1000,
	PAYSANNE
};

/**
 * `Dials` est un tableau associatif qui associe un type de NPC à ses différents
 * comportement lors d'une intéraction
 */
npc_dialog Dials[] = {
	{
		FAIRY, 3, {
			{INTRO, fairy_intro },
			{SURRENDER, fairy_intro},
			{INTIMIDATED, fairy_intimidated},
			//{YES, fairy_said_yes},
			//{NO, fairy_said_no},
		}, 0
	},
	{
		PAYSANNE, 2, {
			{INTRO, paysanne_intro},
			{YES, paysanne_said_yes},
		}, 0
	}
};


/**
 * `dialogue` génère une ligne de dialogue en fonction de l'état d'intéraction,
 * du type, et du nom du NPC.
 * @param npc_type Le type du NPC concerné par l'intéraction
 * @param diag L'état de l'intéraction avec le NPC
 * @param npc_name Le nom du NPC concerné par l'intéraction
 */
void dialogue (uint npc_type, diag_val diag, char * npc_name) {
	if (npc_type > 999) {
		nos_perso(&Dials[npc_type - 1000], diag);
		return;
	}
	if (diag == INTRO) {
		if (npc_type < 100) {
			if (npc_type == 50)
				addDialog("%s    -You may not pass.\n", npc_name);
			else
				addDialog("%s    -Is there a problem citizen?\n", npc_name);
		}
		else if (npc_type < 200) {
			if (npc_type == 150){
				addDialog("%s    -Looking for a way out of town? That can be arranged, for the right price. 30 coins. \n", npc_name);
			}
			else
				addDialog("%s    -Give us your gold or else...\n", npc_name);
		}
		else if (npc_type < 300) {
			if (npc_type == 250)
				addDialog("%s    -The road is dangerous and full of bandits. Can you help me?\n", npc_name);
			else if(npc_type==251)
				addDialog("%s    -Hey there. My axe looks far more better than your tiny sword. Do you want it for 50 gold?", npc_name);
			else if(npc_type==252)
				addDialog( "%s   -Hey there. My armor looks far more safer than yours. Do you want it for 50 gold?", npc_name);
			else if(npc_type==253)
				addDialog( "%s   -This poison might be useful one day... I can sell you a bottle for 50 gold, deal?", npc_name);
			else
				addDialog("%s    -Hey there. Interested in making a smart purchase?\n", npc_name);
		}
		else if (npc_type < 400)
			addDialog("%s    -Life is hard for a poor farmer like myself.\n", npc_name);
		else if (npc_type < 500)
			addDialog("%s    -What is that awful smell?!\n", npc_name);
		else {
			if (npc_type ==  500)
				addDialog("%s    -Who the hell are you?!\n", npc_name);
			if (npc_type ==  501)
			{
				addDialog("%s    -I need to rest or this wound will be my last. However there is no time to lose,", npc_name);
				addDialog("%s    you must go at once to the count's manor and show him this letter I wrote for him.", npc_name);
				buyItem( ITEM_PLETTER, 0 );
			}
			if (npc_type ==  502)
				addDialog("%s    -I've been told you had something for me.\n", npc_name);
			if (npc_type ==  503)
				addDialog("%s    -Looking for something to drink?\n", npc_name);
		}
	} else
	if (diag == SURRENDER) {
		addDialog("surrenders : %s", npc_name);
		addDialog("    -Alright, I surrender...have mercy...\n");
	} else
	if (diag == INTIMIDATED) {
		addDialog("%s    -Please don't hurt me...\n", npc_name);
	} else
	if (diag == CORRUPT) {
		if (npc_type < 100)
			addDialog("%s    -It would seem I made a mistake...\n", npc_name);
		else if (npc_type < 200)
			addDialog("%s    -I like coin more than I like danger.\n", npc_name);
		else if (npc_type < 300)
			addDialog("%s    -Now that's what I call a fair bargain!\n", npc_name);
		else if (npc_type < 400)
			addDialog("%s    -You think because I'm poor you can just buy me off? You're right!\n", npc_name);
		else if (npc_type < 500)
			addDialog("%s    -Every man has his price. I guess I found out mine.\n", npc_name);
		else
			addDialog("%s    -Wow! Money can really solve everything!\n", npc_name);
	} else
	if (diag == NO_CORRUPT) {
		if (npc_type < 100)
			addDialog("%s    -You think you can buy me with money?!\n", npc_name);
		else if (npc_type < 200)
			addDialog("%s    -I'd rather take your money from your dead body!\n", npc_name);
		else if (npc_type < 300)
			addDialog("%s    -Too little too late!\n", npc_name);
		else if (npc_type < 400)
			addDialog("%s    -You think because I'm poor you can just buy me off?\n", npc_name);
		else if (npc_type < 500)
			addDialog("%s    -Unlike your filthy kind, I'm above such petty corruption!\n", npc_name);
		else
			addDialog("%s    -First you attack me and now you insult me?!\n", npc_name);
	} else
	if (diag == DEAL) {
		addDialog("%s    -Nice doing business with you.\n", npc_name);
	} else
	if (diag == NO_DEAL) {
		addDialog("%s    -Gonna need a bit more.\n", npc_name);
	} else
	if (diag == DRUNK) {
		if (!rand()%3)
			addDialog("%s    -I love booze!\n", npc_name);
		else if (rand()%2)
			addDialog("%s    -I feel a bit tipsy...\n", npc_name);
		else
			addDialog("%s    -Are we on a boat? It feels like we're on a boat...\n", npc_name);
	} else
	if (diag == BEER) {
		addDialog("%s    -That's the stuff! Walk right in, friend.\n", npc_name);
	} else
	if (diag == PASS) {
		addDialog("%s    -Everything seems in order. You can pass.\n", npc_name);
	} else
	if (diag == USELESS_ITEM) {
		addDialog("%s    -You think just throwing stuff at me is gonna work?\n", npc_name);
	} else
	if (diag == USELESS_TALK) {
		addDialog("%s    -Words won't save you now!\n", npc_name);
	} else
	if (diag == CONFUSED) {
		addDialog("%s    -What are you trying to do?\n", npc_name);
	} else
	if (diag == GRATEFUL) {
		addDialog("%s    -Thanks!\n", npc_name);
	} else
	if (diag == WTF) {
		if (npc_type < 100)
			addDialog("%s    -You dare draw steel against those who represent the King?!\n", npc_name);
		else if (npc_type < 200)
			addDialog("%s    -Oooh! So that's how you wanna play?\n", npc_name);
		else if (npc_type < 300)
			addDialog("%s    -What the hell is wrong with you?!\n", npc_name);
		else if (npc_type < 400)
			addDialog("%s    -Why are you doing this?! Am I not miserable enough as it is?\n", npc_name);
		else if (npc_type < 500)
			addDialog("%s    -How dare you?! You will pay for this!\n", npc_name);
		else
			addDialog("%s    -Traitor! Prepare to meet The Weeper!\n", npc_name);
	} else {
		fprintf(stderr, "error : NO DIALOGUE FOUND\n");
	}
}

/**
 * `advDialogue` répond à un NPC
 * @param talk La réponse du joueur
 * @param npc_stats Les statistiques du NPC
 * @param npc_name Le nom du NPC
 * @return Le nouveau status du NPC
 */
int advDialogue (talk_type talk, npc_stats * npc, char * npc_name) {
	if (npc->type > 999) {
		nos_perso(&Dials[npc->type - 1000], talk);
		return 0;
	}

	if (npc->type < 100) { /* guard */
		if (talk == YES) {
			if (npc->type == 50) {
				addDialog("%s    -No.\n", npc_name);
				return npc->status;
			}
			if (npc->status == 0) {
				addDialog("%s    -Then maybe you should find someone who cares.\n", npc_name);
				return 4;
			}
			addDialog("%s    -Get lost!\n", npc_name);
			return 3;
		}
		if (talk == NO) {
			if (npc->type == 50) {
				addDialog("%s    -Yes.\n", npc_name);
				return npc->status;
			}
			if (npc->status == 0) {
				addDialog("%s    -Then why are you bothering me? Beat it.\n", npc_name);
				return 4;
			}
			addDialog("%s    -Get lost!\n", npc_name);
			return 3;
		}
		if (talk == THREAT) {
			if (npc->status == 3) {
				addDialog("%s    -I'm gonna teach you some manners!\n", npc_name);
				return 1;
			}
			addDialog("%s    -Watch your tongue or lose it!\n", npc_name);
			return 3;
		}
		if (talk == TRADE) {
			if (npc->type == 50) {
				addDialog("%s    -I'm not sure I get what you're saying. Something golden might help me understand better...\n", npc_name);
				return 2;
			}
			addDialog("%s    -Sorry but I'm no merchant.\n", npc_name);
			return npc->status;
		}
	}
	if (npc->type < 200) { /* bandit */
		if (npc->type == 150) {
			if (npc->status == 6) {
				addDialog("%s    -...\n", npc_name);
				return 6;
			}
			if (npc->status == 4) {
				addDialog("%s    -I don't like talking to clowns.\n", npc_name);
				return 6;
			}
			if (talk == YES) {
				if (npc->status == 0) {
					if( Gameplay.gold >= 30 ){
						Gameplay.gold -= 30;
						addDialog("%s    -There you go.\n", npc_name);
						loadArea( 13 );
						Gameplay.state = STATE_EXPLORATION;
						return 2;
					}
					else
					{
						addDialog("%s    -No time for people like you.", npc_name);
						return 2;
					}
				}
				if (npc->status == 2) {
					addDialog("%s    -Good, now hand over the money.\n", npc_name);
					return 2;
				}
				if (npc->status == 5) {
					addDialog("%s    -Good, now hand over the money.\n", npc_name);
					return 4;
				}
				addDialog("%s    -Then find somewhere else to be.\n", npc_name);
				return 2;
			}
			if (talk == NO) {
				addDialog("%s    -Then find somewhere else to be.\n", npc_name);
				return 4;
			}
			if (talk == THREAT) {
				if (npc->status < 4) {
					addDialog("%s    -Let's keep this civil. You want out? Yes or no.\n", npc_name);
					return 5;
				}
				addDialog("%s    -Uncivil it is.\n", npc_name);
				return 1;
			}
			if (talk == TRADE) {
				if (npc->type == 0) {
					addDialog("%s    -I like your style! Special discount for you: only 30 coins.\n", npc_name);
					return 2;
				}
				if (npc->status == 2) {
					addDialog("%s    -Sorry but I can't go any lower.\n", npc_name);
					return 2;
				}
				addDialog("%s    -It's a bit late for that.\n", npc_name);
				return 4;
			}
		}
		else {
			if (npc->status == 3) {
				addDialog("%s    -Stop talking and hand over the money!\n", npc_name);
				return 5;
			}
			if (npc->status == 5) {
				addDialog("%s    -I've had enough of this. I'm gonna kill you and take the bloody money myself!\n", npc_name);
				return 1;
			}
			if (talk == YES) {
				addDialog("%s    -Good, now hand over the money.\n", npc_name);
				return 3;
			}
			if (talk == NO) {
				addDialog("%s    -Then die!\n", npc_name);
				return 1;
			}
			if (talk == THREAT) {
				addDialog("%s    -I'm gonna stab you in the gut!\n", npc_name);
				return 1;
			}
			if (talk == TRADE) {
				addDialog("%s    -Do I look like I'm here to negotiate? Now hand over the money!\n", npc_name);
				return 3;
			}
		}
	}
	if (npc->type < 300) { /* merchant */
		if (npc->type == 250) {
			if (talk == YES) {
				addDialog("%s    -Great! Let's go!\n", npc_name);
				processAction( ACTION_TALK_QUIT );
				loadArea( 12 );// change zone -> road + rencontre bandit classique
				processElement( 3 );
				Gameplay.no_leave = 1;
				return 0;
			}
			if (talk == NO) {
				addDialog("%s    -Pretty please?\n", npc_name);
				return 0;
			}
			if (talk == THREAT) {
				addDialog("%s    -Oh my! How about putting those skills of yours to good use?\n", npc_name);
				return 0;
			}
			if (talk == TRADE) {
				addDialog("%s    -Sorry but I'm not open for business. How about that request of mine though?\n", npc_name);
				return 0;
			}
		} else {
			if (talk == YES) {
				if( Gameplay.gold < 50 )
				{
					addDialog("%s    -Don't try to scam me! Come back with money!", npc_name);
					return 60;
				}
				if( npc->type==251){
					buyItem( ITEM_AXE, 0 );
				}
				else if( npc->type==252){
					buyItem( ITEM_ARMOR, 0);
				}
				else if( npc->type == 253){
					buyItem( ITEM_POISON, 0 );
				}
				addDialog("%s    -Great! You will not regret that.\n", npc_name);
				return 60;
			}
			if (talk == NO) {
				addDialog("%s    -Maybe later then. Have a nice day!\n", npc_name);
				return 0;
			}
			if (talk == THREAT) {
				addDialog("%s    -HAHAHA, you're funny.\n", npc_name);
				return npc->status;
			}
			if (talk == TRADE) {
				addDialog("%s    -Great! Have a look then.\n", npc_name);
				return 60;
			}
		}
	}
	if (npc->type < 400) { /* peasant */
		addDialog("%s    -I'm just a dumb peasant, you shouldn't waste your time talking to me.", npc_name);
		return 0;
	}
	if (npc->type < 500) { /* noble */
		if (npc->status == 0) {
			addDialog("%s    -And now I think I also hear a noise.\n", npc_name);
			return 2;
		}
		if (npc->status == 2) {
			addDialog("%s    -There it goes again, the noise.\n", npc_name);
			return 4;
		}
		if (npc->status == 4) {
			addDialog("%s    -Bloody noise again! I wonder what it might be...\n", npc_name);
			return 5;
		}
		if (npc->status == 5) {
			addDialog("%s    -I should endeavor to find its source.\n", npc_name);
			return 6;
		}
		addDialog("%s    -...\n", npc_name);
		return 6;
	}
	if (npc->type == 500) { /* duke */
		if (talk == THREAT) {
			addDialog("%s    -You think you can threaten me at my own court?!\n", npc_name);
			return 1;
		}
		if (npc->status == 0) {
			addDialog("%s    -Do you not understand my words?\n", npc_name);
			return 2;
		}
		if (npc->status == 2) {
			addDialog("%s    -This is insolence!\n", npc_name);
			return 3;
		}
		addDialog("%s    -I'll teach you to mock me!\n", npc_name);
		return 1;
	}
	if (npc->type == 501) { /* prince */
		addDialog("%s    -Stop being weird and just go!\n", npc_name);
		return npc->status;
	}
	if (npc->type == 502) { /* count */
		if (npc->status == 5) {
			addDialog("%s    -You have been warned!\n", npc_name);
			return 1;
		}
		if (talk == THREAT) {
			if (npc->status == 3) {
				addDialog("%s    -You have been warned!\n", npc_name);
				return 1;
			}
			addDialog("%s    -You will mind your manners when you are in my home!\n", npc_name);
			return 3;
		}
		if (talk == YES) {
			if (npc->status == 0) {
				addDialog("%s    -Show me.\n", npc_name);
				return 2;
			}
		}
		if (npc->status >= 2) {
			addDialog("%s    -I suggest you do not try my patience.\n", npc_name);
			return 5;
		}
		if (talk == NO) {
			addDialog("%s    -Then I'll have to ask you to stop wasting my time and get out.\n", npc_name);
			return 3;
		}
		if (talk == TRADE) {
			addDialog("%s    -Who do you think you're talking to?!\n", npc_name);
			return 3;
		}
	}
	if (npc->type == 503) { /* barkeep */
		if (talk == YES) {
			if (npc->status == 0) {
				buyItem( ITEM_BEER, 0 );
				addDialog("%s    -Here you go. Want another?\n", npc_name);
				return 0;
			}
			addDialog("%s    -You can't leave town without a good reason, like proper business. Want a beer now?\n", npc_name);
			return 0;
		}
		if (talk == NO) {
			if (npc->status == 0) {
				addDialog("%s    -No? something else then? Information?\n", npc_name);
				return 2;
			}
			addDialog("%s    -How about a beer then?\n", npc_name);
			return 0;
		}
		addDialog("%s    -Sorry pal but beer is all I have. Want some?\n", npc_name);
		return 0;

	}
	return 99;
}

/**
 * `advDialogue` détermine et effectue l'action du NPC en réponse à une action du joueur
 * @param npc Les statistiques du NPC
 * @param action L'action à laquelle répondre
 * @param action_value Un argument pour l'action à effectuer
 * @param npc_name Le nom du NPC
 * @return Le nouveau status du NPC
 */
int npcResponse (npc_stats * npc, action_type action, uint action_value, char * npc_name) {
	int temp, corrupt_val = 9999;
	talk_type talk = YES;

	if (npc->status == 1) {
		if (action == ATTACK) {
			temp = npc->life - action_value;
			if (temp <= 0) {
				if (rand()%2) {
					dialogue(npc->type, SURRENDER, npc_name);
					npc->status = -1;
					return 0;
				}
			}
			attaque(0, npc);
			return 1;
		}
		if (action == ITEM) {
			if (action_value > 300) {
				temp = action_value - 300;
				if (npc->type < 100) corrupt_val = 25;
				else if (npc->type < 200) corrupt_val = 50;
				else if (npc->type < 300) corrupt_val = 100;
				else if (npc->type < 400) corrupt_val = 10;
				else if (npc->type < 500) corrupt_val = 250;
				else corrupt_val = 500;
				if (temp < (corrupt_val + 5) || (rand()%temp) < corrupt_val) {
					dialogue(npc->type, NO_CORRUPT, npc_name);
					attaque(0, npc);
					return 1;
				}
				dialogue(npc->type, CORRUPT, npc_name);
				return 0;
			}
			dialogue(npc->type, USELESS_ITEM, npc_name);
			attaque(0, npc);
			return 1;
		}
		if (action == TALK) {
			temp = npc->life - *PlayerAta;
			if (temp <= 0) {
				if (rand()%4 && action_value == 3) {
					dialogue(npc->type, SURRENDER, npc_name);
					npc->status = -1;
					return 0;
				}
			}
			dialogue(npc->type, USELESS_TALK, npc_name);
			attaque(0, npc);
			return 1;
		}
		return 99;
	} else {
		if (npc->type % 100 == 22) {
			if (action == ATTACK) {
				if (action_value) {
					dialogue(npc->type, WTF, npc_name);
					//npc->type += 1;
					return 1;
				}
				npc->type += 1;
				return 1;
			}
			dialogue(npc->type, DRUNK, npc_name);
			return 0;
		}
		if (action == NONE) {
			dialogue(npc->type, INTRO, npc_name);
			return 0;
		}
		if (action == TALK) {
			talk += action_value;
			return advDialogue(talk, npc, npc_name);
		}
		if (action == ITEM) {

			if (action_value > 300) {
				if (npc->type == 150 && npc->status == 2) {
					if (action_value > 329) {
						dialogue(npc->type, DEAL, npc_name);
						loadArea( 13 );
						return 0;
					}
					dialogue(npc->type, NO_DEAL, npc_name);
					return 2;
				}
				if (npc->type > 49 && npc->type < 60 && npc->status == 2) {
					if (action_value > 399) {
						dialogue(npc->type, PASS, npc_name);
						if (npc->type == 50)
							loadArea( 4 );
						if (npc->type == 51)
							loadArea( 12 );
						if (npc->type == 52)
							loadArea( 2 );

						Gameplay.state = STATE_EXPLORATION;
						return 0;
					}
					dialogue(npc->type, NO_DEAL, npc_name);
					return 2;
				}
				dialogue(npc->type, GRATEFUL, npc_name);
				buyItem( ITEM_NONE, action_value - 300 );
				return npc->status;
			}

			if (action_value == 201 && npc->type == 502) {
				addDialog("%s    -I see... I'm sorry but there isn't much I can do. Unless you can find a way to kill the duke.", npc_name );
				addDialog("%s    Take this letter, it will help you get inside the castle. Good luck!\n", npc_name);
				buyItem( ITEM_CLETTER, 0 );
				return 0;
			}

			if (action_value == 202 && npc->type == 50) {
				dialogue(npc->type, PASS, npc_name);
				loadArea( 4 );
				Gameplay.state = STATE_EXPLORATION;
				return 0;
			}

			if (action_value == 203 && npc->type == 51 && npc->status == 2) {
				dialogue(npc->type, PASS, npc_name);
				// remove "lettre du marchand" from inventory
				// change zone to -> route
				return 0;
			}

			if (action_value == 204 && npc->type > 49 && npc->type < 60) {
				Gameplay.items = inventoryDel( 204 );
				dialogue(npc->type, BEER, npc_name);
				if (npc->type == 50)
					loadArea( 4 );
				if (npc->type == 51)
					loadArea( 12 );
				if (npc->type == 52)
					loadArea( 2 );

				Gameplay.state = STATE_EXPLORATION;
				return 2 ;
			}

			dialogue(npc->type, CONFUSED, npc_name);
			return 0;
		}
		if (action == ATTACK) {
			if (action_value) {
				if (npc->status > -1) {
					dialogue(npc->type, WTF, npc_name);
					return 1;
				}
				return 1;
			}
			if (npc->status == -1)
				return 0;
			return 1;
		}
		fprintf(stderr, "error : no npc response found\n");
		return 99;
	}
}
