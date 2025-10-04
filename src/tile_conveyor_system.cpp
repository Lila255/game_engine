#include "tile_conveyor_system.hpp"

namespace game
{
	void tile_conveyor_system::start_thread() {
		printf("Starting tile conveyor system thread\n");
		running = 1;
		uint16_t tick_ms = 150;
		uint64_t tick_count = 0;

		while(running)
		{
			std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();
			this->update(tick_count);
			tick_count++;
			std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();
			std::chrono::microseconds elapsed_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
			// printf("Elapsed time: %ld\n", elapsed_ms.count());
			if(elapsed_ms.count() < tick_ms * 1000)
			{
				std::this_thread::sleep_for(std::chrono::microseconds((tick_ms * 1000 - elapsed_ms.count())));
			}
		}
	}

	
	// direction deltas for conveyor movement, key is the edginess value of the tile. value is the two possible directions the conveyor can move
	std::unordered_map<uint8_t, std::pair<std::pair<int, int>, std::pair<int, int>>> direction_deltas = {
		{1, {{-1, 0}, {1, 0}}}, // tile above, moves sideways
		{2, {{0, -1}, {0, 1}}}, // tile right, moves up/down
		{4, {{1, 0}, {-1, 0}}}, // tile below, moves sideways
		{8, {{0, 1}, {0, -1}}}, // tile left, moves up/down
		{1 + 2, {{-1, 0}, {0, 1}}}, // tile above + right, moves left or down
		{2 + 4, {{0, -1}, {1, 0}}}, // tile right + below, moves up or left
		{4 + 8, {{1, 0}, {0, -1}}}, // tile below + left, moves right or up
		{8 + 1, {{0, 1}, {-1, 0}}}, // tile left + above, moves down or right
	};

	int8_t directions_x[] = {0, 1, 0, -1};
	int8_t directions_y[] = {-1, 0, 1, 0};

	std::unordered_map<uint8_t, std::pair<std::pair<int, int>, std::pair<int, int>>> diag_direction_deltas = {
		{1, {{-1, 0}, {0, -1}}}, // tile above left, move left or up
		{2, {{0, -1}, {1, 0}}}, // tile above right, move up or right
		{4, {{1, 0}, {0, 1}}}, // tile below right, move right or down
		{8, {{0, 1}, {-1, 0}}}, // tile below left, move down or left
		{1 + 2, {{-1, 0}, {1, 0}}}, // tile above both sides, move left or right
		{2 + 4, {{0, -1}, {0, 1}}}, // tile right both sides, move up or down
		{4 + 8, {{1, 0}, {-1, 0}}}, // tile below both sides, move left or right
		{8 + 1, {{0, 1}, {0, -1}}} // tile left both sides, move up or down
	};
	int8_t diag_directions_x[] = {-1, 1, 1, -1};
	int8_t diag_directions_y[] = {-1, -1, 1, 1};

	// // above but with all 8 directions
	// int8_t directions_x[] = {-1, -1, 0, 1, 1, 1, 0, -1};
	// int8_t directions_y[] = {0, -1, -1, -1, 0, 1, 1, 1};
	// std::unordered_map<uint8_t, std::pair<std::pair<int, int>, std::pair<int, int>>> direction_deltas = {
	// };


