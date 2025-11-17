#include <algorithm>
#include "tree_system.hpp"
namespace game
{
	std::random_device rd;
	std::mt19937 g(rd());
	std::mutex tree_mutex;

	void tree_system::add_tree(entity ent, tree t)
	{
		std::lock_guard<std::mutex> lock(tree_mutex);
		trees.add(ent, t);
	}

	void tree_system::remove_tree(entity ent)
	{
		std::lock_guard<std::mutex> lock(tree_mutex);
		trees.remove(ent);
	}

	tree &tree_system::get_tree(entity ent)
	{
		return trees.get_component(ent);
	}

	bool tree_system::can_place_tree_tile_at(int x, int y, tile_type tile_type)
	{
		if (x < 0 || x >= CHUNKS_WIDTH * CHUNK_SIZE || y < 0 || y >= CHUNKS_WIDTH * CHUNK_SIZE)
			return false;

		if (game_engine::in_set(world_tiles->get_tile_at(x, y), BEDROCK, WOOD, ROOT, TREE_SEED))
			return false;

		uint16_t adjacent_tiles = 0;
		if (game_engine::in_set(world_tiles->get_tile_at(x, y - 1), tile_type, TREE_SEED))
			adjacent_tiles++;
		if (game_engine::in_set(world_tiles->get_tile_at(x, y + 1), tile_type, TREE_SEED))
			adjacent_tiles++;
		if (game_engine::in_set(world_tiles->get_tile_at(x - 1, y), tile_type, TREE_SEED))
			adjacent_tiles++;
		if (game_engine::in_set(world_tiles->get_tile_at(x + 1, y), tile_type, TREE_SEED))
			adjacent_tiles++;

		if (world_tiles->get_tile_at(x, y) <= GLASS && rand() % 10 != 0)
			return false;

		if (adjacent_tiles < 2)
			return true;
		else
			return rand() % 20 == 0;
	}

	void tree_system::trim_tree(tree &t, tile_coord start_tile)
	{
		// tree has been severed from the seed, trim loose bits
		tree_tracer tt = t.root_tiles[start_tile];
		if (tt.up == 1)
		{
			tile_coord new_current_tile = tile_coord(start_tile.x, start_tile.y - 1);
			trim_tree(t, new_current_tile);
		}
		else if (tt.down == 1)
		{
			tile_coord new_current_tile = tile_coord(start_tile.x, start_tile.y + 1);
			trim_tree(t, new_current_tile);
		}
		else if (tt.left == 1)
		{
			tile_coord new_current_tile = tile_coord(start_tile.x - 1, start_tile.y);
			trim_tree(t, new_current_tile);
		}
		else if (tt.right == 1)
		{
			tile_coord new_current_tile = tile_coord(start_tile.x + 1, start_tile.y);
			trim_tree(t, new_current_tile);
		}

		t.root_tiles.erase(start_tile);
	}

