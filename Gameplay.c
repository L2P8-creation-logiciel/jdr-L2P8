/**
 *   \file Gameplay.c
 *   \brief Corps des fonctions gérant la logique de jeu
 */


#include "Gameplay.h"

#include <assert.h>
#include <stdio.h>

/**
 * `initGameplay` initialise la variable globale @ref Gameplay avec ses valeurs
 * par défaut.
 */
void initGameplay() {
  initItems();

  PlayerLife = &Gameplay.player_current_life;
  PlayerAta = &Gameplay.player_atk;
  PlayerDef = &Gameplay.player_def;

  Gameplay.state = STATE_START;
  Gameplay.elements = NULL;
  Gameplay.nb_elements = 0;
  Gameplay.player_max_life = 120;
  Gameplay.player_current_life = 120;
  Gameplay.player_atk = 20;
  Gameplay.player_def = 5;
  Gameplay.items = inventoryInit();
  Gameplay.selected_item = 0;
  Gameplay.index_selected_item = -1;
  Gameplay.stuff[0] = 50;
  Gameplay.stuff[1] = 39;
  Gameplay.gold = START_GOLD;

  Gameplay.npcs = NULL;
  Gameplay.nb_npc = 0;

  Gameplay.no_leave = 0;

  loadArea(7);
}

/**
 * `cleanArea` permet de libérer la mémoire des éléments affichés à l’écran et
 * permet de décharger la zone.\n Attention, d’éventuels pointeurs vers des
 * objets se situants dans `Gameplay.elements` seront invalides après exécution
 * de cette fonction
 */
void cleanArea() {
  free(Gameplay.elements);
  Gameplay.elements = NULL;
  Gameplay.nb_elements = 0;
}

/**
 * `loadArea` charge la zone de jeu correspondant à son identifiant donné par
 * argument et l’affiche à l’écran.
 *
 * @param area Identifiant de la zone à charger
 */
void loadArea(int area) {
  char path[64];
  sprintf(path, "Data/Zone%d.txt", area);

  FILE *file = fopen(path, "r");
  if (!file) {
	printf("%s : not found", path);
	assert(0);
  }

  cleanArea();

  while (!feof(file)) {
	Element elem;
	fscanf(file, "%d %s %d %d %d %d\n", &elem.type, elem.name, &elem.rect.x,
		   &elem.rect.y, &elem.value, &elem.value2);

	addElement(elem);
  }

  char file_name[12];
  sprintf(file_name, "Zone%d", area);

  loadImage(file_name, &Gameplay.bg_tex[0], &Gameplay.bg_rect[0]);

  sprintf(file_name, "IZone%d", area);
  loadImage(file_name, &Gameplay.bg_tex[1], &Gameplay.bg_rect[1]);

  Gameplay.area = area;
}

/**
 * Ajoute l’élément passé par argument à la liste des éléments de la zone
 * et l’affiche. Attention, des pointeurs sur `Gameplay.elements` peuvent
 * être invalidés par la fonction.
 *
 * @param elem Élément à rajouter à la zone actuelle
 */
void addElement(Element elem) {
  elem.tex = NULL;
  loadImage(elem.name, &elem.tex, &elem.rect);

  int size = Gameplay.nb_elements;
  Element *elems = Gameplay.elements;

  elems = realloc(elems, sizeof(*elems) * (size + 1));
  elems[size] = elem;

  Gameplay.elements = elems;
  Gameplay.nb_elements = size + 1;
}

/**
 * `intersects` teste si la souris se situe dans une certaine zone en se
 * basant sur sa position verticale et horizontale, ainsi que sur la
 * disposition du rectangle interactif.
 *
 * @param rect Canevas interactif testé
 * @param x_mouse Position horizontale de la souris
 * @param y_mouse Position verticale de la souris
 * @return Retourne 1 si la souris se situe sur le canevas, 0 sinon
 */
int intersects(SDL_Rect rect, int x_mouse, int y_mouse) {
  if (x_mouse >= rect.x && x_mouse <= rect.x + rect.w && y_mouse >= rect.y &&
	  y_mouse <= rect.y + rect.h)
	return 1;
  return 0;
}

/**
 * `elementTriggered` teste si un élément de l’écran est activé selon la
 * position de la souris passée en argument dans la fonction. Chaque élément
 * à l’écran sera successivement testé jusqu’à ce qu’un élément ne soit en
 * effet activé par le joueur.
 *
 * @param x Position horizontale de la souris
 * @param y Position verticale de la souris
 * @return Retourne 1 si un élément a été activé, 0 sinon
 */
