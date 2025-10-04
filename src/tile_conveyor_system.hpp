#pragma once


#include "engine_comp.hpp"
#include "tile_conveyor.hpp"
#include "world_tile_system.hpp"
#include "tile_pathfinding_system.hpp"

namespace game
{
	struct tile_conveyor_system : public game_engine::system
	{
	private:
		game_engine::sparse_component_set<tile_conveyor> conveyors;
		bool running = false;

		world_tile_system *world_tile_sys = nullptr;
		tile_pathfinding_system *tile_pathfinding_sys = nullptr;
		std::pair<int, int> get_next_conveyor_tooth_position(tile_conveyor &conveyor, int x, int y);
	
	public:
		tile_conveyor_system(world_tile_system *world_tile_sys, tile_pathfinding_system *tile_pathfinding_sys) : world_tile_sys(world_tile_sys), tile_pathfinding_sys(tile_pathfinding_sys) {}
		void update(){};
		void update(uint64_t tick_count);

		void add_component(entity ent, tile_conveyor &tc);

		tile_conveyor &get(entity ent) { return conveyors.get(ent); }
		void start_thread();
		void set_running(bool r) { running = r; }
	};

}