#pragma once

#include "engine_comp.hpp"
#include "box2d_system.hpp"
#include "world_tile_system.hpp"

namespace game
{
	extern std::mutex projectile_mutex;
	struct projectile : public game_engine::component
	{
		b2Body *body;
		projectile() = default;
		projectile(b2Body *b) : body(b) {}
		tile_type debri_tile_type;
		tile_type permanent_trail_tile_type;
		tile_type temporary_trail_tile_type;
		int16_t tile_temperature;
		uint16_t tile_misc_data = 0;
		float fall_speed_multiplier = 1.0f;
		
		uint64_t last_collided_time = 0;
	};

	struct projectile_system : public game_engine::system
	{
	private:
		game_engine::sparse_component_set<projectile> projectiles;

	public:
		projectile_system() = default;
		void update() {}
		void update(uint64_t tick_count);

		projectile &create_projectile(entity ent, float x, float y, float ang, float vel, float radius, uint16_t millis_lifetime, b2fixture_types projectile_type);
		void add_projectile(entity ent, projectile proj);
		void remove_projectile(entity ent);
		projectile &get_projectile(entity ent);
		bool contains(entity ent) { return projectiles.contains(ent); }
		
	};
}