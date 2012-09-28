blips : main.o blips_gui.o blips_game.o blips_campaign.o blips_input_state.o breakable_media_set.o collectible_media_set.o creature_media_set.o projectile_media_set.o ai_type.o breakable_type.o collectible_type.o creature_type.o projectile_type.o world_tile.o maze.o sprite_animation.o breakable.o collectible.o creature.o projectile.o string_map.o ai_directives.o
	gcc -g -o blips main.o blips_gui.o blips_game.o blips_campaign.o blips_input_state.o breakable_media_set.o collectible_media_set.o creature_media_set.o projectile_media_set.o ai_type.o breakable_type.o collectible_type.o creature_type.o projectile_type.o world_tile.o maze.o sprite_animation.o breakable.o collectible.o creature.o projectile.o string_map.o ai_directives.o `sdl-config --cflags --libs` `pkg-config --libs cairo` -lSDL_image -lSDL_mixer -lcairo
breakable.o : breakable.h breakable.c
	gcc -g -c breakable.c
collectible.o : collectible.h collectible.c
	gcc -g -c collectible.c
creature.o : creature.h creature.c
	gcc -g -c creature.c
projectile.o : projectile.h projectile.c
	gcc -g -c projectile.c
ai_type.o : ai_type.h ai_type.c
	gcc -g -c ai_type.c
breakable_type.o : breakable_type.h breakable_type.c
	gcc -g -c breakable_type.c
collectible_type.o : collectible_type.h collectible_type.c
	gcc -g -c collectible_type.c
creature_type.o : creature_type.h creature_type.c
	gcc -g -c creature_type.c
projectile_type.o : projectile_type.h projectile_type.c
	gcc -g -c projectile_type.c
breakable_media_set.o : breakable_media_set.h breakable_media_set.c
	gcc -g -c breakable_media_set.c `sdl-config --cflags` `pkg-config --cflags cairo`
collectible_media_set.o : collectible_media_set.h collectible_media_set.c
	gcc -g -c collectible_media_set.c `sdl-config --cflags` `pkg-config --cflags cairo`
creature_media_set.o : creature_media_set.h creature_media_set.c
	gcc -g -c creature_media_set.c `sdl-config --cflags` `pkg-config --cflags cairo`
projectile_media_set.o : projectile_media_set.h projectile_media_set.c
	gcc -g -c projectile_media_set.c `sdl-config --cflags` `pkg-config --cflags cairo`
sprite_animation.o : sprite_animation.h sprite_animation.c
	gcc -g -c sprite_animation.c `sdl-config --cflags` `pkg-config --cflags cairo`
world_tile.o : world_tile.h world_tile.c maze.h
	gcc -g -c world_tile.c
maze.o : maze.h maze.c
	gcc -g -c maze.c
string_map.o : string_map.h string_map.c
	gcc -g -c string_map.c
blips_input_state.o : blips_input_state.h blips_input_state.c
	gcc -g -c blips_input_state.c
blips_campaign.o : blips_campaign.h blips_campaign.c
	gcc -g -c blips_campaign.c
ai_directives.o : ai_directives.h ai_directives.c
	gcc -g -c ai_directives.c
blips_game.o : blips_game.h blips_game.c
	gcc -g -c blips_game.c `sdl-config --cflags` `pkg-config --cflags cairo`
blips_gui.o : blips_gui.h blips_gui.c blips_game.h
	gcc -g -c blips_gui.c `sdl-config --cflags` `pkg-config --cflags cairo`
main.o : main.c blips_gui.h blips_game.h
	gcc -g -c main.c `sdl-config --cflags` `pkg-config --cflags cairo`
clean :
	rm blips *.o;
