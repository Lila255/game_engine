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
		create_debris_params(float x_pos, float y_pos, float vel_x, float vel_y, float radius, uint8_t debri_tile_type, uint8_t perm_tile_type, uint8_t temp_tile_type, uint16_t millis_lifetime) : x(x_pos), y(y_pos), v_x(vel_x), v_y(vel_y), r(radius), debri_tile_type(debri_tile_type), perm_tile_type(perm_tile_type), temp_tile_type(temp_tile_type), millis_lifetime(millis_lifetime) {}
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
}