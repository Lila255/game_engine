#pragma once

#include <stdio.h>
#include <string>

#include "projectile_system.hpp"
#include "box2d_system.hpp"
#include "world_tile_system.hpp"

#define entity uint32_t

namespace game
{
	void shutdown_task_schedular_task(void * parameters);
	
	struct create_debris_params
	{
		float x, y, v_x, v_y, r;
		uint16_t millis_lifetime;
		uint8_t debri_tile_type, perm_tile_type, temp_tile_type;
		int16_t tile_temperature;
		create_debris_params(float x_pos, float y_pos, float vel_x, float vel_y, float radius, uint8_t debri_tile_type, uint8_t perm_tile_type, uint8_t temp_tile_type, uint16_t millis_lifetime, int16_t tile_temp) : x(x_pos), y(y_pos), v_x(vel_x), v_y(vel_y), r(radius), debri_tile_type(debri_tile_type), perm_tile_type(perm_tile_type), temp_tile_type(temp_tile_type), millis_lifetime(millis_lifetime), tile_temperature(tile_temp) {}
	};
	void create_debris_task(void *parameters);
	void create_single_debris_task(void *parameters);

	struct delete_circle_params
	{
		int x, y, radius;
		delete_circle_params(int x_pos, int y_pos, int rad) : x(x_pos), y(y_pos), radius(rad){}
	};
	void delete_circle_task(void *parameters);

	struct update_tile_params
	{
		int x, y;
		tile_type m_tile_type;
		update_tile_params(int x, int y, tile_type t) : x(x), y(y), m_tile_type(t) {}
	};
	void update_tile_task(void * parameters);


	struct create_flying_creature_nest_params
	{
		int x, y;
		create_flying_creature_nest_params(int x_pos, int y_pos) : x(x_pos), y(y_pos) {}
	};
	void create_flying_creature_nest_task(void *parameters);

	struct flying_creature_eat_params
	{
		int x, y, ent;
		flying_creature_eat_params(int x_pos, int y_pos, entity e) : x(x_pos), y(y_pos), ent(e) {}
	};
	void flying_creature_eat_task(void *parameters);

	struct flying_creature_deposit_params
	{
		int x, y, ent;
		flying_creature_deposit_params(int x_pos, int y_pos, entity e) : x(x_pos), y(y_pos), ent(e) {}
	};
	void flying_creature_deposit_task(void *parameters);

	struct legged_creature_step_params
	{
		entity legged_creature_entity;
		entity foot_entity;
		uint64_t leg_index;

		legged_creature_step_params(entity legged_creature_ent, entity foot_ent, uint64_t leg_i) : legged_creature_entity(legged_creature_ent), foot_entity(foot_ent), leg_index(leg_i) {}
	};
	void legged_creature_step_task(void *parameters);
}