int elementTriggered(int x, int y) {
  int i;
  for (i = 0; i < Gameplay.nb_elements; i++) {
	if (intersects(Gameplay.elements[i].rect, x, y)) {
	  processElement(i);
	  return 1;
	}
  }

  return 0;
}

/**
 * `buttonTriggered` teste si un bouton du menu a été activé ou non selon la
 * position de la souris au moment de l’appel de la fonction. En mode
 * exploration, le seul bouton disponible est celui de l’inventaire, sinon
 * tous les boutons sont successivement testés. Si un bouton est
 * effectivemenent activé, les boutons suivants ne le seront pas.
 *
 * @param x Position horizontale de la souris
 * @param y Position verticale de la souris
 * @param rects Liste des boutons du menu
 * @return Retourne 1 is un bouton a été activé, 0 sinon
 */
int buttonTriggered(int x, int y, SDL_Rect rects[]) {
  int i;
  for (i = 0; i < 4; i++) {
	if (Gameplay.state == STATE_EXPLORATION && i != 2)
	  continue;

	if (intersects(rects[i], x, y)) {
	  if (Gameplay.state == STATE_INVENTORY)
		processAction(i + 4);
	  else if (Gameplay.state == STATE_TALK)
		processAction(i + 8);
	  else
		processAction(i);

	  return 1;
	}
  }

  return 0;
}

/**
 * `itemTriggered` marque un objet comme étant sélectionné si ce dernier se
 * trouve sous la souris au moment de l’appel de la fonction. Chaque objet est
 * testé jusqu’à ce qu’un objet soit en effet sous la souris et sélectionné.
 *
 * @param x Position horizontale de la souris
 * @param y Position verticale de la souris
 * @param rects Liste des objets affichés à l’écran
 * @return Retourne 1 si un objet a été sélectionné, 0 sinon
 */
int itemTriggered(int x, int y, SDL_Rect rects[]) {
  int i;
  for (i = 0; i < MAX_ITEM; i++) {
	if (intersects(rects[i], x, y)) {
	  Gameplay.selected_item = Gameplay.items[i];
	  Gameplay.index_selected_item = i;
	  return 1;
	}
  }

  return 0;
}

/**
 * `processElement` gère l’élément se situant à l’index spécifié en argument
 * dans la liste des éléments de la variable globale @ref Gameplay. Si le type
 * de l’élément a pour valeur `1`, alors il s’agit d’une zone à charger ; sinon
 * il s’agit d’un élément de la zone qui est à gérer. Le jeu passe alors en
 * mode interactif. Si l’élément passé par argument correspond à un NPC, alors
 * une interaction avec le NPC est lancée. Sinon le NPC correspondant est
 * rajouté à la scène.
 * \n Attention, les pointeurs éventuels sur `Gameplay.npcs` peuvent être
 * invalidés après appel de la fonction.
 *
 * @param element_index Numéro de l’élément à gérer dans la fonction
 */
void processElement(int element_index) {
  Element elem = Gameplay.elements[element_index];

  if (elem.type == 0) {
	Gameplay.state = STATE_INTERACTION;
	Gameplay.interaction_index = element_index;

	int i;
	for (i = 0; i < Gameplay.nb_npc; i++) {
	  if (Gameplay.npcs[i].type == elem.value &&
		  elem.value2 == Gameplay.npcs[i].unique_id) {
		Gameplay.index_current_npc = i;
		Gameplay.npcs[i].status =
			npcResponse(&Gameplay.npcs[i], NONE, 0, Gameplay.name);
		return;
	  }
	}

	Gameplay.npcs =
		realloc(Gameplay.npcs, sizeof(*Gameplay.npcs) * (Gameplay.nb_npc + 1));
	Gameplay.npcs[Gameplay.nb_npc].unique_id = elem.value2;

	if (encounterInit(elem.value, &Gameplay.npcs[Gameplay.nb_npc],
					  Gameplay.name) == 1) {
	  printf("%d.txt not found", elem.value);
	  assert(0);
	}

	Gameplay.index_current_npc = Gameplay.nb_npc;
	Gameplay.nb_npc++;

	Gameplay.npcs[Gameplay.index_current_npc].status = npcResponse(
		&Gameplay.npcs[Gameplay.index_current_npc], NONE, 0, Gameplay.name);
  } else if (elem.type == 1) {
	loadArea(elem.value);
  }
}

/**
 * `processAction` gère l’action du joueur passée en argument en mettant à jour
 * ou non l’état du jeu ou en communiquant au joueur l’impossibilité d’une
 * action, ou la fin du jeu.
 *
 * @param action Action que le joueur souhaite effectuer
 */
