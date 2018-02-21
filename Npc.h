/**
 * \file Npc.h
 * \brief Déclarations des types et des fonctions centrées autour 
 * de l'interaction avec les NPC
 */

#ifndef __NPC_H__
#define __NPC_H__

extern char* DialogsQueue[ 3 ];
extern int* PlayerLife;
extern int* PlayerAta;
extern int* PlayerDef;

/// \brief Ajoute un dialogue à la file d'attente
void pushQueue( char* dialog );
/// \brief Ajoute un dialogue formatté à la file d'attente
void addDialog( char* format, ... );

/// \enum action_type \brief Le type d'interaction possible avec un NPC
typedef enum {
	NONE, ///< Aucune action
	ATTACK, ///< Le joueur attaque
	ITEM, ///< Le joueur utilise un objet
	TALK, ///< Le joueur veut parler avec le NPC
	MOVE ///< Inutilisé 
} action_type;
/// \enum talk_type \brief Le type de réponse possible avec un NPC
typedef enum {
	YES, ///< Le joueur répond OUI
	NO, ///< Le joueur répond NON
	THREAT, ///< Le joueur menace le NPC
	TRADE ///< Le joueur demande à échanger avec le NPC
} talk_type;

/// \enum diag_val \brief Le type de réaction possible d'un NPC
typedef enum {
	SURRENDER, ///< Le NPC se rend
	INTIMIDATED, ///< Le NPC est intimidé
	CORRUPT, ///< Le NPC à été corrompu (Avec de l'argent) (dans le sens moral)
	NO_CORRUPT, ///< La tentative de corruption du joueur a échoué
	DEAL, ///< Le NPC a accepté l'échange
	NO_DEAL, ///< Le NPC a refusé l'échange
	INTRO, ///< État initial, le NPC se présente
	USELESS_ITEM, ///< Tentative de corruption avec un objet
	USELESS_TALK, ///< Le joueur essaye de convaincre le NPC en parlant
	WTF, ///< Le joueur attaque les NPC irrationnelement, comme des alliés ou des innocents
	CONFUSED, ///< Le joueur a voulu donner un objet impertinant au contexte au NPC
	GRATEFUL, ///< Le joueur a donné de l'argent au NPC gratuitement
	DRUNK, ///< Le NPC est saoul
	BEER, ///< Le NPC a recu une bière de la part du joueur
	PASS ///< Le NPC autorise le joueur à traverser.
} diag_val;

typedef unsigned int uint;

/**
 * \struct npc_stats
 * \brief Représentation unique d'un NPC
 */
struct npc_stats {
	uint type; ///< Le type du NPC
	int ata; ///< Sa statistique d'attaque
	int def; ///< Sa statistique de défense
	int life; ///< Son nombre de points de vie
	int status; ///< Son status
	int unique_id; ///< Identifiant unique
};

/* Typedefs */
typedef struct npc_stats npc_stats;

/// \brief Initialise les données de l'interaction avec un NPC
int encounterInit (uint npc_type, npc_stats * npc, char * npc_name);
/// \brief Effectue une action sur le NPC
int npcResponse (npc_stats * npc, action_type action, uint action_value, char * npc_name);
/// \brief Termine l'interaction avec un NPC
int encounterEnd (npc_stats npc);

#endif