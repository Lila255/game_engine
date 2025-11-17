#pragma once

#include <stdio.h>
#include <string>

#include "projectile_system.hpp"
#include "box2d_system.hpp"
#include "world_tile_system.hpp"

#define entity uint32_t

namespace game
{
	bool shutdown_task_schedular_task(void *parameters);

	struct create_debris_params
	{
		float x, y, v_x, v_y, r;
		uint16_t millis_lifetime;
		uint8_t debri_tile_type, perm_tile_type, temp_tile_type;
		float tile_temperature;
		uint16_t misc_data = 0;
		create_debris_params(float x_pos
			, float y_pos
			, float vel_x
			, float vel_y
			, float radius
			, uint8_t debri_tile_type
			, uint8_t perm_tile_type
			, uint8_t temp_tile_type
			, uint16_t millis_lifetime
			, float tile_temp
			, uint16_t misc_data_value
		) : x(x_pos)
			, y(y_pos)
			, v_x(vel_x)
			, v_y(vel_y)
			, r(radius)
			, debri_tile_type(debri_tile_type)
			, perm_tile_type(perm_tile_type)
			, temp_tile_type(temp_tile_type)
			, millis_lifetime(millis_lifetime)
			, tile_temperature(tile_temp)
			, misc_data(misc_data_value)
			{}
	};
	bool create_debris_task(void *parameters);
	bool create_single_debris_task(void *parameters);

	struct delete_circle_params
	{
		int x, y, radius;
		delete_circle_params(int x_pos, int y_pos, int rad) : x(x_pos), y(y_pos), radius(rad) {}
	};
	bool delete_circle_task(void *parameters);

	struct update_tile_params
	{
		int x, y;
		tile_type m_tile_type;
		float m_tile_temperature;
		uint16_t m_tile_misc_data;
		update_tile_params(int x, int y, tile_type t, float temp, uint16_t misc) : x(x), y(y), m_tile_type(t), m_tile_temperature(temp), m_tile_misc_data(misc) {}
	};
	bool update_tile_task(void *parameters);

	// add the tile flush to as many existing solid tiles as possible
	bool update_tile_flush_task(void *parameters);

	struct create_flying_creature_nest_params
	{
		int x, y;
		create_flying_creature_nest_params(int x_pos, int y_pos) : x(x_pos), y(y_pos) {}
	};
	bool create_flying_creature_nest_task(void *parameters);

	struct flying_creature_eat_params
	{
		int x, y, ent;
		flying_creature_eat_params(int x_pos, int y_pos, entity e) : x(x_pos), y(y_pos), ent(e) {}
	};
	bool flying_creature_eat_task(void *parameters);

	struct flying_creature_deposit_params
	{
		int x, y, ent;
		flying_creature_deposit_params(int x_pos, int y_pos, entity e) : x(x_pos), y(y_pos), ent(e) {}
	};
	bool flying_creature_deposit_task(void *parameters);

	struct legged_creature_step_params
	{
		entity legged_creature_entity;
		entity foot_entity;
		uint64_t leg_index;

		legged_creature_step_params(entity legged_creature_ent, entity foot_ent, uint64_t leg_i) : legged_creature_entity(legged_creature_ent), foot_entity(foot_ent), leg_index(leg_i) {}
	};
	bool legged_creature_step_task(void *parameters);

	struct set_b2d_body_enabled_params
	{
		entity ent;
		bool enabled;

		set_b2d_body_enabled_params(entity e, bool en) : ent(e), enabled(en) {}
	};
	bool set_b2d_body_enabled_task(void *parameters);
}