	std::pair<int, int> tile_conveyor_system::get_next_conveyor_tooth_position(tile_conveyor &conveyor, int x, int y)
	{
		int direction = conveyor.moving > 0 ? 0 : 1; // if moving is positive, use first direction, else use second direction

		uint8_t edginess = 0;
		for(int i = 0; i < 4; i++)
		{
			int nx = x + directions_x[i];
			int ny = y + directions_y[i];
			uint8_t neighbor_tile = world_tile_sys->get_write_tile_at(nx, ny);
			if(neighbor_tile == CONVEYOR_BELT)
			{
				edginess |= (1 << i);
			}
		}

		if(direction_deltas.count(edginess))
		{
			auto &deltas = direction_deltas[edginess];
			int new_x;
			int new_y;
			if(direction == 0)
			{
				new_x = x + deltas.first.first;
				new_y = y + deltas.first.second;
			}
			else
			{
				new_x = x + deltas.second.first;
				new_y = y + deltas.second.second;
			}
			return {new_x, new_y};
		}
		else if(edginess != 0)
		{
			// invalid edginess, return original position
			return {x, y};
		}
		else
		{
			// no valid direction in first check. Either a corner/end piece or an invalid conveyor placement
			// check diagonals for a valid direction
			edginess = 0;
			for(int i = 0; i < 4; i++)
			{
				int nx = x + diag_directions_x[i];
				int ny = y + diag_directions_y[i];
				uint8_t neighbor_tile = world_tile_sys->get_write_tile_at(nx, ny);
				if(neighbor_tile == CONVEYOR_BELT)
				{
					edginess |= (1 << i);
				}
			}

			if(diag_direction_deltas.count(edginess))
			{
				auto &deltas = diag_direction_deltas[edginess];
				int new_x;
				int new_y;
				if(direction == 0)
				{
					new_x = x + deltas.first.first;
					new_y = y + deltas.first.second;
				}
				else
				{
					new_x = x + deltas.second.first;
					new_y = y + deltas.second.second;
				}
				return {new_x, new_y};
			}
			else
			{
				// no valid direction found, return original position
				return {x, y};
			}
		}
	}