void processAction(int action) {
  switch (action) {
  case ACTION_ATTACK: {
	int index = Gameplay.index_current_npc;

	if (!encounterEnd(Gameplay.npcs[index])) {
	  Gameplay.npcs[index].status =
		  npcResponse(&Gameplay.npcs[index], ATTACK, 3, Gameplay.name);
	  attaque(1, &Gameplay.npcs[index]);

	  if (encounterEnd(Gameplay.npcs[index])) {
		if (Gameplay.npcs[index].type == 501) {
		  EndGame(0);
		  return;
		} else if (Gameplay.npcs[index].type == 500) {
		  EndGame(1);
		  return;
		} else if (Gameplay.npcs[index].type == 110) {
		  Gameplay.no_leave = 0;
		}

		pushQueue("Your opponent died.");
		Gameplay.state = STATE_EXPLORATION;

		int add = rand() % 10 + 10;
		Gameplay.gold += add;
		addDialog("You earn %d gold.", add);
	  }

	  if (Gameplay.player_current_life <= 0) {
		EndGame(0);
		return;
	  }
	}

	break;
  }

  case ACTION_TALK: {
	Gameplay.state = STATE_TALK;
	break;
  }

  case ACTION_INVENTORY: {
	Gameplay.old_state = Gameplay.state;
	Gameplay.state = STATE_INVENTORY;
	break;
  }

  case ACTION_MOVE: {
	if (Gameplay.no_leave == 0)
	  Gameplay.state = STATE_EXPLORATION;
	else
	  pushQueue("You can't escape from bandit. You will have to kill him.");

	break;
  }

  case ACTION_INV_USE: {
	if (Gameplay.selected_item != 0) {
	  Item *item = getItemFromID(Gameplay.selected_item);
	  if (item->stat == STAT_NONE) {
		processItem(Gameplay.selected_item);
		Gameplay.items = inventoryDel(Gameplay.selected_item);

		if (Gameplay.old_state == STATE_INTERACTION) {
		  int index = Gameplay.index_current_npc;
		  Gameplay.npcs[index].status =
			  npcResponse(&Gameplay.npcs[index], ITEM, item->id, Gameplay.name);
		  Gameplay.state = STATE_INTERACTION;
		}
	  }

	  Gameplay.selected_item = 0;
	  Gameplay.index_selected_item = -1;
	}

	break;
  }

  case ACTION_INV_EQUIP: {
	if (Gameplay.selected_item != 0) {
	  Item *item = getItemFromID(Gameplay.selected_item);
	  if (item->stat == STAT_ATK) {
		int temp = Gameplay.stuff[0];
		Gameplay.player_atk -= getItemFromID(Gameplay.stuff[0])->value_stat;

		Gameplay.stuff[0] = Gameplay.selected_item;
		Gameplay.items = inventoryDel(Gameplay.selected_item);
		Gameplay.items = inventoryAdd(temp);

		Gameplay.player_atk += item->value_stat;
	  } else if (item->stat == STAT_DEF) {
		int temp = Gameplay.stuff[1];
		Gameplay.player_def -= getItemFromID(Gameplay.stuff[1])->value_stat;

		Gameplay.stuff[1] = Gameplay.selected_item;
		Gameplay.items = inventoryDel(Gameplay.selected_item);
		Gameplay.items = inventoryAdd(temp);

		Gameplay.player_def += item->value_stat;
	  }

	  Gameplay.selected_item = 0;
	  Gameplay.index_selected_item = -1;
	}

	break;
  }

  case ACTION_INV_THROW: {
	if (Gameplay.selected_item != 0) {
	  Gameplay.items = inventoryDel(Gameplay.selected_item);
	  Gameplay.selected_item = 0;
	  Gameplay.index_selected_item = -1;
	}
	break;
  }

  case ACTION_INV_QUIT: {
	Gameplay.selected_item = 0;
	Gameplay.index_selected_item = -1;
	Gameplay.state = Gameplay.old_state;
	break;
  }

  case ACTION_TALK_YES: {
	int index = Gameplay.index_current_npc;
	Gameplay.npcs[index].status =
		npcResponse(&Gameplay.npcs[index], TALK, YES, Gameplay.name);
	break;
  }

  case ACTION_TALK_NO: {
	int index = Gameplay.index_current_npc;
	Gameplay.npcs[index].status =
		npcResponse(&Gameplay.npcs[index], TALK, NO, Gameplay.name);
	break;
  }

  case ACTION_TALK_THREAT: {
	int index = Gameplay.index_current_npc;
	Gameplay.npcs[index].status =
		npcResponse(&Gameplay.npcs[index], TALK, THREAT, Gameplay.name);
	break;
  }

  case ACTION_TALK_QUIT: {
	Gameplay.state = STATE_INTERACTION;
	break;
  }
  }
}

