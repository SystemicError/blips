/* ai_directives.c */
/* Started 9/27/12 by Tevis Tsai. */

#include"ai_directives.h"

creature* ai_directives_get_nearest_creature(blips_game *bgame,creature *cr)
{
	creature *enemy_cr;
	int i;
	double cr_abs_x,cr_abs_y;
	double distance_squared,candidate_distance_squared;
	double candidate_x,candidate_y;

	/* compute this creatures absolute position */

	cr_abs_x=creature_absolute_x(cr);
	cr_abs_y=creature_absolute_y(cr);

	/* get nearest enemy creature */

	distance_squared=-1;

		/* search non-player creatures */
	for(i=0;i<bgame->num_creatures;i++)
		if(bgame->creatures[i]->team!=cr->team && bgame->creatures[i]->current_health>0)
		{
			candidate_x=creature_absolute_x(bgame->creatures[i]);
			candidate_y=creature_absolute_y(bgame->creatures[i]);
			candidate_distance_squared=pow(candidate_x-cr_abs_x,2)+pow(candidate_y-cr_abs_y,2);

			/* if distance to this creatures is less than distance, change ptr */
			if(distance_squared>candidate_distance_squared ||
			   distance_squared==-1)
			{
				enemy_cr=bgame->creatures[i];
				distance_squared=candidate_distance_squared;
			}
		}

		/* search player creatures */
	for(i=0;i<bgame->campaign->num_players;i++)
		if(bgame->players[i]->team!=cr->team && bgame->players[i]->current_health>0)
		{
			candidate_x=creature_absolute_x(bgame->players[i]);
			candidate_y=creature_absolute_y(bgame->players[i]);
			candidate_distance_squared=pow(candidate_x-cr_abs_x,2)+pow(candidate_y-cr_abs_y,2);

			/* if distance to this creatures is less than distance, change ptr */
			if(distance_squared>candidate_distance_squared ||
			   distance_squared==-1)
			{
				enemy_cr=bgame->players[i];
				distance_squared=candidate_distance_squared;
			}
		}

	if(distance_squared==-1)
		return 0;
	return enemy_cr;
}

projectile* ai_directives_get_nearest_projectile(blips_game *bgame,creature *cr)
{
	double distance_squared,candidate_distance_squared;
	projectile *enemy_pr;
	double cr_abs_x,cr_abs_y;
	double candidate_x,candidate_y;
	int i;

	/* compute this creatures absolute position */

	cr_abs_x=creature_absolute_x(cr);
	cr_abs_y=creature_absolute_y(cr);

	/* get nearest enemy projectile */

	distance_squared=-1;

	for(i=0;i<bgame->num_projectiles;i++)
		if(bgame->projectiles[i]->team!=cr->team)
		{
			candidate_x=projectile_absolute_x(bgame->projectiles[i]);
			candidate_y=projectile_absolute_y(bgame->projectiles[i]);
			candidate_distance_squared=pow(candidate_x-cr_abs_x,2)+pow(candidate_y-cr_abs_y,2);

			/* if distance to this creatures is less than distance, change ptr */
			if(distance_squared>candidate_distance_squared ||
			   distance_squared==-1)
			{
				enemy_pr=bgame->projectiles[i];
				distance_squared=candidate_distance_squared;
			}
		}

	if(distance_squared==-1)
		return 0;
	return enemy_pr;
}

void ai_directives_move_dodge(blips_game *bgame,creature *cr,projectile *enemy_pr)
{
	if(enemy_pr)
	{
		cr->current_move_speed=cr->type->move_speed;

		/* if the cross product of the pr's vel and
		 * the vector from pr to cr has positive z,
		 * then we should SUBTRACT M_PI/2 from orientation */

		/* compute cross product's z */
		if(cos(enemy_pr->orientation)*(projectile_absolute_y(enemy_pr)-creature_absolute_y(cr))-
		   sin(enemy_pr->orientation)*(projectile_absolute_x(enemy_pr)-creature_absolute_x(cr))<0)
			cr->move_orientation=enemy_pr->orientation+M_PI/2.0;
		else
			cr->move_orientation=enemy_pr->orientation-M_PI/2.0;
	}
	else
		cr->current_move_speed=0;
	return;
}

