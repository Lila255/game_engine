#pragma once

#include <unordered_map>

#include "engine_comp.hpp"
#include "world_tile_system.hpp"

namespace game
{
	extern std::mutex tree_mutex;
	struct tree_tracer
	{
		uint8_t up, down, left, right;
	};

	enum tree_type
	{
		NODE_TREE,
		TILE_TREE
	};

	struct tree : public game_engine::component
	{
		int32_t seed_x, seed_y;
		uint32_t stored_pollution = 0;
		uint32_t placed_tiles = 0;
		tree_type type = NODE_TREE;
		std::unordered_map<tile_coord, tree_tracer, global_tile_coord_hash> branch_tiles;

		// second type of branch structure that stores nodes that we draw branches between. Used for larger trees. Want to also store how many child branches each node has
		std::unordered_map<tile_coord, uint32_t, global_tile_coord_hash> child_node_counts;
		std::unordered_map<tile_coord, std::vector<tile_coord>, global_tile_coord_hash> child_nodes;

		std::unordered_map<tile_coord, tree_tracer, global_tile_coord_hash> root_tiles;
	};

	struct tree_system : public game_engine::system
	{
	private:
		game_engine::sparse_component_set<tree> trees;
		world_tile_system *world_tiles;
		bool running = true;

		bool can_place_tree_tile_at(int x, int y, tile_type tile_type);
		bool find_tile_to_grow_to(tree &t, tile_coord &current_tile, tile_coord &last_tile, tile_type tree_tile_type);
		void trim_tree(tree &t, tile_coord start_tile);
		bool try_grow_node_tree_recursive(tree &t, tile_coord &current_tile);
		void place_tiles_for_node_tree(tree &t, tile_coord from_tile, tile_coord to_tile, tile_type tree_tile_type, uint32_t &tiles_to_place);
		std::mutex tree_mutex;

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