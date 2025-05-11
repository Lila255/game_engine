#pragma once

#include "world_tile_system.hpp"
#include "chunk.hpp"
#include "building_component.hpp"
#include "projectile_system.hpp"

#include "PerlinNoise.hpp"
#include <vector>

namespace game
{
	struct building_component_system : public game_engine::system
	{
	private:
		game_engine::sparse_component_set<building_component> building_components;
		world_tile_system *world_tile_sys;
		game_engine::box_system *box_sys;
		projectile_system *projectile_sys;

		std::pair<int, int> get_box_coordinate(entity);
		
	public:
		bool running = 0;
		building_component_system(world_tile_system *world_system, game_engine::box_system *box_system, projectile_system *proj_sys) : world_tile_sys(world_system), box_sys(box_system), projectile_sys(proj_sys)
		{

		}
		~building_component_system();

		void update();
		void update(uint64_t tick_count);

		void place_building_component(entity ent);

		void add(entity ent, building_component &building_comp);

		void start_thread();
	};
}
