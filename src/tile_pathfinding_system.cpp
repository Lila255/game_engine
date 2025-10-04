#include "tile_pathfinding_system.hpp"
#include "box2d_system.hpp"

namespace game
{
	struct node
	{
		int x, y;
		int g, h;
		int f() const { return g + h; }
		node *parent = nullptr;
	};
	struct node_comparator {
		bool operator()(const node* a, const node* b) const {
			return a->f() > b->f();
		}
	};
	int pathfinding_dx[] = {0, 1, 1, 1, 0, -1, -1, -1};
	int pathfinding_dy[] = {-1, -1, 0, 1, 1, 1, 0, -1};

	void tile_pathfinding_system::update()
	{
		if (!running)
			return;

		std::vector<entity> entities = pathfinding_components.get_entities();
		printf("Pathfinding for %d entities", (int)entities.size());
		uint32_t active_count = 0;
		for (entity ent : entities)
		{
			tile_pathfinding &tp = pathfinding_components.get(ent);
			if (tp.keep_live == 0 && tp.path.size() > 0)
				continue; // only pathfind once if keep_live is 0
			active_count++;
			tp.path.clear();
			int start_x = tp.start_x;
			int start_y = tp.start_y;
			// if the start or end are entities, get their positions
			if(tp.start_ent != 0)
			{
				auto b2d_sys = (game::box2d_system *)(game_engine::game_engine_pointer -> get_system(game_engine::family::type<game::box2d_system>()));
				auto &start_pos = b2d_sys -> get_dynamic_body(tp.start_ent) -> GetPosition();
				start_x = (int)start_pos.x * box2d_scale;
				start_y = (int)start_pos.y * box2d_scale;
			}
			int end_x = tp.end_x;
			int end_y = tp.end_y;
			// if the start or end are entities, get their positions
			if(tp.end_ent != 0)
			{
				auto b2d_sys = (game::box2d_system *)(game_engine::game_engine_pointer -> get_system(game_engine::family::type<game::box2d_system>()));
				auto &end_pos = b2d_sys -> get_dynamic_body(tp.end_ent) -> GetPosition();
				end_x = (int)end_pos.x * box2d_scale;
				end_y = (int)end_pos.y * box2d_scale;
			}

			// if the start and end are very close, skip pathfinding
			if (abs(start_x - end_x) + abs(start_y - end_y) < tp.step_distance * 1.5)
				continue;

			// A* pathfinding algorithm
			std::priority_queue<node *, std::vector<node *>, node_comparator> open_set;
			std::unordered_set<tile_coord, tile_coord_hash> closed_set;
			// keep track of all created nodes
			std::vector<node *> all_nodes;

			node *start_node = new node{start_x, start_y, 0, abs(start_x - end_x) + abs(start_y - end_y), nullptr};
			open_set.push(start_node);
			all_nodes.push_back(start_node);
			bool path_found = false;
			node *end_node = nullptr;

			while (!open_set.empty())
			{
				node *current_node = open_set.top();
				open_set.pop();
				// if(open_set.size() % 100 == 0){
				// 	printf("Remaining open set size: %d\n", (int)open_set.size());
				// }
				if (abs(current_node->x - end_x) + abs(current_node->y - end_y) < tp.step_distance * 1.5)
				{
					path_found = true;
					end_node = current_node;
					break;
				}

				closed_set.insert({(uint32_t)current_node->x, (uint32_t)current_node->y});

				for (int i = 0; i < 8; i+=8/tp.directions)
				{
					int new_x = current_node->x + tp.step_distance * pathfinding_dx[i];
					int new_y = current_node->y + tp.step_distance * pathfinding_dy[i];

					if (new_x < 0 || new_x >= CHUNKS_WIDTH * CHUNK_SIZE || new_y < 0 || new_y >= CHUNKS_WIDTH * CHUNK_SIZE)
						continue;
					if (closed_set.count({(uint32_t)new_x, (uint32_t)new_y}))
						continue;
					bool walkable = true;

					// check clearance
					// for (int j = 0; j )
					tile_type t = (tile_type)world_tiles->get_tile_at(new_x, new_y);
					if (t >= SOLID_TILE_START_INDEX && is_solid_tile[t])
					{
						walkable = false;
					}
					else 
					{
						for(int j = 0; j < tp.step_distance; j++)
						{
							tile_type t2 = (tile_type)world_tiles->get_tile_at(current_node->x + j * pathfinding_dx[i], current_node->y + j * pathfinding_dy[i]);
							if (t2 >= SOLID_TILE_START_INDEX && is_solid_tile[t2])
							{
								walkable = false;
								break;
							}
						}
					}
					// if (walkable && !tp.can_fly)
					// {
					// 	bool solid_ground = false;
					// 	// check clearance area
					// 	for (int cx = -tp.clearance; cx <= tp.clearance; cx++)
					// 	{
					// 		for (int cy = -tp.clearance; cy <= tp.clearance; cy++)
					// 		{
					// 			if (cx * cx + cy * cy > tp.clearance * tp.clearance)
					// 				continue;
					// 			int check_x = new_x + cx;
					// 			int check_y = new_y + cy;
					// 			if (check_x < 0 || check_x >= CHUNKS_WIDTH * CHUNK_SIZE || check_y < 0 || check_y >= CHUNKS_WIDTH * CHUNK_SIZE)
					// 				continue;
					// 			tile_type t2 = (tile_type)world_tiles->get_tile_at(check_x, check_y);
					// 			if (t2 >= SOLID_TILE_START_INDEX && is_solid_tile[t2])
					// 			{
					// 				solid_ground = true;
					// 				cx = tp.clearance + 1; // break outer loop
					// 				break;
					// 			}
					// 		}
					// 	}
					// 	if (!solid_ground)
					// 	{
					// 		walkable = false;
					// 	}
					// }
					

					if (walkable)
					{
						int g_cost = current_node->g + ((i % 2 == 0) ? tp.step_distance : (int)(tp.step_distance * 1.4));
						int h_cost = abs(new_x - end_x) + abs(new_y - end_y);
						node *neighbor_node = new node{new_x, new_y, g_cost, h_cost, current_node};
						open_set.push(neighbor_node);
						closed_set.insert({(uint32_t)new_x, (uint32_t)new_y});
						all_nodes.push_back(neighbor_node);
						// world_tiles->set_tile_at_with_lock(new_x, new_y, TEMPORARY_SMOKE); // mark the pathfinding search area with TEMPORARY_SMOKE
					}
				}
				// delete current_node;
				
			}

			if (path_found)
			{
				printf("Path found for entity \n");
				node *current = end_node;
				while (current != nullptr)
				{
					tp.path.push_back({current->x, current->y});
					current = current->parent;
				}
				std::reverse(tp.path.begin(), tp.path.end());
				// draw the path into the world tiles as TEMPORARY_SMOKE
				// for (auto &p : tp.path)
				// {
				// 	// world_tiles->set_tile_temperature_at(p.first, p.second, 10000);
				// 	world_tiles->set_tile_at_with_lock(p.first, p.second, TEMPORARY_SMOKE);
				// }
			}

			// clean up all created nodes
			for (auto n : all_nodes)
			{
				delete n;
			}
		}
		printf(", %d active pathfinding operations\n", (int)active_count);
	}

	void tile_pathfinding_system::start_thread()
	{
		printf("Starting pathfinding thread\n");
		running = true;
		uint16_t tick_ms = 1000;
		uint64_t tick_count = 0;
		
		while (running)
		{
			auto start = std::chrono::steady_clock::now();
			this->update();
			tick_count++;
			auto end = std::chrono::steady_clock::now();
			auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
			printf("Pathfinding tick %d took %d ms out of %d ms\n", tick_count, (int)elapsed, tick_ms);
			if (elapsed < tick_ms)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(tick_ms - elapsed));
			}
		}
	}
	void tile_pathfinding_system::set_running(bool run)
	{
		running = run;
	}
}
