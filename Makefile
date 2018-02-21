LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf
FLAGS = -W -Wall

FILES = Main.c Graphics.c Gameplay.c Inventory.c Npc.c

OBJS = $(FILES:%.c=%.o)

all: $(OBJS)
	gcc -o 4A $(OBJS) $(LIBS)

$%.o: %.c
	gcc -c $< -o $@ $(FLAGS)

clean:
	rm -rf *.o