	void tile_conveyor_system::update(uint64_t tick_count)
	{
		for(auto &ent : conveyors.get_entities())
		{
			tile_conveyor &conveyor = conveyors.get(ent);

			if(conveyor.moving == 0)
				continue;

			// if the conveyor has no path, look for one in the entity's tile_pathfinding component
			if(conveyor.path.size() == 0)
			{
				tile_pathfinding &tp = tile_pathfinding_sys -> get(ent);
				if(tp.path.size() == 0)
				{
					continue;
				}
				conveyor.path.clear();
				for(int i = 0; i < tp.path.size() - 1; i+=1)
				{
					int next_index = i + 1;

					std::pair<int, int> &tile_pos = tp.path[i];
					std::pair<int, int> &next_tile_pos = tp.path[next_index];

					int dx = next_tile_pos.first - tile_pos.first;
					int dy = next_tile_pos.second - tile_pos.second;

					for(int j = 0; j <= abs(dx); j++)
					{
						conveyor.path.push_back({tile_pos.first + j * (dx > 0 ? 1 : -1), tile_pos.second});
					}

					// conveyor.path.push_back(next_tile_pos);
					for(int j = 0; j <= abs(dy); j++)
					{
						conveyor.path.push_back({tile_pos.first, tile_pos.second + j * (dy > 0 ? 1 : -1)});
					}
					

					// for(int step = 0; step <= tp.step_distance; step++)
					// {
					// 	int interp_x = tile_pos.first + (next_tile_pos.first - tile_pos.first) * step / tp.step_distance;
					// 	int interp_y = tile_pos.second + (next_tile_pos.second - tile_pos.second) * step / tp.step_distance;
					// 	if(conveyor.path.size() == 0 || conveyor.path.back() != std::pair<int, int>{interp_x, interp_y})
					// 		conveyor.path.push_back({interp_x, interp_y});
					// }
				}
				// conveyor.path = tp.path;
				// place conveyor tiles along the path
				for(auto &p : conveyor.path)
				{
					world_tile_sys->set_tile_at_with_lock(p.first, p.second, CONVEYOR_BELT);
				}
				// create tooth_path around the outside of the path
				conveyor.tooth_path.clear();
				// std::unordered_set<std::pair<int, int>, pair_hash> added_tooth_positions;
				// int d_i = 1;
				// for(int i = 0; i > 0 || d_i == 1; i+=d_i)
				// {
				// 	if(i == conveyor.path.size() - 1)
				// 	{
				// 		d_i = -1;
				// 	}
				// 	std::pair<int, int> &tile_pos = conveyor.path[i];
				// 	std::pair<int, int> next_tile_pos = conveyor.path[(i + d_i)];
				// 	int dx = next_tile_pos.first - tile_pos.first;
				// 	int dy = next_tile_pos.second - tile_pos.second;
				// 	if (dx == 1 && dy == 0)
				// 	{
				// 		if(added_tooth_positions.find({tile_pos.first, tile_pos.second - 1}) != added_tooth_positions.end())
				// 			continue;
				// 		// moving right, add tooth above
				// 		conveyor.tooth_path.push_back({tile_pos.first, tile_pos.second - 1});
				// 		added_tooth_positions.insert({tile_pos.first, tile_pos.second - 1});
				// 	}
				// 	else if (dx == -1 && dy == 0)
				// 	{

				// 		if(added_tooth_positions.find({tile_pos.first, tile_pos.second + 1}) != added_tooth_positions.end())
				// 			continue;
				// 		// moving left, add tooth below
				// 		conveyor.tooth_path.push_back({tile_pos.first, tile_pos.second + 1});
				// 		added_tooth_positions.insert({tile_pos.first, tile_pos.second + 1});
				// 	}
				// 	else if (dx == 0 && dy == 1)
				// 	{
				// 		if(added_tooth_positions.find({tile_pos.first + 1, tile_pos.second}) != added_tooth_positions.end())
				// 			continue;
				// 		// moving down, add tooth right
				// 		conveyor.tooth_path.push_back({tile_pos.first + 1, tile_pos.second});
				// 		added_tooth_positions.insert({tile_pos.first + 1, tile_pos.second});
				// 	}
				// 	else if (dx == 0 && dy == -1)
				// 	{
				// 		if(added_tooth_positions.find({tile_pos.first - 1, tile_pos.second}) != added_tooth_positions.end())
				// 			continue;
				// 		// moving up, add tooth left
				// 		conveyor.tooth_path.push_back({tile_pos.first - 1, tile_pos.second});
				// 		added_tooth_positions.insert({tile_pos.first - 1, tile_pos.second});
				// 	}
				// }

				std::pair<int, int> current_tooth_pos = {conveyor.path[0].first, conveyor.path[0].second - 1};
				if(world_tile_sys->get_tile_at(current_tooth_pos.first, current_tooth_pos.second) == CONVEYOR_BELT)
				{
					current_tooth_pos = {conveyor.path[0].first + 1, conveyor.path[0].second};
				}
				conveyor.tooth_path.push_back(current_tooth_pos);
				std::pair<int, int> start_tooth_pos = current_tooth_pos;
				current_tooth_pos = get_next_conveyor_tooth_position(conveyor, current_tooth_pos.first, current_tooth_pos.second);
				while(current_tooth_pos != start_tooth_pos)
				{
					if(world_tile_sys->get_tile_at(current_tooth_pos.first, current_tooth_pos.second) == CONVEYOR_BELT)
					{
						printf("Conveyor tooth path generation hit conveyor belt at %d, %d\n", current_tooth_pos.first, current_tooth_pos.second);
						break;
					}
					conveyor.tooth_path.push_back(current_tooth_pos);
					std::pair<int, int> next_tile_pos = get_next_conveyor_tooth_position(conveyor, current_tooth_pos.first, current_tooth_pos.second);
					if(next_tile_pos == current_tooth_pos)
						break;
					current_tooth_pos = next_tile_pos;
				}

				// place conveyor tooth tiles along the tooth_path
				for(int i = 0; i < conveyor.tooth_path.size(); i+=conveyor.teeth_spacing)
				{
					auto &p = conveyor.tooth_path[i];
					world_tile_sys->set_tile_at_with_lock(p.first, p.second, CONVEYOR_TOOTH);
				}
			}
			
			auto tile_mutex = world_tile_sys->get_chunk_mutex_base();
			std::unique_lock<std::shared_mutex> lock(*tile_mutex);

			// move the tiles in the tooth path along the tooth path
			if(conveyor.tooth_path.size() > 0)
			{

				int direction = conveyor.moving > 0 ? -1 : 1;
				int index = conveyor.moving > 0 ? (conveyor.tooth_path.size() - 1) : 0;

				// world_tile_sys->
				while(index >= 0 && index < conveyor.tooth_path.size())
				{
					std::pair<int, int> &tile_pos = conveyor.tooth_path[index];
					tile_type tile = (tile_type)world_tile_sys->get_tile_at(tile_pos.first, tile_pos.second);
					tile_simple_type st = world_tile_sys->get_simple_tile_type(tile);

					std::pair<int, int> next_tile_pos = conveyor.tooth_path[(index + direction + conveyor.tooth_path.size()) % conveyor.tooth_path.size()];
					// tile_type next_tile = (tile_type)world_tile_sys->get_tile_at(next_tile_pos.first, next_tile_pos.second);

					if(tile == CONVEYOR_TOOTH)
					{
						// printf("moving tooth\n");
						world_tile_sys->switch_tiles_no_lock(tile_pos.first, tile_pos.second, next_tile_pos.first, next_tile_pos.second);
						
						std::pair<int, int> above_tile = {tile_pos.first, tile_pos.second - 1};
						while(true)
						{
							// printf("checking above tile at %d, %d\n", above_tile.first, above_tile.second);
							if(above_tile.second < tile_pos.second - 8)
								break;
							
							// int above_x = above_tile.first;
							// int above_y = above_tile.second;
							tile_type above_tile_type = (tile_type)world_tile_sys->get_tile_at(above_tile.first, above_tile.second);
							tile_simple_type st3 = world_tile_sys->get_simple_tile_type(above_tile_type);
							if(st3 == SOLID && is_tile_fixed[above_tile_type] == 0)
							{
								int above_right_x = above_tile.first + 1;
								int above_right_y = above_tile.second;
								tile_type above_right_tile = (tile_type)world_tile_sys->get_tile_at(above_right_x, above_right_y);
								tile_simple_type st4 = world_tile_sys->get_simple_tile_type(above_right_tile);
								if(st4 < SOLID)
								{
									world_tile_sys->switch_tiles_no_lock(above_tile.first, above_tile.second, above_right_x, above_right_y);
								}
								above_tile = {above_tile.first, above_tile.second - 1};
								// else
								// 	break;
							}
							else
								break;
						}
					}
					else
					{
						// if the there are tiles inbetween this tile and the next tooth tile, move this tile along the path
						bool reached_next_tooth = false;
						for(int i = 1; i < conveyor.teeth_spacing; i++)
						{
							int between_index = (index + direction * i + conveyor.tooth_path.size()) % conveyor.tooth_path.size();
							std::pair<int, int> &between_tile_pos = conveyor.tooth_path[between_index];
							tile_type between_tile = (tile_type)world_tile_sys->get_tile_at(between_tile_pos.first, between_tile_pos.second);
							tile_simple_type st2 = world_tile_sys->get_simple_tile_type(between_tile);
							if(between_tile == CONVEYOR_TOOTH)
							{
								reached_next_tooth = true;
								break;
							}
							else if(st > st2)
							{
								reached_next_tooth = false;
								break;
							}
						}
						if(reached_next_tooth)
						{
							// printf("moving non-tooth\n");
							world_tile_sys->switch_tiles_no_lock(tile_pos.first, tile_pos.second, next_tile_pos.first, next_tile_pos.second);
							if(st == SOLID && is_tile_fixed[tile] == 0)
							{
								std::pair<int, int> above_tile = {tile_pos.first, tile_pos.second - 1};
								while(true)
								{
									// printf("checking above tile at %d, %d\n", above_tile.first, above_tile.second);
									if(above_tile.second < tile_pos.second - 8)
										break;
									
									// int above_x = above_tile.first;
									// int above_y = above_tile.second;
									int above_right_x = above_tile.first + 1;
									int above_right_y = above_tile.second;
									tile_type above_tile_type = (tile_type)world_tile_sys->get_tile_at(above_tile.first, above_tile.second);
									tile_simple_type st3 = world_tile_sys->get_simple_tile_type(above_tile_type);
									if(st3 == SOLID && is_tile_fixed[above_tile_type] == 0)
									{
										tile_type above_right_tile = (tile_type)world_tile_sys->get_tile_at(above_right_x, above_right_y);
										tile_simple_type st4 = world_tile_sys->get_simple_tile_type(above_right_tile);
										if(st4 < SOLID)
										{
											world_tile_sys->switch_tiles_no_lock(above_tile.first, above_tile.second, above_right_x, above_right_y);
											above_tile = {above_tile.first, above_tile.second - 1};
										}
										else
											break;
									}
									else
										break;
								}
							}
						}
					}

					index += direction;

				}


				// int end_index = conveyor.moving > 0 ? (conveyor.tooth_path.size() - 1) : 0;
				// tile_type last_tile = (tile_type)world_tile_sys->get_tile_at(conveyor.tooth_path[end_index].first, conveyor.tooth_path[end_index].second);
				// float last_temp = world_tile_sys->get_tile_temperature_at(conveyor.tooth_path[end_index].first, conveyor.tooth_path[end_index].second);

				// for(int i = conveyor.moving > 0 ? (conveyor.tooth_path.size() - 1) : 0; i >= 0 && i < conveyor.tooth_path.size(); i += conveyor.moving < 0 ? - 1 : 1)
				// {


				// int last_tooth_index = -1;
				// for(int ii = 0; ii < conveyor.tooth_path.size(); ii++)
				// {
				// 	int i = conveyor.moving > 0 ? (conveyor.tooth_path.size() - 1 - ii) : ii;
				// 	std::pair<int, int> &tile_pos = conveyor.tooth_path[i];
				// 	tile_type tile = (tile_type)world_tile_sys->get_tile_at(tile_pos.first, tile_pos.second);

				// 	if(tile != CONVEYOR_TOOTH)
				// 		continue;

				// 	if (last_tooth_index + (conveyor.moving > 0 ? conveyor.teeth_spacing : -conveyor.teeth_spacing) != i && last_tooth_index != -1)
				// 	{
				// 		last_tooth_index = i;
				// 		continue; // skip teeth that are too close to the last moved tooth
				// 	}

				// 	// backtrack to find how many tiles to push along the path
				// 	// int backtrack_index = 0;
				// 	// for(int j = 0; j < conveyor.teeth_spacing; j++)
				// 	// {
				// 	// 	int bt_index = i + backtrack_index + (conveyor.moving < 0 ? -1 : 1);
				// 	// 	if(bt_index < 0)
				// 	// 	bt_index = conveyor.tooth_path.size() - 1;
				// 	// 	if(bt_index >= conveyor.tooth_path.size())
				// 	// 	bt_index = 0;
				// 	// 	std::pair<int, int> &bt_tile_pos = conveyor.tooth_path[bt_index];
				// 	// 	tile_type bt_tile = (tile_type)world_tile_sys->get_tile_at(bt_tile_pos.first, bt_tile_pos.second);
				// 	// 	tile_simple_type st = world_tile_sys->get_simple_tile_type(bt_tile);
				// 	// 	if(st != SOLID && st != LIQUID)// && is_solid_tile[bt_tile] == 1)
				// 	// 		break;
				// 	// 	backtrack_index -= conveyor.moving < 0 ? -1 : 1;
				// 	// }
				// 	// if(backtrack_index == 0)
				// 	// {
				// 	// 	world_tile_sys->switch_tiles_with_lock(tile_pos.first, tile_pos.second, conveyor.tooth_path[(i + conveyor.moving < 0 ? -1 : 1 + i + conveyor.tooth_path.size()) % conveyor.tooth_path.size()].first, conveyor.tooth_path[(i + conveyor.moving < 0 ? -1 : 1 + i + conveyor.tooth_path.size()) % conveyor.tooth_path.size()].second);
				// 	// 	continue;
				// 	// }
				// 	// else
				// 	// {
				// 	// 	// move all the tiles backtrack_index along the path
				// 	// 	for(int k = backtrack_index; k != 0; k -= conveyor.moving < 0 ? 1 : -1)
				// 	// 	{
				// 	// 		int from_index = (i + k + conveyor.tooth_path.size()) % conveyor.tooth_path.size();
				// 	// 		int to_index = (i + k + (conveyor.moving < 0 ? -1 : 1) + conveyor.tooth_path.size()) % conveyor.tooth_path.size();
				// 	// 		std::pair<int, int> &from_tile_pos = conveyor.tooth_path[from_index];
				// 	// 		std::pair<int, int> &to_tile_pos = conveyor.tooth_path[to_index];
				// 	// 		world_tile_sys->switch_tiles_with_lock(from_tile_pos.first, from_tile_pos.second, to_tile_pos.first, to_tile_pos.second);
				// 	// 	}
				// 	// }

				// 	// found a tooth
				// 	int last_index = i;
				// 	for(int j = 0; j < conveyor.teeth_spacing; j++)
				// 	{
				// 		int next_index = last_index + (conveyor.moving > 0 ? 1 : -1);
				// 		if(next_index < 0)
				// 			next_index = conveyor.tooth_path.size() - 1;
				// 		if(next_index >= conveyor.tooth_path.size())
				// 			next_index = 0;
				// 		std::pair<int, int> &next_tile_pos = conveyor.tooth_path[next_index];
				// 		tile_type next_tile = (tile_type)world_tile_sys->get_tile_at(next_tile_pos.first, next_tile_pos.second);
				// 		tile_simple_type st = world_tile_sys->get_simple_tile_type(next_tile);
				// 		last_index = next_index;
				// 		if(st < LIQUID)
				// 			break;
				// 	}
				// 	for(int j = last_index; j != i; j -= conveyor.moving < 0 ? 1 : -1)
				// 	{
				// 		int from_index = (j + conveyor.tooth_path.size()) % conveyor.tooth_path.size();
				// 		int to_index = (j + (conveyor.moving < 0 ? -1 : 1) + conveyor.tooth_path.size()) % conveyor.tooth_path.size();
				// 		std::pair<int, int> &from_tile_pos = conveyor.tooth_path[from_index];
				// 		std::pair<int, int> &to_tile_pos = conveyor.tooth_path[to_index];
				// 		world_tile_sys->switch_tiles_with_lock(from_tile_pos.first, from_tile_pos.second, to_tile_pos.first, to_tile_pos.second);
				// 	}

				// 	last_tooth_index = i + (conveyor.moving > 0 ? 1 : -1);
				// }

			}

			// move the conveyor teeth and any tiles in them along the path, around the outside
			// if moving is > 0, move clockwise around the outside of the path
			// if moving is < 0, move counterclockwise around the outside of the path

			// for(int i = conveyor.moving > 0 ? 0 : (conveyor.path.size() - 1); i >= 0 && i < conveyor.path.size(); i += conveyor.moving == 0 ? -1 : 1)
			// {
				
			// }

			// // do one direction first
			// for(int i = 0; i < conveyor.path.size(); i++)
			// {
			// 	int current_index = i;
			// 	int next_index = (i + 1);
			// 	if(next_index >= conveyor.path.size())
			// 		next_index = i - 1;
			// 	// int prev_index = (i - 1);
			// 	// if(prev_index < 0)
			// 	// 	prev_index = conveyor.path.size() - 1;

			// 	std::pair<int, int> &tile_pos = conveyor.path[i];
			// 	std::pair<int, int> next_tile_pos = conveyor.path[(i + 1) % conveyor.path.size()];
			// 	// std::pair<int, int> prev_tile_pos = conveyor.path[(i - 1 + conveyor.path.size()) % conveyor.path.size()];

			// 	// get the tile at tile_pos
			// 	tile_type tile = (tile_type)world_tile_sys->get_tile_at(tile_pos.first, tile_pos.second);

			// 	if (tile != CONVEYOR_BELT)
			// 	{
			// 		// Conveyor is broken, stop moving
			// 		conveyor.moving = 0;
			// 		break;
			// 	}

			// 	int dx = next_tile_pos.first - tile_pos.first;
			// 	int dy = next_tile_pos.second - tile_pos.second;

			// 	if ()

		}
	}

	void tile_conveyor_system::add_component(entity ent, tile_conveyor &tc) {
		conveyors.add(ent, tc);

		// create pathfinding component for the conveyor
		tile_pathfinding pathfinding_comp = tile_pathfinding();

		pathfinding_comp.start_x = tc.start_x;
		pathfinding_comp.start_y = tc.start_y;
		pathfinding_comp.end_x = tc.end_x;
		pathfinding_comp.end_y = tc.end_y;
		pathfinding_comp.directions = 4;
		pathfinding_comp.clearance = 5;
		pathfinding_comp.step_distance = tc.teeth_spacing * 2;
		pathfinding_comp.can_fly = 1;
		pathfinding_comp.keep_live = 0;
		tile_pathfinding_sys->add_component(ent, pathfinding_comp);
	}
}