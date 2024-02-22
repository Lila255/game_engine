#pragma once

#include <unordered_map>

#include "engine_comp.hpp"
#include "world_tile_system.hpp"

namespace game
{
	struct tree_tracer
	{
		uint8_t up, down, left, right;
	};

	struct tree : public game_engine::component
	{
		uint32_t seed_x, seed_y;
		std::unordered_map<tile_coord, tree_tracer, tile_coord_hash> branch_tiles;
		std::unordered_map<tile_coord, tree_tracer, tile_coord_hash> root_tiles;
	};

	struct tree_system : public game_engine::system
	{
	private:
		game_engine::sparse_component_set<tree> trees;
		world_tile_system *world_tiles;
		bool running = true;

		bool can_place_tree_tile_at(int x, int y, tile_type tile_type);
		bool find_tile_to_grow_to(tree &t, tile_coord &current_tile, tile_coord &last_tile);
		void trim_tree(tree &t, tile_coord start_tile);
		
	public:
		tree_system() = delete;
		tree_system(world_tile_system *wts) : world_tiles(wts) {}
		void update();

		void add_tree(entity ent, tree t);
		void remove_tree(entity ent);
		tree &get_tree(entity ent);
		void start();
		void set_running(bool run) { running = run; }
	};
}