	bool tree_system::find_tile_to_grow_to(tree &t, tile_coord &current_tile, tile_coord &last_tile, tile_type tree_tile_type)
	{
		std::unordered_map<tile_coord, tree_tracer, tile_coord_hash> *tile_map;
		if(tree_tile_type == ROOT)
		{
			tile_map = &t.root_tiles;
		} else {
			tile_map = &t.branch_tiles;
		}

		tree_tracer tt;
		if(tile_map -> count(current_tile))
		{
			tt = tile_map->at(current_tile);
		} else {
			tt = {0, 0, 0, 0};
		}

		// check if tile is still valid
		if (!game_engine::in_set(world_tiles->get_tile_at(current_tile.x, current_tile.y), ROOT, TREE_SEED, WOOD))
		{
			if (tt.up == 2)
			{
				last_tile = tile_coord(current_tile.x, current_tile.y - 1);
				tile_map->at({current_tile.x, current_tile.y - 1}).down = 0;
			}
			else if (tt.down == 2)
			{
				last_tile = tile_coord(current_tile.x, current_tile.y + 1);
				tile_map->at({current_tile.x, current_tile.y + 1}).up = 0;
			}
			else if (tt.left == 2)
			{
				last_tile = tile_coord(current_tile.x - 1, current_tile.y);
				tile_map->at({current_tile.x - 1, current_tile.y}).right = 0;
			}
			else if (tt.right == 2)
			{
				last_tile = tile_coord(current_tile.x + 1, current_tile.y);
				tile_map->at({current_tile.x + 1, current_tile.y}).left = 0;
			}

			trim_tree(t, current_tile);
			return true;
		}

		uint16_t dir_count = 0;
		if (tt.up == 1)
			dir_count++;
		if (tt.down == 1)
			dir_count++;
		if (tt.left == 1)
			dir_count++;
		if (tt.right == 1)
			dir_count++;

		if (dir_count == 0) // end of root/branch
		{
			if (tt.down == 0 && can_place_tree_tile_at(current_tile.x, current_tile.y + 1, tree_tile_type) && rand() % 4 != 0)
			{
				last_tile = current_tile;
				current_tile = tile_coord(current_tile.x, current_tile.y + 1);
				return true;
			}
			else if (tt.left == 0 && can_place_tree_tile_at(current_tile.x - 1, current_tile.y, tree_tile_type) && rand() % 2 == 0)
			{
				last_tile = current_tile;
				current_tile = tile_coord(current_tile.x - 1, current_tile.y);
				return true;
			}
			else if (tt.right == 0 && can_place_tree_tile_at(current_tile.x + 1, current_tile.y, tree_tile_type) && rand() % 2 == 0)
			{
				last_tile = current_tile;
				current_tile = tile_coord(current_tile.x + 1, current_tile.y);
				return true;
			}
			else if (tt.up == 0 && can_place_tree_tile_at(current_tile.x, current_tile.y - 1, tree_tile_type) && rand() % 20 == 0)
			{
				last_tile = current_tile;
				current_tile = tile_coord(current_tile.x, current_tile.y - 1);
				return true;
			}
			return false;
		}

		if (dir_count < 3 && rand() % 25 == 0) // can try branching
		{
			if (tt.up == 0 && can_place_tree_tile_at(current_tile.x, current_tile.y - 1, ROOT) && rand() % 20 == 0)
			{
				last_tile = current_tile;
				current_tile = tile_coord(current_tile.x, current_tile.y - 1);
				return true;
			}
			else if (tt.down == 0 && can_place_tree_tile_at(current_tile.x, current_tile.y + 1, ROOT) && rand() % 2 == 0)
			{
				last_tile = current_tile;
				current_tile = tile_coord(current_tile.x, current_tile.y + 1);
				return true;
			}
			else if (tt.left == 0 && can_place_tree_tile_at(current_tile.x - 1, current_tile.y, ROOT) && rand() % 2 == 0)
			{
				last_tile = current_tile;
				current_tile = tile_coord(current_tile.x - 1, current_tile.y);
				return true;
			}
			else if (tt.right == 0 && can_place_tree_tile_at(current_tile.x + 1, current_tile.y, ROOT) && rand() % 2 == 0)
			{
				last_tile = current_tile;
				current_tile = tile_coord(current_tile.x + 1, current_tile.y);
				return true;
			}
			// return false;
		}

		int dirs[4] = {0, 1, 2, 3};
		std::shuffle(std::begin(dirs), std::end(dirs), g);

		for (int i = 0; i < 4; i++)
		{
			if (dirs[i] == 0 && tt.up == 1)
			{

				tile_coord new_current_tile = tile_coord(current_tile.x, current_tile.y - 1);
				if (find_tile_to_grow_to(t, new_current_tile, last_tile, tree_tile_type))
				{
					current_tile = new_current_tile;
					return true;
				}
			}
			else if (dirs[i] == 1 && tt.down == 1)
			{
				tile_coord new_current_tile = tile_coord(current_tile.x, current_tile.y + 1);
				if (find_tile_to_grow_to(t, new_current_tile, last_tile, tree_tile_type))
				{
					current_tile = new_current_tile;
					return true;
				}
			}
			else if (dirs[i] == 2 && tt.left == 1)
			{
				tile_coord new_current_tile = tile_coord(current_tile.x - 1, current_tile.y);
				if (find_tile_to_grow_to(t, new_current_tile, last_tile, tree_tile_type))
				{
					current_tile = new_current_tile;
					return true;
				}
			}
			else if (dirs[i] == 3 && tt.right == 1)
			{
				tile_coord new_current_tile = tile_coord(current_tile.x + 1, current_tile.y);
				if (find_tile_to_grow_to(t, new_current_tile, last_tile, tree_tile_type))
				{
					current_tile = new_current_tile;
					return true;
				}
			}
		}
		return false;
	}

