#LIBS = -lSDL2 -lSDL2_image -lSDL2_ttf 
LIBS = $(shell pkg-config --libs SDL2_image SDL2_ttf)
FLAGS = -W -Wall -D_THREAD_SAFE -I/opt/local/include

FILES = Main.c Graphics.c Gameplay.c Inventory.c Npc.c

OBJS = $(FILES:%.c=%.o)

all: $(OBJS)
	gcc -o 4A $(OBJS) $(LIBS) 

%.o: %.c
	gcc -c $< -o $@ $(FLAGS)

clean:
	rm -rf *.o