void ai_directives_move_flee(blips_game *bgame,creature *cr,creature *enemy_cr)
{
	if(enemy_cr)
	{
		cr->current_move_speed=cr->type->move_speed;
		cr->move_orientation=atan2(creature_absolute_y(enemy_cr)-creature_absolute_y(cr),
					   creature_absolute_x(enemy_cr)-creature_absolute_x(cr))+
				     M_PI;
	}
	else
		cr->current_move_speed=0;
	return;
}

void ai_directives_move_close(blips_game *bgame,creature *cr,creature *enemy_cr)
{
	if(enemy_cr)
	{
		cr->current_move_speed=cr->type->move_speed;
		cr->move_orientation=atan2(creature_absolute_y(enemy_cr)-creature_absolute_y(cr),
					   creature_absolute_x(enemy_cr)-creature_absolute_x(cr));
	}
	else
		cr->current_move_speed=0;
	return;
}

void ai_directives_move_wander(blips_game *bgame,creature *cr)
{
	cr->current_move_speed=cr->type->move_speed/2;
	if(blips_game_move_creature(bgame,cr))
		cr->move_orientation=rand();
	return;
}

void ai_directives_move_patrol_ns(blips_game *bgame,creature *cr)
{
	if(abs(cos(cr->move_orientation))>.01)
		cr->move_orientation=M_PI/2.0;
	cr->current_move_speed=cr->type->move_speed/2;
	if(blips_game_move_creature(bgame,cr))
		cr->move_orientation+=M_PI;
	return;
}

void ai_directives_move_patrol_ew(blips_game *bgame,creature *cr)
{
	if(abs(sin(cr->move_orientation))>.01)
		cr->move_orientation=M_PI;
	cr->current_move_speed=cr->type->move_speed/2;
	if(blips_game_move_creature(bgame,cr))
		cr->move_orientation+=M_PI;
	return;
}

void ai_directives_aim_face(blips_game *bgame,creature *cr,creature *enemy_cr)
{
	cr->move_orientation=atan2(creature_absolute_y(enemy_cr)-creature_absolute_y(cr),
				   creature_absolute_x(enemy_cr)-creature_absolute_x(cr));
	return;
}

void ai_directives_aim_spin(blips_game *bgame,creature *cr)
{
	cr->aim_orientation+=.01;
	return;
}

void ai_directives_aim_lag_face(blips_game *bgame,creature *cr,creature *enemy_cr)
{
	/* sine of difference in orientation and angle of vector to target
	 * determines whether to add or subtract to current orientation. */
	double target_angle,sine_of;

	target_angle=atan2(creature_absolute_y(enemy_cr)-creature_absolute_y(cr),
			   creature_absolute_x(enemy_cr)-creature_absolute_x(cr));

	sine_of=sin(cr->aim_orientation-target_angle);

	if(sine_of<0)
		cr->aim_orientation-=.01;
	else if(sine_of>0)
		cr->aim_orientation+=.01;
	return;
}
void ai_directives_aim_lead_face(blips_game *bgame,creature *cr,creature *enemy_cr)
{
/*UNFINISHED*/
	/* possible locations of fired projectile are a cone in hyperspace,
	 * and projected location of enemy is a hyperline.  Aim where they
	 * intersect. */
	return;
}

void ai_directives_aim_as_velocity(blips_game *bgame,creature *cr)
{
	cr->aim_orientation=cr->move_orientation;
	return;
}

void ai_directives_fire_spam(blips_game *bgame,creature *cr)
{
	cr->fire_cycle_state=(cr->fire_cycle_state+1)%(cr->type->fire_delay);
	return;
}

void ai_directives_fire_sight_first(blips_game *bgame,creature *cr,creature *enemy_cr)
{
	/* if dot product of vector to target and orientation is near 1,
	 * fire */
	if(cos(cr->aim_orientation)*cos(creature_absolute_x(enemy_cr)-creature_absolute_x(cr))+
	   sin(cr->aim_orientation)*sin(creature_absolute_y(enemy_cr)-creature_absolute_y(cr))>.9)
		cr->fire_cycle_state=(cr->fire_cycle_state+1)%(cr->type->fire_delay);
	return;
}

void ai_directives_fire_spurt(blips_game *bgame,creature *cr,creature *enemy_cr)
{
/*UNFINISHED*/
	return;
}