/**
 * Gère l’utilisation d’un objet par le joueur. L’objet d’ID 204 lui fait
 * perdre 10 points de vie, l’objet d’ID 205 dans la zone 5 met fin à la
 * partie (victoire).
 *
 * @param item_id Identifiant de l’objet à gérer
 */
void processItem(int item_id) {
	Item *item = getItemFromID(item_id);
	if (item->id == 204) {
		Gameplay.player_current_life -= 10;
	} else if (item->id == 205 && Gameplay.area == 5) {
		EndGame(1);
	} else if (item->id == ITEM_CUPCAKE) {
		Gameplay.player_current_life = Gameplay.player_max_life;
	}
}

/**
 * Renvoie la texture d’un objet se situant à l’écran. Si l’argument
 * `inventory` est mis à 1, alors on cherche la texture d’un élément de
 * l’inventaire, sinon on cherche un élément de l’équipement porté par le
 * joueur.
 *
 * @param i ième élément de l’inventaire ou de l’équipement du joueur
 * @param inventory 1 pour chercher dans l’inventaire, 0 pour le stuff
 * @return Pointeur sur la texture de l’objet voulu, `NULL` s’il n’existe pas
 */
SDL_Texture *getItemTexture(int i, int inventory) {
  if (inventory == 1) {
	if (Gameplay.items[i] == 0)
	  return NULL;

	return getItemFromID(Gameplay.items[i])->tex;
  } else {
	if (Gameplay.stuff[i] == 0)
	  return NULL;

	return getItemFromID(Gameplay.stuff[i])->tex;
  }
}

/**
 * @brief Renvoie la description de l’objet sélectionné
 *
 * Renvoie via son paramètre la description de l’objet actuellement sélectionné
 * dans l’état de jeu. Si aucun objet n’est sélectionné, une chaine nulle est
 * renvoyée. Attention, cette fonction utilise `strcpy`, tout pointeur sur une
 * chaine précédente est invalidé.
 *
 * @param[out] desc Description de l’objet sélectionné
 */
void getCurrentItemDesc(char *desc) {
  if (Gameplay.selected_item != 0) {
	Item *item = getItemFromID(Gameplay.selected_item);
	strcpy(desc, item->description);
  } else
	desc[0] = '\0';
}

/**
 * Change l’état du jeu, ayant pour incidence la fin de la partie. Si
 * l’argument `successful` est non-nul, alors le joueur a gagné. Sinon, le
 * joueur a perdu.
 *
 * @param successful Jeu gagné si non-nul, sinon jeu perdu
 */
void EndGame(int successful) {
  if (successful) {
	Gameplay.state = STATE_WON;
  } else {
	Gameplay.state = STATE_LOST;
  }
}

/**
 * `buyItem` gère l’achat d’objet par le joueur, en vérifiant si le joueur a
 * assez d’argent, et le cas échéant ajoute à son inventaire les objets
 * correspondants tout en retirant de sa bourse le prix de l’objet.
 * @param item Identifiant de l’objet que le joueur souhaite acheter
 * @param gold Valeur de l’objet
 */
void buyItem(int item, int gold) {
	if (item > 200) {
		if (Gameplay.gold >= gold) {
			inventoryAdd(item);
			Gameplay.gold -= gold;
		}
	}
	if (item == ITEM_NONE) {
		Gameplay.gold -= gold;
		if (Gameplay.gold < 0)
			Gameplay.gold = 0;
	}
	if (item == ITEM_BEER) {
		if (Gameplay.gold >= 5) {
			inventoryAdd(204);
			Gameplay.gold -= 5;
		}
	}
	if (item == ITEM_CLETTER) {
		inventoryAdd(202);
	}
	if (item == ITEM_PLETTER) {
		inventoryAdd(201);
	}
	if (item == ITEM_AXE) {
		if (Gameplay.gold >= 50) {
			inventoryAdd(51);
			Gameplay.gold -= 50;
		}
	}
	if (item == ITEM_ARMOR) {
		if (Gameplay.gold >= 50) {
			inventoryAdd(40);
			Gameplay.gold -= 50;
		}
	}
	if (item == ITEM_POISON) {
		if (Gameplay.gold >= 50) {
			inventoryAdd(205);
			Gameplay.gold -= 50;
		}
	}
}
