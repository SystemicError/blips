blips : main.o blips_gui.o blips_game.o blips_input_state.o breakable_media_set.o collectible_media_set.o creature_media_set.o
	gcc -o main.o blips_gui.o blips_game.o blips_input_state.o breakable_media_set.o collectible_media_set.o creature_media_set.o `sdl-config --cflags --libs` `pkg-config --libs cairo` -lSDL_image -lSDL_mixer -lcairo
breakable_media_set.o : breakable_media_set.h breakable_media_set.c
	gcc -c breakable_media_set.c
collectible_media_set.o : collectible_media_set.h collectible_media_set.c
	gcc -c collectible_media_set.c
creature_media_set.o : creature_media_set.h creature_media_set.c
	gcc -c creature_media_set.c
projectile_media_set.o : projectile_media_set.h projectile_media_set.c
	gcc -c projectile_media_set.c
blips_input_state.o : blips_input_state.h blips_input_state.c
	gcc -c blips_input_state.c
blips_game.o : blips_game.h blips_game.c
	gcc -c blips_game.c `sdl-config --cflags` `pkg-config --cflags cairo`
blips_gui.o : blips_gui.h blips_gui.c blips_game.h
	gcc -c blips_gui.c `sdl-config --cflags` `pkg-config --cflags cairo`
main.o : main.c blips_gui.h blips_game.h
	gcc -c main.c `sdl-config --cflags` `pkg-config --cflags cairo`
clean :
	rm blips *.o;
