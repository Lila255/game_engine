#include <algorithm>
#include "tree_system.hpp"
namespace game
{
	std::random_device rd;
	std::mt19937 g(rd());

	void tree_system::add_tree(entity ent, tree t)
	{
		trees.add(ent, t);
	}

	void tree_system::remove_tree(entity ent)
	{
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

		uint16_t adjacent_tiles = 0;
		if (game_engine::in_set(world_tiles->get_tile_at(x, y - 1), tile_type, TREE_SEED))
			adjacent_tiles++;
		if (game_engine::in_set(world_tiles->get_tile_at(x, y + 1), tile_type, TREE_SEED))
			adjacent_tiles++;
		if (game_engine::in_set(world_tiles->get_tile_at(x - 1, y), tile_type, TREE_SEED))
			adjacent_tiles++;
		if (game_engine::in_set(world_tiles->get_tile_at(x + 1, y), tile_type, TREE_SEED))
			adjacent_tiles++;

		return adjacent_tiles < 2;
	}

	bool tree_system::find_tile_to_grow_to(tree &t, tile_coord &current_tile, tile_coord &last_tile)
	{
		tree_tracer tt = t.root_tiles[current_tile];
		uint16_t dir_count = 0;
		if (tt.up == 1)
			dir_count++;
		if (tt.down == 1)
			dir_count++;
		if (tt.left == 1)
			dir_count++;
		if (tt.right == 1)
			dir_count++;

		if (dir_count == 0) // end of root
		{
			if(tt.down == 0 && can_place_tree_tile_at(current_tile.x, current_tile.y + 1, ROOT) && rand() % 4 != 0)
			{
				last_tile = current_tile;
				current_tile = tile_coord(current_tile.x, current_tile.y + 1);
				return true;
			}
			else if(tt.left == 0 && can_place_tree_tile_at(current_tile.x - 1, current_tile.y, ROOT) && rand() % 2 == 0)
			{
				last_tile = current_tile;
				current_tile = tile_coord(current_tile.x - 1, current_tile.y);
				return true;
			}
			else if(tt.right == 0 && can_place_tree_tile_at(current_tile.x + 1, current_tile.y, ROOT) && rand() % 2 == 0)
			{
				last_tile = current_tile;
				current_tile = tile_coord(current_tile.x + 1, current_tile.y);
				return true;
			}
			else if(tt.up == 0 && can_place_tree_tile_at(current_tile.x, current_tile.y - 1, ROOT) && rand() % 3 == 0)
			{
				last_tile = current_tile;
				current_tile = tile_coord(current_tile.x, current_tile.y - 1);
				return true;
			}
			return false;
		}

		if (dir_count < 3 && rand() % 100 == 0) // can try branching
		{
			if (tt.up == 0 && can_place_tree_tile_at(current_tile.x, current_tile.y - 1, ROOT) && rand() % 2 == 0)
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
			//return false;
		}

		int dirs[4] = {0, 1, 2, 3};
		std::shuffle(std::begin(dirs), std::end(dirs), g);

		for (int i = 0; i < 4; i++)
		{
			if (dirs[i] == 0 && tt.up == 1)
			{

				tile_coord new_current_tile = tile_coord(current_tile.x, current_tile.y - 1);
				if (find_tile_to_grow_to(t, new_current_tile, last_tile))
				{
					current_tile = new_current_tile;
					return true;
				}
			}
			else if (dirs[i] == 1 && tt.down == 1)
			{
				tile_coord new_current_tile = tile_coord(current_tile.x, current_tile.y + 1);
				if (find_tile_to_grow_to(t, new_current_tile, last_tile))
				{
					current_tile = new_current_tile;
					return true;
				}
			}
			else if (dirs[i] == 2 && tt.left == 1)
			{
				tile_coord new_current_tile = tile_coord(current_tile.x - 1, current_tile.y);
				if (find_tile_to_grow_to(t, new_current_tile, last_tile))
				{
					current_tile = new_current_tile;
					return true;
				}
			}
			else if (dirs[i] == 3 && tt.right == 1)
			{
				tile_coord new_current_tile = tile_coord(current_tile.x + 1, current_tile.y);
				if (find_tile_to_grow_to(t, new_current_tile, last_tile))
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
		// Update all trees
		for (auto &tree_entity : trees.get_entities())
		{
			tree &t = trees.get(tree_entity);
			// Update the tree

			if (t.root_tiles.size() < 256 && rand() % 3 == 0)
			{ // grow roots
				tile_coord current_tile = tile_coord(t.seed_x, t.seed_y);
				tile_coord last_tile = tile_coord(t.seed_x, t.seed_y);

				bool found_tile = find_tile_to_grow_to(t, current_tile, last_tile);

				if(!found_tile)
				{
					continue;
				}

				// while (1)
				// {
				// 	tree_tracer tt = t.root_tiles[current_tile];
				// 	uint16_t dir_count = 0;
				// 	if (tt.up == 1)
				// 		dir_count++;
				// 	if (tt.down == 1)
				// 		dir_count++;
				// 	if (tt.left == 1)
				// 		dir_count++;
				// 	if (tt.right == 1)
				// 		dir_count++;
				// 	if (dir_count == 0) // end of root
				// 	{
				// 		if (tt.down == 0 && can_place_tree_tile_at(current_tile.x, current_tile.y + 1, ROOT) && rand() % 2 == 0)
				// 		{
				// 			last_tile = current_tile;
				// 			current_tile = tile_coord(current_tile.x, current_tile.y + 1);
				// 		}
				// 		// else if (tt.up == 0 && can_place_tree_tile_at(current_tile.x, current_tile.y - 1, ROOT))
				// 		// {
				// 		// 	last_tile = current_tile;
				// 		// 	current_tile = tile_coord(current_tile.x, current_tile.y - 1);
				// 		// }
				// 		else if (tt.left == 0 && can_place_tree_tile_at(current_tile.x - 1, current_tile.y, ROOT) && rand() % 2 == 0)
				// 		{
				// 			last_tile = current_tile;
				// 			current_tile = tile_coord(current_tile.x - 1, current_tile.y);
				// 		}
				// 		else if (tt.right == 0 && can_place_tree_tile_at(current_tile.x + 1, current_tile.y, ROOT) && rand() % 2 == 0)
				// 		{
				// 			last_tile = current_tile;
				// 			current_tile = tile_coord(current_tile.x + 1, current_tile.y);
				// 		}
				// 		break;
				// 	}
				// 	else if (dir_count < 3 && rand() % 100 == 0) // can try branching
				// 	{
				// 		if (tt.up == 0 && can_place_tree_tile_at(current_tile.x, current_tile.y - 1, ROOT))
				// 		{
				// 			// t.root_tiles[{current_tile.x, current_tile.y - 1}] = tree_tracer{0, 2, 0, 0};
				// 			// t.root_tiles[current_tile].up = 1;
				// 			// world_tiles->set_tile_at(current_tile.x, current_tile.y - 1, ROOT);
				// 			last_tile = current_tile;
				// 			current_tile = tile_coord(current_tile.x, current_tile.y - 1);
				// 		}
				// 		else if (tt.down == 0 && can_place_tree_tile_at(current_tile.x, current_tile.y + 1, ROOT))
				// 		{
				// 			last_tile = current_tile;
				// 			current_tile = tile_coord(current_tile.x, current_tile.y + 1);
				// 		}
				// 		else if (tt.left == 0 && can_place_tree_tile_at(current_tile.x - 1, current_tile.y, ROOT))
				// 		{
				// 			last_tile = current_tile;
				// 			current_tile = tile_coord(current_tile.x - 1, current_tile.y);
				// 		}
				// 		else if (tt.right == 0 && can_place_tree_tile_at(current_tile.x + 1, current_tile.y, ROOT))
				// 		{
				// 			last_tile = current_tile;
				// 			current_tile = tile_coord(current_tile.x + 1, current_tile.y);
				// 		}
				// 		break;
				// 	}

				// 	if (tt.up == 1 && rand() % 2 == 0)
				// 	{
				// 		last_tile = current_tile;
				// 		current_tile = tile_coord(current_tile.x, current_tile.y - 1);
				// 	}
				// 	else if (tt.down == 1 && rand() % 2 == 0)
				// 	{
				// 		last_tile = current_tile;
				// 		current_tile = tile_coord(current_tile.x, current_tile.y + 1);
				// 	}
				// 	else if (tt.left == 1 && rand() % 2 == 0)
				// 	{
				// 		last_tile = current_tile;
				// 		current_tile = tile_coord(current_tile.x - 1, current_tile.y);
				// 	}
				// 	else if (tt.right == 1 && rand() % 2 == 0)
				// 	{
				// 		last_tile = current_tile;
				// 		current_tile = tile_coord(current_tile.x + 1, current_tile.y);
				// 	}
				// }



				if(last_tile.y > current_tile.y)	// moved up
				{
					t.root_tiles[last_tile].up = 1;
					t.root_tiles[current_tile] = tree_tracer{0, 2, 0, 0};
					world_tiles->set_tile_at_with_lock(current_tile.x, current_tile.y, ROOT);
				} else if(last_tile.y < current_tile.y)	// moved down
				{
					t.root_tiles[last_tile].down = 1;
					t.root_tiles[current_tile] = tree_tracer{2, 0, 0, 0};
					world_tiles->set_tile_at_with_lock(current_tile.x, current_tile.y, ROOT);
				} else if(last_tile.x > current_tile.x)	// moved left
				{
					t.root_tiles[last_tile].left = 1;
					t.root_tiles[current_tile] = tree_tracer{0, 0, 0, 2};
					world_tiles->set_tile_at_with_lock(current_tile.x, current_tile.y, ROOT);
				} else if(last_tile.x < current_tile.x)	// moved right
				{
					t.root_tiles[last_tile].right = 1;
					t.root_tiles[current_tile] = tree_tracer{0, 0, 2, 0};
					world_tiles->set_tile_at_with_lock(current_tile.x, current_tile.y, ROOT);
				}
			}
		}
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
			printf("Tree system loop time: %d\n", duration.count());
			if (duration.count() < loop_time_millis)
				std::this_thread::sleep_for(std::chrono::milliseconds(loop_time_millis - duration.count()));
		}
		printf("Tree system stopped\n");
	}
}