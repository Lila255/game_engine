#pragma once

#include <array>
#include <queue>
#include <shared_mutex>

#include "engine_comp.hpp"

#include "world_tile_system.hpp"
#include "tile_pathfinding.hpp"

namespace game
{



	struct tile_pathfinding_system : public game_engine::system
	{
	private:
		world_tile_system *world_tiles;
		game_engine::sparse_component_set<tile_pathfinding> pathfinding_components;
		bool running = false;

	public:
		tile_pathfinding_system(world_tile_system *world_tiles) : world_tiles(world_tiles) {}
		void update() override;
		void start_thread();
		void add_component(entity ent, tile_pathfinding &tp) { pathfinding_components.add(ent, tp); }
		void set_running(bool r);
		tile_pathfinding &get(entity ent) { return pathfinding_components.get(ent); }
	};
}