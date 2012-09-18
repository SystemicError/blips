blips : main.o blips_gui.o blips_game.o blips_campaign.o blips_input_state.o breakable_media_set.o collectible_media_set.o creature_media_set.o projectile_media_set.o ai_type.o breakable_type.o collectible_type.o creature_type.o projectile_type.o world_tile.o maze.o
	gcc -o blips main.o blips_gui.o blips_game.o blips_campaign.o blips_input_state.o breakable_media_set.o collectible_media_set.o creature_media_set.o projectile_media_set.o ai_type.o breakable_type.o collectible_type.o creature_type.o projectile_type.o world_tile.o maze.o `sdl-config --cflags --libs` `pkg-config --libs cairo` -lSDL_image -lSDL_mixer -lcairo
ai_type.o : ai_type.h ai_type.c
	gcc -c ai_type.c
breakable_type.o : breakable_type.h breakable_type.c
	gcc -c breakable_type.c
collectible_type.o : collectible_type.h collectible_type.c
	gcc -c collectible_type.c
creature_type.o : creature_type.h creature_type.c
	gcc -c creature_type.c
projectile_type.o : projectile_type.h projectile_type.c
	gcc -c projectile_type.c
breakable_media_set.o : breakable_media_set.h breakable_media_set.c
	gcc -c breakable_media_set.c
collectible_media_set.o : collectible_media_set.h collectible_media_set.c
	gcc -c collectible_media_set.c
creature_media_set.o : creature_media_set.h creature_media_set.c
	gcc -c creature_media_set.c
projectile_media_set.o : projectile_media_set.h projectile_media_set.c
	gcc -c projectile_media_set.c
world_tile.o : world_tile.h world_tile.c maze.h
	gcc -c world_tile.c
maze.o : maze.h maze.c
	gcc -c maze.c
blips_input_state.o : blips_input_state.h blips_input_state.c
	gcc -c blips_input_state.c
blips_campaign.o : blips_campaign.h blips_campaign.c
	gcc -c blips_campaign.c
blips_game.o : blips_game.h blips_game.c
	gcc -c blips_game.c `sdl-config --cflags` `pkg-config --cflags cairo`
blips_gui.o : blips_gui.h blips_gui.c blips_game.h
	gcc -c blips_gui.c `sdl-config --cflags` `pkg-config --cflags cairo`
main.o : main.c blips_gui.h blips_game.h
	gcc -c main.c `sdl-config --cflags` `pkg-config --cflags cairo`
clean :
	rm blips *.o;