	void tree_system::update()
	{
		increment_counter();
		tree_mutex.lock();
		// Update all trees
		for (auto &tree_entity : trees.get_entities())
		{
			tree &t = trees.get(tree_entity);
			// Update the tree

			// check if seed is still there
			// if (world_tiles->get_tile_at(t.seed_x, t.seed_y) != TREE_SEED)
			// {
			// 	remove_tree(tree_entity);
			// 	game_engine::game_engine_pointer->remove_entity(tree_entity);
			// 	continue;
			// }

			if (get_simple_tile_type(world_tiles->get_tile_at(t.seed_x, t.seed_y + 1)) == tile_simple_type::GAS)
			{
				// seed is floating, move down
				world_tiles->switch_tiles_with_lock(t.seed_x, t.seed_y, t.seed_x, t.seed_y + 1);
				t.seed_y += 1;
				continue;
			}

			if (t.root_tiles.size() < 48 && rand() % 3 == 0)
			{ // grow roots
				tile_coord current_tile = tile_coord(t.seed_x, t.seed_y);
				tile_coord last_tile = tile_coord(t.seed_x, t.seed_y);

				bool found_tile = find_tile_to_grow_to(t, current_tile, last_tile, ROOT);

				if (!found_tile)
				{
					continue;
				}

				if (last_tile.y > current_tile.y) // moved up
				{
					t.root_tiles[last_tile].up = 1;
					t.root_tiles[current_tile] = tree_tracer{0, 2, 0, 0};
					world_tiles->set_tile_at_with_lock(current_tile.x, current_tile.y, ROOT);
					world_tiles->set_tile_misc_data_at(current_tile.x, current_tile.y, 20);
				}
				else if (last_tile.y < current_tile.y) // moved down
				{
					t.root_tiles[last_tile].down = 1;
					t.root_tiles[current_tile] = tree_tracer{2, 0, 0, 0};
					world_tiles->set_tile_at_with_lock(current_tile.x, current_tile.y, ROOT);
					world_tiles->set_tile_misc_data_at(current_tile.x, current_tile.y, 20);
					
				}
				else if (last_tile.x > current_tile.x) // moved left
				{
					t.root_tiles[last_tile].left = 1;
					t.root_tiles[current_tile] = tree_tracer{0, 0, 0, 2};
					world_tiles->set_tile_at_with_lock(current_tile.x, current_tile.y, ROOT);
					world_tiles->set_tile_misc_data_at(current_tile.x, current_tile.y, 20);
				}
				else if (last_tile.x < current_tile.x) // moved right
				{
					t.root_tiles[last_tile].right = 1;
					t.root_tiles[current_tile] = tree_tracer{0, 0, 2, 0};
					world_tiles->set_tile_at_with_lock(current_tile.x, current_tile.y, ROOT);
					world_tiles->set_tile_misc_data_at(current_tile.x, current_tile.y, 20);
				}
			}

			if(t.branch_tiles.size() < t.root_tiles.size() && rand() % 3 == 0)
			{
				// grow branches
				tile_coord current_tile = tile_coord(t.seed_x, t.seed_y);
				tile_coord last_tile = tile_coord(t.seed_x, t.seed_y);

				bool found_tile = find_tile_to_grow_to(t, current_tile, last_tile, WOOD);

				if (!found_tile)
				{
					continue;
				}
				if (last_tile.y > current_tile.y) // moved up
				{
					t.branch_tiles[last_tile].up = 1;
					t.branch_tiles[current_tile] = tree_tracer{0, 2, 0, 0};
					world_tiles->set_tile_at_with_lock(current_tile.x, current_tile.y, WOOD);
					world_tiles->set_tile_misc_data_at(current_tile.x, current_tile.y, 20);
				}
				else if (last_tile.y < current_tile.y) // moved down
				{
					t.branch_tiles[last_tile].down = 1;
					t.branch_tiles[current_tile] = tree_tracer{2, 0, 0, 0};
					world_tiles->set_tile_at_with_lock(current_tile.x, current_tile.y, WOOD);
					world_tiles->set_tile_misc_data_at(current_tile.x, current_tile.y, 20);
				}
				else if (last_tile.x > current_tile.x) // moved left
				{
					t.branch_tiles[last_tile].left = 1;
					t.branch_tiles[current_tile] = tree_tracer{0, 0, 0, 2};
					world_tiles->set_tile_at_with_lock(current_tile.x, current_tile.y, WOOD);
					world_tiles->set_tile_misc_data_at(current_tile.x, current_tile.y, 20);
				}
				else if (last_tile.x < current_tile.x) // moved right
				{
					t.branch_tiles[last_tile].right = 1;
					t.branch_tiles[current_tile] = tree_tracer{0, 0, 2, 0};
					world_tiles->set_tile_at_with_lock(current_tile.x, current_tile.y, WOOD);
					world_tiles->set_tile_misc_data_at(current_tile.x, current_tile.y, 20);
				}
			}
		}

		tree_mutex.unlock();
	}

	void tree_system::start()
	{
		uint16_t loop_time_millis = 50;

		while (running)
		{
			auto start = std::chrono::high_resolution_clock::now();
			update();
			auto end = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
			add_time(duration.count());
			// printf("Tree system loop time: %d\n", duration.count());
			if (duration.count() < loop_time_millis)
				std::this_thread::sleep_for(std::chrono::milliseconds(loop_time_millis - duration.count()));
		}
		printf("Tree system stopped\n");
	}
}