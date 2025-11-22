#include "chunk.hpp"
#include "engine_core.hpp"
#include "tasks.hpp"
// #include "engine_comp.hpp"
// #include "game_core.hpp"

namespace game
{
	std::array<uint8_t, 256> is_solid_tile;						   // indicate which tiles are solid
	std::array<uint8_t, 256> is_tile_fixed;						   // indicate which tiles are fixed in place
	std::array<float, 256> tile_max_temperature;				   // maximum temperature a tile can have
	std::unordered_map<tile_type, tile_type> max_temp_tile_change; // tile to change to when overheated
	std::array<float, 256> tile_min_temperature;				   // minimum temperature a tile can have
	std::unordered_map<tile_type, tile_type> min_temp_tile_change; // tile to change to when cooled
	float absolute_max_temperature = 32765;						   // maximum temperature any tile can have
	std::array<float, 256> tile_heat_capacity;					   // heat capacity of each tile(combines density and specific heat, as there is no density value stored)
	std::array<float, 256> tile_thermal_conductivity;			   // thermal conductivity of each tile
	std::array<float, 256> tile_debris_fall_speed_multiplier;	   // multiplier for how fast debris of this tile type falls
	std::array<int8_t, 256> tile_phase_change_count_multiplier;	   // multiplier for how many items are created when a tile changes phase into the target tile

	void chunk::create_chunk(int32_t x, int32_t y)
	{
		chunk_x = x;
		chunk_y = y;

		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int x = 0; x < CHUNK_SIZE; x++)
			{
				double n_x = (x + chunk_x * game::CHUNK_SIZE) / 61.0;
				double n_y = (y + chunk_y * game::CHUNK_SIZE) / 61.0;
				double n_z = ((x + 10000) + chunk_x * game::CHUNK_SIZE) / 84.0;
				double noise_1 = perlin_noise_1.noise3D_01(n_x, n_y, n_z);
				double noise_2 = perlin_noise_2.noise3D_01(n_x, n_y, n_z);
				double noise_3 = perlin_noise_3.noise3D_01(n_x, n_y, n_z);
				// if (noise_1 * noise_2 > .30)
				// // if (noise_1 + noise_2 > .970)
				// { // solid
				// 	if (noise_2 > 0.725)
				// 	{
				// 		data[y][x] = game::STONE;
				// 	}
				// 	else
				// 	{
				// 		if (noise_3 > 0.758)
				// 		{
				// 			data[y][x] = game::SAND;
				// 		}
				// 		else
				// 		{
				// 			data[y][x] = game::DIRT;
				// 		}
				// 	}
				// }
				// else
				// {
				// 	data[y][x] = game::AIR;
				// }
				// -===============- //
				if (1 == 2 && (((y / 5) % 195 == 0) || ((x / 5) % 255 == 0)))
				{
					data[y][x] = game::AIR;
				}
				else
				{
					if (noise_1 * noise_2 > .250)
					// if (noise_1 + noise_2 > .970)
					{ // solid
						if (noise_2 > 0.75)
						{
							data[y][x] = game::STONE;
						}
						else
						{
							if (noise_3 > 0.658)
							{
								data[y][x] = game::SAND;
							}
							else
							{
								data[y][x] = game::DIRT;
							}
						}
					}
					else
					{
						data[y][x] = game::AIR;
					}
				}
				// else
				// {
				// 	data[y][x] = game::AIR;
				// }
				// -===============- //
				// data[y][x] = game::AIR;
				// -===============- //
				// if(((x / 15) % 2 == 0) && ((y / 15) % 2 == 0) && (x / 30) % 2 == 0){
				// if(((x / 15) % 2 == 0) && ((y / 15) % 4 == 0) && (x / 30) % 2 == 0){
				// -===============- //

				// 	data[y][x] = game::STONE;
				// }
				// else if(((x / 15) % 2 == 0) && ((y / 15) % 4 == 2) && (x / 30) % 2 == 0)
				// {
				// 	data[y][x] = game::SAND;
				// }
				// else {
				// 	data[y][x] = game::AIR;
				// }
				// --------------------------
				// if ( x == 90 || x == 91 || y == 90 || y == 91)
				// {
				// 	data[z][y][x] = game::STONE;
				// }
				// else
				// {
				// 	data[z][y][x] = game::AIR;
				// }
				// --------------------------
				// if (x + y < CHUNK_SIZE / 4 || x + y > 2.25 * (CHUNK_SIZE - CHUNK_SIZE / 4)) // || x + y > CHUNK_SIZE * 3 / 4)
				// // else if (noise_1 * noise_2 > .320)
				// {
				// 	data[z][y][x] = game::DIRT;
				// }
				// else
				// {
				// 	data[z][y][x] = game::AIR;

				// }
				// --------------------------
				// if (chunk_x % 2 && chunk_y % 2 && x > 3 * CHUNK_SIZE / 2 && y > 3 * CHUNK_SIZE / 2)
				// {
				// 	data[z][y][x] = game::DIRT;
				// }
				// else
				// {
				// 	data[z][y][x] = game::AIR;

				// }

				temperature_data[y][x] = 30;
			}
		}

		// spawn some grass
		for (int y = 1; y < CHUNK_SIZE; y++)
		{
			for (int x = 0; x < CHUNK_SIZE; x++)
			{
				if (data[y][x] == game::DIRT && data[y - 1][x] == game::AIR)
				{
					if (rand() % 100 < 10)
						data[y][x] = game::GRASS;
				}
			}
		}

		// for (int i = 0; i < 2; i++)
		// {
		// 	uint16_t rx = rand() % CHUNK_SIZE;
		// 	uint16_t ry = rand() % CHUNK_SIZE;
		// 	if (data[ry][rx] == game::AIR)
		// 	{
		// 		// create a task to set the tile
		// 		game_engine::task_scheduler_pointer->add_task({&update_tile_task, new update_tile_params(chunk_x * CHUNK_SIZE + rx, chunk_y * CHUNK_SIZE + ry, game::TREE_SEED, 20.f, 0)});
		// 	}
		// 	else
		// 	{
		// 		i--;
		// 	}
		// }

		// create solid border around world
		// if (chunk_x == 0 || chunk_x == CHUNKS_WIDTH - 1 || chunk_y == 0 || chunk_y == CHUNKS_WIDTH - 1)
		// {
		// 	if (chunk_x == 0)
		// 	{
		// 		for (int i = 0; i < CHUNK_SIZE; i++)
		// 		{
		// 			data[i][0] = game::BEDROCK;
		// 			temperature_data[i][0] = -100;
		// 			data[i][1] = game::BEDROCK;
		// 			temperature_data[i][1] = -100;
		// 		}
		// 	}
		// 	else if (chunk_x == CHUNKS_WIDTH - 1)
		// 	{
		// 		for (int i = 0; i < CHUNK_SIZE; i++)
		// 		{
		// 			data[i][CHUNK_SIZE - 1] = game::BEDROCK;
		// 			temperature_data[i][CHUNK_SIZE - 1] = -100;
		// 			data[i][CHUNK_SIZE - 2] = game::BEDROCK;
		// 			temperature_data[i][CHUNK_SIZE - 2] = -100;
		// 		}
		// 	}

		// 	if (chunk_y == 0)
		// 	{
		// 		for (int i = 0; i < CHUNK_SIZE; i++)
		// 		{
		// 			data[0][i] = game::BEDROCK;
		// 			temperature_data[i][0] = -100;
		// 			data[1][i] = game::BEDROCK;
		// 			temperature_data[i][1] = -100;
		// 		}
		// 	}
		// 	else if (chunk_y == CHUNKS_WIDTH - 1)
		// 	{
		// 		for (int i = 0; i < CHUNK_SIZE; i++)
		// 		{
		// 			data[CHUNK_SIZE - 1][i] = game::BEDROCK;
		// 			temperature_data[i][CHUNK_SIZE - 1] = -100;
		// 			data[CHUNK_SIZE - 2][i] = game::BEDROCK;
		// 			temperature_data[i][CHUNK_SIZE - 2] = -100;
		// 		}
		// 	}
		// }
	}

	void chunk::to_string()
	{
		for (int x = 0; x < CHUNK_SIZE; x++)
		{
			for (int y = 0; y < CHUNK_SIZE; y++)
			{
				printf("%d ", data[y][x]);
			}
			printf("\n");
		}
		printf("\n");
	}

	std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE> *chunk::get_data()
	{
		return &data;
	}
	std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE> *chunk::get_data_copy()
	{
		return &data_copy;
	}
	std::array<std::array<float, CHUNK_SIZE>, CHUNK_SIZE> *chunk::get_temperature_data()
	{
		return &temperature_data;
	}
	std::array<std::array<uint16_t, CHUNK_SIZE>, CHUNK_SIZE> *chunk::get_misc_data()
	{
		return &misc_data;
	}

	void chunk::copy_to_data_copy()
	{
		// data_copy = data;
		// memcpy
		std::memcpy(data_copy.data(), data.data(), CHUNK_SIZE * CHUNK_SIZE * sizeof(uint8_t));
	}

	uint16_t chunk::get_misc_data_at(int x, int y)
	{
		if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE)
			return misc_data[y][x];
		else
			return 0;
	}
	void chunk::set_misc_data_at(int x, int y, uint16_t value)
	{
		if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE)
			misc_data[y][x] = value;
	}
	void chunk::add_misc_data_at(int x, int y, int16_t value)
	{
		if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE)
		{
			misc_data[y][x] += value;
		}
	}

	/// @brief Set the tile at the given x and y coordinates
	/// This does not lock
	/// @param x
	/// @param y
	/// @param value
	void chunk::set_tile(int x, int y, uint8_t value)
	{
		if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE)
			data[y][x] = value;
	}
	/// @brief Set the tile copy at the given x and y coordinates
	void chunk::set_tile_copy(int x, int y, uint8_t value)
	{
		if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE)
			data_copy[y][x] = value;
	}

	/// @brief
	/// This does not lock
	/// @param x
	/// @param y
	/// @return
	uint8_t chunk::get_tile(int x, int y)
	{
		if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE)
			return data[y][x];
		else
			return 0;
	}
	uint8_t chunk::get_tile_using_buffer(int x, int y)
	{
		if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE)
			return data[y][x];
		else if ((y == -1 || y == CHUNK_SIZE) != (x == -1 || x == CHUNK_SIZE))
		{
			if (y == -1)
			{
				return neighbour_tile_buffer.top_tiles[x];
			}
			else if (y == CHUNK_SIZE)
			{
				return neighbour_tile_buffer.bottom_tiles[x];
			}
			else if (x == -1)
			{
				return neighbour_tile_buffer.left_tiles[y];
			}
			else if (x == CHUNK_SIZE)
			{
				return neighbour_tile_buffer.right_tiles[y];
			}
			else
				return 0;
		}
		else
		{
			return 0;
		}
	}

	uint8_t chunk::get_tile_copy(int x, int y)
	{
		if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE)
			return data_copy[y][x];
		else
			return 0;
	}
	float chunk::get_tile_temperature(int x, int y)
	{
		if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE)
			return temperature_data[y][x];
		else
			return 0;
	}
	float chunk::get_tile_temperature_using_buffer(int x, int y)
	{
		if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE)
			return temperature_data[y][x];
		else if ((y == -1 || y == CHUNK_SIZE) != (x == -1 || x == CHUNK_SIZE))
		{
			if (y == -1)
			{
				return neighbour_tile_buffer.top_temps[x];
			}
			else if (y == CHUNK_SIZE)
			{
				return neighbour_tile_buffer.bottom_temps[x];
			}
			else if (x == -1)
			{
				return neighbour_tile_buffer.left_temps[y];
			}
			else if (x == CHUNK_SIZE)
			{
				return neighbour_tile_buffer.right_temps[y];
			}
			else
				return 0;
		}
		else
		{
			return 0;
		}
	}

	void chunk::set_tile_temperature(int x, int y, float temperature)
	{
		if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE)
			temperature_data[y][x] = temperature;
		else if ((y == -1 || y == CHUNK_SIZE) != (x == -1 || x == CHUNK_SIZE))
		{
			if (y == -1)
			{
				neighbour_tile_buffer.top_temps[x] = temperature;
			}
			else if (y == CHUNK_SIZE)
			{
				neighbour_tile_buffer.bottom_temps[x] = temperature;
			}
			else if (x == -1)
			{
				neighbour_tile_buffer.left_temps[y] = temperature;
			}
			else if (x == CHUNK_SIZE)
			{
				neighbour_tile_buffer.right_temps[y] = temperature;
			}
		}
	}
	void chunk::add_tile_temperature(int x, int y, float temperature)
	{
		if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE)
		{
			if (temperature_data[y][x] + temperature > absolute_max_temperature)
			{
				temperature = absolute_max_temperature - temperature_data[y][x];
			}
			else
			{
				temperature_data[y][x] += temperature;
			}
		}
		else if ((y == -1 || y == CHUNK_SIZE) != (x == -1 || x == CHUNK_SIZE))
		{
			if (y == -1)
			{
				neighbour_tile_buffer.top_temps[x] += temperature;
			}
			else if (y == CHUNK_SIZE)
			{
				neighbour_tile_buffer.bottom_temps[x] += temperature;
			}
			else if (x == -1)
			{
				neighbour_tile_buffer.left_temps[y] += temperature;
			}
			else if (x == CHUNK_SIZE)
			{
				neighbour_tile_buffer.right_temps[y] += temperature;
			}
		}
	}

	bool chunk::isBoundaryTile(int x, int y)
	{
		uint16_t rows = (uint16_t)data.size();
		uint16_t cols = (uint16_t)data[0].size();
		for (int i = -1; i <= 1; i++)
		{
			for (int j = -1; j <= 1; j++)
			{
				int newX = x + i;
				int newY = y + j;
				if (newX < 0 || newX >= cols || newY < 0 || newY >= rows || data[newY][newX] == 0)
				{
					return true;
				}
			}
		}
		return false;
	}

	std::pair<int, int> chunk::getNextBoundaryTile2(int x, int y, std::pair<int, int> came_from)
	{
		// check N, NE, E, SE, S, SW, W, NW
		// The first direction that is a boundary tile is the next boundary tile
		uint16_t rows = (uint16_t)data.size();
		uint16_t cols = (uint16_t)data[0].size();
		int newX;
		int newY;
		for (int i = 0; i < sizeof(dx); i++)
		{
			int newDirection = i;
			newX = x + dx[newDirection];
			newY = y + dy[newDirection];
			if (newX == came_from.first && newY == came_from.second)
			{
				continue;
			}
			if (newX >= 0 && newX < cols && newY >= 0 && newY < rows && data[newY][newX] > 0 && isBoundaryTile(newX, newY))
			{
				return std::make_pair(newX, newY);
			}
		}
		return std::make_pair(-1, -1);
	}

	bool chunk::is_outline(tile_line l)
	{
		int x1, y1, x2, y2;
		if (l.x1 > l.x2)
		{
			x1 = l.x2;
			y1 = l.y2;
			x2 = l.x1;
			y2 = l.y1;
		}
		else if (l.x1 < l.x2)
		{
			x1 = l.x1;
			y1 = l.y1;
			x2 = l.x2;
			y2 = l.y2;
		}
		else if (l.y1 > l.y2)
		{
			x1 = l.x2;
			y1 = l.y2;
			x2 = l.x1;
			y2 = l.y1;
		}
		else if (l.y1 < l.y2)
		{
			x1 = l.x1;
			y1 = l.y1;
			x2 = l.x2;
			y2 = l.y2;
		}
		else
		{
			return false;
		}
		// l1 is <= l2 in x and y, but < in x or y
		if (x1 < 0 || x1 > CHUNK_SIZE || x2 < 0 || x2 > CHUNK_SIZE)
			return false;
		if (y1 < 0 || y1 > CHUNK_SIZE || y2 < 0 || y2 > CHUNK_SIZE)
			return false;

		if (x1 == x2)
		{
			// vertical line
			if (y1 == 0 && x1 == 0)
			{
				// top left of chunk down line
				return data[y1][x1];
			}
			if (x1 == 0)
			{
				// edge of chunk (left side)
				return data[y1][x1];
			}

			if (x1 == CHUNK_SIZE)
			{
				// edge of chunk (right side)
				return data[y1][x1 - 1];
			}

			return data[y1][x1] && !data[y1][x1 - 1] || !data[y1][x1] && data[y1][x1 - 1];
		}
		else if (y1 == y2)
		{
			// horizontal line
			if (x1 == 0 && y1 == 0)
			{
				// top left right line
				return data[y1][x1];
			}
			if (y1 == 0)
			{
				// edge of chunk (top side)
				return data[y1][x1];
			}
			if (y1 == CHUNK_SIZE)
			{
				// edge of chunk (bottom side)
				return data[y1 - 1][x1];
			}

			return data[y1][x1] && !data[y1 - 1][x1] || !data[y1][x1] && data[y1 - 1][x1];
		}

		// int x1 = l.x1;
		// int y1 = l.y1;
		// int x2 = l.x2;
		// int y2 = l.y2;
		// if (x1 < 0 || x1 >= CHUNK_SIZE || x2 < 0 || x2 >= CHUNK_SIZE)
		// 	return false;
		// if (y1 < 0 || y1 >= CHUNK_SIZE || y2 < 0 || y2 >= CHUNK_SIZE)
		// 	return false;
		// if (x1 == x2)
		// {
		// 	// vertical line, compare tiles to the left and right
		// 	if (y1 < y2)
		// 	{
		// 		// check for edge of chunk
		// 		if (x1 == 0 || x1 == CHUNK_SIZE - 1)
		// 		{
		// 			return data[y1][x1];
		// 		}
		// 		if (data[y1][x1] && !data[y1][x1 - 1] || !data[y1][x1] && data[y1][x1 - 1])
		// 			return true;
		// 	}
		// 	else
		// 	{
		// 		// check for edge of chunk
		// 		if (x1 == 0 || x1 == CHUNK_SIZE - 1)
		// 		{
		// 			return data[y2][x2];
		// 		}
		// 		if (data[y2][x2] && !data[y2][x2 - 1] || !data[y2][x2] && data[y2][x2 - 1])
		// 			return true;
		// 	}
		// }
		// else
		// {
		// 	// horizontal line, compare tiles above and below
		// 	if (x1 < x2)
		// 	{
		// 		// check for edge of chunk
		// 		if (y1 == 0 || y1 == CHUNK_SIZE - 1)
		// 		{
		// 			return data[y1][x1];
		// 		}
		// 		if (data[y1][x1] && !data[y1 - 1][x1] || !data[y1][x1] && data[y1 - 1][x1])
		// 			return true;
		// 	}
		// 	else
		// 	{
		// 		// check for edge of chunk
		// 		if (y1 == 0 || y1 == CHUNK_SIZE - 1)
		// 		{
		// 			return data[y2][x2];
		// 		}
		// 		if (data[y2][x2] && !data[y2 - 1][x2] || !data[y2][x2] && data[y2 - 1][x2])
		// 			return true;
		// 	}
		// }
		// if (x1 == x2)
		// {
		// 	// if edge of chunk
		// 	// if (x1 == 0 || x1 == CHUNK_SIZE - 1)
		// 	// 	return true;
		// 	if (y1 < y2)
		// 	{
		// 		// if (!data[y1][x1] != !data[y2][x1])
		// 		// 	return true;
		// 		if (x1 == 0)
		// 		{
		// 			if (data[])
		// 		}
		// 		if (data[y1][x1] && !data[y2][x1] || !data[y1][x1] && data[y2][x1])
		// 			return true;
		// 	}
		// 	else
		// 	{
		// 		// if (!data[y1][x1] != !data[y2][x1])
		// 		// 	return true;
		// 		if (data[y1][x1] && !data[y2][x1] || !data[y1][x1] && data[y2][x1])
		// 			return true;
		// 	}
		// }
		// else if (y1 == y2)
		// {
		// 	// if edge of chunk
		// 	if (y1 == 0 || y1 == CHUNK_SIZE - 1)
		// 		return true;
		// 	if (x1 < x2)
		// 	{
		// 		// if (!data[y1][x1] != !data[y1][x2])
		// 		// 	return true;
		// 		if (data[y1][x1] && !data[y1][x2] || !data[y1][x1] && data[y1][x2])
		// 			return true;
		// 	}
		// 	else
		// 	{
		// 		// if (!data[y1][x1] != !data[y1][x2])
		// 		// 	return true;
		// 		if (data[y1][x1] && !data[y1][x2] || !data[y1][x1] && data[y1][x2])
		// 			return true;
		// 	}
		// }
		return false;
	}

	bool chunk::is_edge(int x, int y)
	{
		if (!data[y][x])
			return false;

		if (x == 0 || x == CHUNK_SIZE - 1 || y == 0 || y == CHUNK_SIZE - 1)
		{
			return data[y][x];
		}

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (data[y + dy[i]][x + dx[j]] == 0)
					return true;
			}
		}
		return false;
	}

	std::vector<std::pair<int, int>> chunk::trace_outline(tile_line start_line, std::unordered_set<tile_line, tile_line_hash> &visited_lines)
	{
		visited_lines.insert(start_line);
		std::vector<std::pair<int, int>> outline;
		// line current_line = start_line;
		std::pair<int, int> start_point = {start_line.x1, start_line.y1};

		outline.push_back(std::make_pair(start_point.first + chunk_x * game::CHUNK_SIZE, start_point.second + chunk_y * game::CHUNK_SIZE));
		std::pair<int, int> current_point = {start_line.x2, start_line.y2};
		// outline.push_back(current_point);

		while (current_point != start_point)
		{
			std::pair<int, int> p = std::make_pair(current_point.first + chunk_x * game::CHUNK_SIZE, current_point.second + chunk_y * game::CHUNK_SIZE);
			outline.push_back(p);

			tile_line up = {current_point.first, current_point.second, current_point.first, current_point.second - 1};
			tile_line right = {current_point.first, current_point.second, current_point.first + 1, current_point.second};
			tile_line left = {current_point.first, current_point.second, current_point.first - 1, current_point.second};
			tile_line down = {current_point.first, current_point.second, current_point.first, current_point.second + 1};

			if (!visited_lines.count(up))
			{
				if (is_outline(up))
				{
					visited_lines.insert(up);
					current_point = {up.x2, up.y2};
					continue;
				}
			}

			if (!visited_lines.count(right))
			{
				if (is_outline(right))
				{
					visited_lines.insert(right);
					current_point = {right.x2, right.y2};
					continue;
				}
			}

			if (!visited_lines.count(down))
			{
				if (is_outline(down))
				{
					visited_lines.insert(down);
					current_point = {down.x2, down.y2};
					continue;
				}
			}

			if (!visited_lines.count(left))
			{
				if (is_outline(left))
				{
					visited_lines.insert(left);
					current_point = {left.x2, left.y2};
					continue;
				}
			}
			// no line, exit
			break;
		}

		return outline;
	}

	const int adjacent_tiles_dx[4] = {-1, 0, 0, -1}; // bottom left, bottom right, top right, top left
	const int adjacent_tiles_dy[4] = {0, 0, -1, -1};
	const int offset[4] = {1, 2, 4, 8};
	uint16_t chunk::get_tile_edginess(int x, int y)
	{
		uint16_t edginess = 0;

		for (int i = 0; i < 4; i++)
		{
			int adjacent_x = x + adjacent_tiles_dx[i];
			int adjacent_y = y + adjacent_tiles_dy[i];
			if (adjacent_x < 0 || adjacent_x >= CHUNK_SIZE || adjacent_y < 0 || adjacent_y >= CHUNK_SIZE)
			{
				continue;
			}
			// if (data[adjacent_y][adjacent_x] >= SOLID_TILE_START_INDEX)
			if (is_solid_tile[data[adjacent_y][adjacent_x]])
			{
				edginess += offset[i];
			}
		}

		return edginess;
	}

	const std::array<std::vector<tile_linef>, 16> chunk_edges_lines = {
		{{},
		 {{-0.5, 0, 0, 0.5}},
		 {{0, 0.5, 0.5, 0}},
		 {{-0.5, 0, 0.5, 0}},
		 {{0, -0.5, 0.5, 0}},
		 {{-0.5, 0, 0, -0.5}, {0, 0.5, 0.5, 0}},
		 {{0, 0.5, 0, -0.5}},
		 {{-0.5, 0, 0, -0.5}},
		 {{-0.5, 0, 0, -0.5}},
		 {{0, -0.5, 0, 0.5}},
		 {{-0.5, 0, 0, 0.5}, {0, -0.5, 0.5, 0}},
		 {{0, -0.5, 0.5, 0}},
		 {{-0.5, 0, 0.5, 0}},
		 {{0, 0.5, 0.5, 0}},
		 {{-0.5, 0, 0, 0.5}},
		 {}}};

	void chunk::create_outlines(std::vector<std::vector<std::pair<float, float>>> *chunk_outline)
	{
		lock_chunk_shared();
		// start time
		auto start = std::chrono::high_resolution_clock::now();
		std::unordered_map<std::pair<float, float>, line_mapping_pair, chunk_coord_pair_hash> edge_lines;
		std::unordered_map<std::pair<float, float>, line_mapping_pair, chunk_coord_pair_hash> edge_lines_reverse;

		for (int y = 0; y < CHUNK_SIZE + 1; y++)
		{
			for (int x = 0; x < CHUNK_SIZE + 1; x++)
			{
				// if (data[y][x] >= SOLID_TILE_START_INDEX)
				// {
				uint16_t edginess = get_tile_edginess(x, y);
				for (tile_linef line : chunk_edges_lines[edginess])
				{
					line.x1 += x;
					line.y1 += y;
					line.x2 += x;
					line.y2 += y;
					if (!edge_lines.count({line.x1, line.y1}))
					{
						edge_lines[{line.x1, line.y1}] = line_mapping_pair({line.x2, line.y2});

						if (!edge_lines_reverse.count({line.x2, line.y2}))
							edge_lines_reverse[{line.x2, line.y2}] = line_mapping_pair({line.x1, line.y1});
						else
							edge_lines_reverse[{line.x2, line.y2}].insert({line.x1, line.y1});
					}
					else
					{
						edge_lines[{line.x1, line.y1}].insert({line.x2, line.y2});
						if (!edge_lines_reverse.count({line.x2, line.y2}))
							edge_lines_reverse[{line.x2, line.y2}] = line_mapping_pair({line.x1, line.y1});
						else
							edge_lines_reverse[{line.x2, line.y2}].insert({line.x1, line.y1});
					}
				}
				// }
			}
		}
		unlock_chunk_shared();

		auto last_time = std::chrono::high_resolution_clock::now();
		auto running_duration = last_time - start;
		// printf("		marching squares: %f mis\n", running_duration.count() / 1000.0);

		std::vector<std::vector<std::pair<float, float>>> outlines;
		while (1)
		{
			if (edge_lines.empty())
				break;
			std::vector<std::pair<float, float>> current_outline;
			std::pair<float, float> start_point = edge_lines.begin()->first;

			// current_outline.push_back(start_point);
			std::pair<float, float> current_point = start_point;

			do
			{
				std::pair<float, float> next_point;
				if (edge_lines.count(current_point))
				{
					next_point = edge_lines[current_point].get_next();
					if (edge_lines[current_point].p1.first == -1)
					{
						edge_lines.erase(current_point);
					}
					edge_lines_reverse[next_point].remove_point(current_point);
					if (edge_lines_reverse[next_point].p1.first == -1)
					{
						edge_lines_reverse.erase(next_point);
					}
				}
				else if (edge_lines_reverse.count(current_point))
				{
					next_point = edge_lines_reverse[current_point].get_next();
					if (edge_lines_reverse[current_point].p1.first == -1)
					{
						edge_lines_reverse.erase(current_point);
					}
					edge_lines[next_point].remove_point(current_point);
					if (edge_lines[next_point].p1.first == -1)
					{
						edge_lines.erase(next_point);
					}
				}
				else
				{
					printf("Error: outline ended prematurely\n");
					break;
				}

				if (next_point.first == -1)
				{
					printf("Error: outline ended prematurely\n");
					break;
				}

				current_point = next_point;

				current_outline.push_back(current_point);

			} while (start_point != current_point);

			outlines.push_back(current_outline);
		}

		running_duration = std::chrono::high_resolution_clock::now() - last_time;
		// last_time = std::chrono::high_resolution_clock::now();
		// printf("		Connecting edges: %f mis\n", running_duration.count() / 1000.0);

		// std::vector<std::vector<std::pair<float, float>>> * outlines_triangles = new std::vector<std::vector<std::pair<float, float>>>();
		int vert_retention_count = 4;
		for (int i = 0; i < outlines.size(); i++)
		{
			std::vector<p2t::Point *> outline_points;
			// printf("\nNew outline\n");
			for (int j = 0; j < outlines[i].size(); j++)
			{
				// printf("%d, %d\n", outlines[i][j].x1, outlines[i][j].y1);
				if (outlines[i].size() < 12 || j % vert_retention_count == 0)
				{
					// should also remove useless points
					outline_points.push_back(new p2t::Point(outlines[i][j].first, outlines[i][j].second));
				}
			}
			if (outline_points.size() < 3)
				continue;

			try
			{
				p2t::CDT *cdt = new p2t::CDT(outline_points);
				cdt->Triangulate();
				std::vector<p2t::Triangle *> triangles = cdt->GetTriangles();
				std::vector<std::pair<float, float>> outline_triangles;

				for (int j = 0; j < triangles.size(); j++)
				{
					p2t::Point *p1 = triangles[j]->GetPoint(0);
					p2t::Point *p2 = triangles[j]->GetPoint(1);
					p2t::Point *p3 = triangles[j]->GetPoint(2);

					outline_triangles.push_back(std::make_pair((float)(p1->x + chunk_x * CHUNK_SIZE), (float)(p1->y + chunk_y * CHUNK_SIZE)));
					outline_triangles.push_back(std::make_pair((float)(p2->x + chunk_x * CHUNK_SIZE), (float)(p2->y + chunk_y * CHUNK_SIZE)));
					outline_triangles.push_back(std::make_pair((float)(p3->x + chunk_x * CHUNK_SIZE), (float)(p3->y + chunk_y * CHUNK_SIZE)));
				}

				chunk_outline->push_back(outline_triangles);
				for (p2t::Point *p : outline_points)
					delete p;
				delete cdt;
			}
			catch (std::exception e)
			{
				// printf("Exception  : %s\n", e.what());
				continue;
			}
		}

		running_duration = std::chrono::high_resolution_clock::now() - last_time;
		// last_time = std::chrono::high_resolution_clock::now();
		// printf("		triangulating: %f mis\n", running_duration.count() / 1000.0);

		// auto end = std::chrono::high_resolution_clock::now();
		// auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		// printf("	Time taken by entire function: %d\n", duration.count());
	}

	uint32_t chunk::delete_circle(int x, int y, int radius, std::unordered_set<uint8_t> tile_deny_list)
	{
		if (radius == 0)
			return false;

		int local_x = x - chunk_x * CHUNK_SIZE;
		int local_y = y - chunk_y * CHUNK_SIZE;

		int x0 = local_x - radius;
		int x1 = local_x + radius;
		int y0 = local_y - radius;
		int y1 = local_y + radius;
		if (x0 > CHUNK_SIZE || y0 > CHUNK_SIZE || x1 < 0 || y1 < 0)
			return false;

		lock_chunk();

		uint32_t tiles_deleted = 0;

		for (int y = y0; y <= y1; y++)
		{
			for (int x = x0; x <= x1; x++)
			{
				if (x < 0 || y < 0 || x >= CHUNK_SIZE || y >= CHUNK_SIZE)
					continue;
				if (data[y][x] == tile_type::BEDROCK || data[y][x] < SOLID_TILE_START_INDEX)
					continue;
				if (tile_deny_list.count(data[y][x]))
					continue;
				if ((x - local_x) * (x - local_x) + (y - local_y) * (y - local_y) <= radius * radius)
				{
					if (game_engine::in_set(data[y][x], ROOT, WOOD, LEAF))
					{
						data[y][x] = EMBER;
					}
					else
					{
						data[y][x] = TEMPORARY_SMOKE;
					}
					tiles_deleted++;
				}
			}
		}
		unlock_chunk();
		return tiles_deleted;
	}

	uint32_t chunk::explode_circle(int x, int y, int radius, float max_temperature, std::unordered_set<uint8_t> tile_deny_list)
	{
		if (radius == 0)
			return false;

		int circle_center_x = x;
		int circle_center_y = y;

		int local_x = x - chunk_x * CHUNK_SIZE;
		int local_y = y - chunk_y * CHUNK_SIZE;

		int x0 = local_x - radius;
		int x1 = local_x + radius;
		int y0 = local_y - radius;
		int y1 = local_y + radius;
		if (x0 > CHUNK_SIZE || y0 > CHUNK_SIZE || x1 < 0 || y1 < 0)
			return false;

		lock_chunk();

		uint32_t tiles_affected = 0;

		for (int y = y0; y <= y1; y++)
		{
			for (int x = x0; x <= x1; x++)
			{
				if (x < 0 || y < 0 || x >= CHUNK_SIZE || y >= CHUNK_SIZE)
					continue;
				if (data[y][x] == tile_type::BEDROCK || data[y][x] < SOLID_TILE_START_INDEX)
					continue;
				if (tile_deny_list.count(data[y][x]))
					continue;
				if ((x - local_x) * (x - local_x) + (y - local_y) * (y - local_y) <= radius * radius)
				{
					tile_type t = (tile_type)data[y][x];
					float temperature = temperature_data[y][x];
					uint16_t misc = misc_data[y][x];

					data[y][x] = rand() % 2 == 0 ? TEMPORARY_SMOKE : AIR;

					// velocity away from center
					float dx = (x + chunk_x * CHUNK_SIZE - circle_center_x);
					float dy = (y + chunk_y * CHUNK_SIZE - circle_center_y);
					float distance = sqrt(dx * dx + dy * dy);
					float normalized_dx = distance / radius;
					// float vel_x = dx / distance;
					// float vel_y = dy / distance;

					game_engine::task_scheduler_pointer->add_task({&create_single_debris_task, new create_debris_params(x + chunk_x * CHUNK_SIZE, y + chunk_y * CHUNK_SIZE, dx + (rand() % 100 - 50) / 50.0, -dy + (rand() % 100 - 50) / 50.0, 0.15, t, AIR, t, 250, temperature + max_temperature * (1 - normalized_dx), misc)});
				}
			}
		}
		unlock_chunk();
		return tiles_affected;
	}

	bool chunk::find_tile_in_rect(std::pair<int, int> &result, int x, int y, int w, int h, std::unordered_set<uint8_t> tile_types)
	{
		lock_chunk_shared();
		// check if the rect is in the chunk
		if (x / CHUNK_SIZE <= chunk_x && (x + w) / CHUNK_SIZE >= chunk_x && y / CHUNK_SIZE <= chunk_y && (y + h) / CHUNK_SIZE >= chunk_y)
		{
			int local_x = x - chunk_x * CHUNK_SIZE;
			int local_y = y - chunk_y * CHUNK_SIZE;

			for (int i = local_y; i < local_y + h; i++)
			{
				for (int j = local_x; j < local_x + w; j++)
				{
					if (j < 0 || i < 0 || j >= CHUNK_SIZE || i >= CHUNK_SIZE)
						continue;
					if (tile_types.count(data[i][j]))
					{
						result = std::make_pair(j + chunk_x * CHUNK_SIZE, i + chunk_y * CHUNK_SIZE);
						return true;
					}
				}
			}
		}
		unlock_chunk_shared();
		return false;
	}

	void chunk::update_frame(uint8_t frame)
	{
		current_frame = frame;
	}

	bool chunk::try_place_tile_with_displacement_no_lock(int x, int y, tile_type tile_t, float temperature, uint16_t misc_data, int recursion_depth, int search_size)
	{
		std::unordered_set<tile_coord, intra_chunk_tile_coord_hash> checked_tiles;

		std::queue<tile_coord> tile_queue;
		tile_queue.push(tile_coord(x, y));
		int dx[] = {0, 1, 0, -1};
		int dy[] = {-1, 0, 1, 0};

		tile_simple_type simple_type = get_simple_tile_type(tile_t);

		bool found_non_solid = false;
		while (!tile_queue.empty() && checked_tiles.size() <= search_size)
		{
			tile_coord current = tile_queue.front();
			tile_queue.pop();

			if (current.x < 0 || current.y < 0 || current.x >= CHUNK_SIZE || current.y >= CHUNK_SIZE)
				continue;

			tile_type current_tile = (tile_type)get_tile(current.x, current.y);

			if (!found_non_solid && !is_solid_tile[current_tile])
			{
				found_non_solid = true;
				tile_queue = std::queue<tile_coord>();
				checked_tiles.clear();
			}

			if (found_non_solid && !is_solid_tile[current_tile] && (recursion_depth == 0 || current_tile < tile_t))
			{
				float current_temp = get_tile_temperature(current.x, current.y);
				uint16_t current_misc_data = get_misc_data_at(current.x, current.y);
				switch (current_tile)
				{
				// case VACUUM:
				case AIR:
					set_tile(current.x, current.y, tile_t);
					set_tile_temperature(current.x, current.y, current_temp);
					set_misc_data_at(current.x, current.y, current_misc_data);
					return true;
				default:
					if (recursion_depth > 10)
					{
						printf("recursion depth exceeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeded\n");
						return false;
					}
					bool success = try_place_tile_with_displacement_no_lock(current.x, current.y, current_tile, current_temp, current_misc_data, recursion_depth + 1, search_size);

					if (success)
					{
						set_tile(current.x, current.y, tile_t);
						set_tile_temperature(current.x, current.y, current_temp);
						set_misc_data_at(current.x, current.y, current_misc_data);
					}
					return success;
				}
			}

			for (int i = 0; i < 4; i++)
			{
				tile_coord new_tile = tile_coord(current.x + dx[i], current.y + dy[i]);
				if (!checked_tiles.count(new_tile))
				{
					if (found_non_solid)
					{
						if (is_solid_tile[get_tile(new_tile.x, new_tile.y)] == 0)
						{
							tile_queue.push(new_tile);
							checked_tiles.insert(new_tile);
						}
					}
					else
					{
						tile_queue.push(new_tile);
						checked_tiles.insert(new_tile);
					}
				}
			}
		}

		return false;
	}

	bool chunk::try_consume_nearby_tile_no_lock(int x, int y, tile_type tile_t, int search_size)
	{
		std::unordered_set<tile_coord, intra_chunk_tile_coord_hash> checked_tiles;
		std::queue<tile_coord> tile_queue;
		tile_queue.push(tile_coord(x, y));
		int dx[] = {0, 1, 0, -1};
		int dy[] = {-1, 0, 1, 0};

		while (!tile_queue.empty() && checked_tiles.size() <= search_size)
		{
			tile_coord current = tile_queue.front();
			tile_queue.pop();

			if (current.x < 0 || current.y < 0 || current.x >= CHUNK_SIZE || current.y >= CHUNK_SIZE)
				continue;

			tile_type current_tile = (tile_type)get_tile(current.x, current.y);

			if (current_tile == tile_t)
			{
				set_tile(current.x, current.y, AIR);
				return true;
			}

			for (int i = 0; i < 4; i++)
			{
				tile_coord new_tile = tile_coord(current.x + dx[i], current.y + dy[i]);
				if (!checked_tiles.count(new_tile))
				{
					tile_queue.push(new_tile);
					checked_tiles.insert(new_tile);
				}
			}
		}

		return false;
	}

	void chunk::switch_tiles_no_lock(int x1, int y1, int x2, int y2)
	{
		if (x1 < 0 || y1 < 0 || x1 >= CHUNK_SIZE || y1 >= CHUNK_SIZE || x2 < 0 || y2 < 0 || x2 >= CHUNK_SIZE || y2 >= CHUNK_SIZE)
		{
			if ((x1 < 0 || y1 < 0 || x1 >= CHUNK_SIZE || y1 >= CHUNK_SIZE) && !((y1 == -1 || y1 == CHUNK_SIZE) != (x1 == -1 || x1 == CHUNK_SIZE)))
			{
				return;
			}
			if ((x2 < 0 || y2 < 0 || x2 >= CHUNK_SIZE || y2 >= CHUNK_SIZE) && !((y2 == -1 || y2 == CHUNK_SIZE) != (x2 == -1 || x2 == CHUNK_SIZE)))
			{
				return;
			}
			uint8_t t1;
			uint8_t t2;
			float temp1;
			float temp2;
			uint16_t misc1;
			uint16_t misc2;

			if (x1 == -1)
			{
				t1 = neighbour_tile_buffer.left_tiles[y1];
				temp1 = neighbour_tile_buffer.left_temps[y1];
				misc1 = neighbour_tile_buffer.left_misc[y1];
				t2 = get_tile(x2, y2);
				temp2 = get_tile_temperature(x2, y2);
				misc2 = get_misc_data_at(x2, y2);
				neighbour_tile_buffer.left_tiles[y1] = t2;
				neighbour_tile_buffer.left_temps[y1] = temp2;
				neighbour_tile_buffer.left_misc[y1] = misc2;
				set_tile(x2, y2, t1);
				set_tile_temperature(x2, y2, temp1);
				set_misc_data_at(x2, y2, misc1);
			}
			else if (x1 == CHUNK_SIZE)
			{
				t1 = neighbour_tile_buffer.right_tiles[y1];
				temp1 = neighbour_tile_buffer.right_temps[y1];
				misc1 = neighbour_tile_buffer.right_misc[y1];
				t2 = get_tile(x2, y2);
				temp2 = get_tile_temperature(x2, y2);
				misc2 = get_misc_data_at(x2, y2);
				neighbour_tile_buffer.right_tiles[y1] = t2;
				neighbour_tile_buffer.right_temps[y1] = temp2;
				neighbour_tile_buffer.right_misc[y1] = misc2;
				set_tile(x2, y2, t1);
				set_tile_temperature(x2, y2, temp1);
				set_misc_data_at(x2, y2, misc1);
			}
			else if (y1 == -1)
			{
				t1 = neighbour_tile_buffer.top_tiles[x1];
				temp1 = neighbour_tile_buffer.top_temps[x1];
				misc1 = neighbour_tile_buffer.top_misc[x1];
				t2 = get_tile(x2, y2);
				temp2 = get_tile_temperature(x2, y2);
				misc2 = get_misc_data_at(x2, y2);
				neighbour_tile_buffer.top_tiles[x1] = t2;
				neighbour_tile_buffer.top_temps[x1] = temp2;
				neighbour_tile_buffer.top_misc[x1] = misc2;
				set_tile(x2, y2, t1);
				set_tile_temperature(x2, y2, temp1);
				set_misc_data_at(x2, y2, misc1);
			}
			else if (y1 == CHUNK_SIZE)
			{
				t1 = neighbour_tile_buffer.bottom_tiles[x1];
				temp1 = neighbour_tile_buffer.bottom_temps[x1];
				misc1 = neighbour_tile_buffer.bottom_misc[x1];
				t2 = get_tile(x2, y2);
				temp2 = get_tile_temperature(x2, y2);
				misc2 = get_misc_data_at(x2, y2);
				neighbour_tile_buffer.bottom_tiles[x1] = t2;
				neighbour_tile_buffer.bottom_temps[x1] = temp2;
				neighbour_tile_buffer.bottom_misc[x1] = misc2;
				set_tile(x2, y2, t1);
				set_tile_temperature(x2, y2, temp1);
				set_misc_data_at(x2, y2, misc1);
			}
			else if (x2 == -1)
			{
				t2 = neighbour_tile_buffer.left_tiles[y2];
				temp2 = neighbour_tile_buffer.left_temps[y2];
				misc2 = neighbour_tile_buffer.left_misc[y2];
				t1 = get_tile(x1, y1);
				temp1 = get_tile_temperature(x1, y1);
				misc1 = get_misc_data_at(x1, y1);
				neighbour_tile_buffer.left_tiles[y2] = t1;
				neighbour_tile_buffer.left_temps[y2] = temp1;
				neighbour_tile_buffer.left_misc[y2] = misc1;
				set_tile(x1, y1, t2);
				set_tile_temperature(x1, y1, temp2);
				set_misc_data_at(x1, y1, misc2);
			}
			else if (x2 == CHUNK_SIZE)
			{
				t2 = neighbour_tile_buffer.right_tiles[y2];
				temp2 = neighbour_tile_buffer.right_temps[y2];
				misc2 = neighbour_tile_buffer.right_misc[y2];
				t1 = get_tile(x1, y1);
				temp1 = get_tile_temperature(x1, y1);
				misc1 = get_misc_data_at(x1, y1);
				neighbour_tile_buffer.right_tiles[y2] = t1;
				neighbour_tile_buffer.right_temps[y2] = temp1;
				neighbour_tile_buffer.right_misc[y2] = misc1;
				set_tile(x1, y1, t2);
				set_tile_temperature(x1, y1, temp2);
				set_misc_data_at(x1, y1, misc2);
			}
			else if (y2 == -1)
			{
				t2 = neighbour_tile_buffer.top_tiles[x2];
				temp2 = neighbour_tile_buffer.top_temps[x2];
				misc2 = neighbour_tile_buffer.top_misc[x2];
				t1 = get_tile(x1, y1);
				temp1 = get_tile_temperature(x1, y1);
				misc1 = get_misc_data_at(x1, y1);
				neighbour_tile_buffer.top_tiles[x2] = t1;
				neighbour_tile_buffer.top_temps[x2] = temp1;
				neighbour_tile_buffer.top_misc[x2] = misc1;
				set_tile(x1, y1, t2);
				set_tile_temperature(x1, y1, temp2);
				set_misc_data_at(x1, y1, misc2);
			}
			else if (y2 == CHUNK_SIZE)
			{
				t2 = neighbour_tile_buffer.bottom_tiles[x2];
				temp2 = neighbour_tile_buffer.bottom_temps[x2];
				misc2 = neighbour_tile_buffer.bottom_misc[x2];
				t1 = get_tile(x1, y1);
				temp1 = get_tile_temperature(x1, y1);
				misc1 = get_misc_data_at(x1, y1);
				neighbour_tile_buffer.bottom_tiles[x2] = t1;
				neighbour_tile_buffer.bottom_temps[x2] = temp1;
				neighbour_tile_buffer.bottom_misc[x2] = misc1;
				set_tile(x1, y1, t2);
				set_tile_temperature(x1, y1, temp2);
				set_misc_data_at(x1, y1, misc2);
			}
			return;
		}

		tile_type tile1 = (tile_type)get_tile(x1, y1);
		tile_type tile2 = (tile_type)get_tile(x2, y2);
		float temp1 = get_tile_temperature(x1, y1);
		float temp2 = get_tile_temperature(x2, y2);
		uint16_t misc1 = get_misc_data_at(x1, y1);
		uint16_t misc2 = get_misc_data_at(x2, y2);
		set_tile(x1, y1, tile2);
		set_tile_temperature(x1, y1, temp2);
		set_misc_data_at(x1, y1, misc2);
		set_tile(x2, y2, tile1);
		set_tile_temperature(x2, y2, temp1);
		set_misc_data_at(x2, y2, misc1);
	}

	tile_simple_type get_simple_tile_type(uint8_t tile)
	{
		if (tile < LIQUID_TILE_START_INDEX)
		{
			return GAS;
		}
		else if (tile < SOLID_TILE_START_INDEX)
		{
			return LIQUID;
		}
		else if (tile < BACKGROUND_TILE_START_INDEX)
		{
			return SOLID;
		}
		else
		{
			return BACKGROUND_TILE;
		}
	}

	enum chunk_side
	{
		CHUNK_SIDE_TOP = 0,
		CHUNK_SIDE_RIGHT = 1,
		CHUNK_SIDE_BOTTOM = 2,
		CHUNK_SIDE_LEFT = 3
	};

	chunk_neighbour_tile_buffer *chunk::get_neighbour_tile_buffer_pointer()
	{
		return &neighbour_tile_buffer;
	}

	/// @brief Copies the edge tiles of the chunk into the provided buffer for neighbour chunk updates
	/// @param buffer 
	/// @param side The side that this chunk touches the original chunk
	void chunk::get_neighbour_tile_buffer(chunk_neighbour_tile_buffer *buffer, uint8_t side)
	{
		lock_chunk_shared();
		switch (side)
		{
		case CHUNK_SIDE_TOP:
			memcpy(&buffer->top_tiles, &data[CHUNK_SIZE - 1], sizeof(uint8_t) * CHUNK_SIZE);
			memcpy(&buffer->top_temps, &temperature_data[CHUNK_SIZE - 1], sizeof(float) * CHUNK_SIZE);
			memcpy(&buffer->top_misc, &misc_data[CHUNK_SIZE - 1], sizeof(uint16_t) * CHUNK_SIZE);
			break;
		case CHUNK_SIDE_RIGHT:
			for (int i = 0; i < CHUNK_SIZE; i++)
			{
				buffer->right_tiles[i] = data[i][0];
				buffer->right_temps[i] = temperature_data[i][0];
				buffer->right_misc[i] = misc_data[i][0];
			}
			break;
		case CHUNK_SIDE_BOTTOM:
			memcpy(&buffer->bottom_tiles, &data[0], sizeof(uint8_t) * CHUNK_SIZE);
			memcpy(&buffer->bottom_temps, &temperature_data[0], sizeof(float) * CHUNK_SIZE);
			memcpy(&buffer->bottom_misc, &misc_data[0], sizeof(uint16_t) * CHUNK_SIZE);
			break;
		case CHUNK_SIDE_LEFT:
			for (int i = 0; i < CHUNK_SIZE; i++)
			{
				buffer->left_tiles[i] = data[i][CHUNK_SIZE - 1];
				buffer->left_temps[i] = temperature_data[i][CHUNK_SIZE - 1];
				buffer->left_misc[i] = misc_data[i][CHUNK_SIZE - 1];
			}
			break;
		}
		unlock_chunk_shared();
	}

	void chunk::update_neighbour_tiles(chunk_neighbour_tile_buffer *buffer, chunk_neighbour_tile_buffer *buffer_original, uint8_t side)
	{
		lock_chunk();

		switch (side)
		{
		case CHUNK_SIDE_TOP:
			for (int i = 0; i < CHUNK_SIZE; i++)
			{
				if (buffer->top_tiles[i] != buffer_original->top_tiles[i])
				{
					data[CHUNK_SIZE - 1][i] = buffer->top_tiles[i];
				}
				if (buffer->top_temps[i] != buffer_original->top_temps[i])
				{
					temperature_data[CHUNK_SIZE - 1][i] += buffer->top_temps[i] - buffer_original->top_temps[i];
				}
				if (buffer->top_misc[i] != buffer_original->top_misc[i])
				{
					misc_data[CHUNK_SIZE - 1][i] = buffer->top_misc[i];
				}
			}
			break;
		case CHUNK_SIDE_RIGHT:
			for (int i = 0; i < CHUNK_SIZE; i++)
			{
				if (buffer->right_tiles[i] != buffer_original->right_tiles[i])
				{
					data[i][0] = buffer->right_tiles[i];
				}
				if (buffer->right_temps[i] != buffer_original->right_temps[i])
				{
					temperature_data[i][0] += buffer->right_temps[i] - buffer_original->right_temps[i];
				}
				if (buffer->right_misc[i] != buffer_original->right_misc[i])
				{
					misc_data[i][0] = buffer->right_misc[i];
				}
			}
			break;
		case CHUNK_SIDE_BOTTOM:
			for (int i = 0; i < CHUNK_SIZE; i++)
			{
				if (buffer->bottom_tiles[i] != buffer_original->bottom_tiles[i])
				{
					data[0][i] = buffer->bottom_tiles[i];
				}
				if (buffer->bottom_temps[i] != buffer_original->bottom_temps[i])
				{
					temperature_data[0][i] += buffer->bottom_temps[i] - buffer_original->bottom_temps[i];
				}
				if (buffer->bottom_misc[i] != buffer_original->bottom_misc[i])
				{
					misc_data[0][i] = buffer->bottom_misc[i];
				}
			}
			break;
		case CHUNK_SIDE_LEFT:
			for (int i = 0; i < CHUNK_SIZE; i++)
			{
				if (buffer->left_tiles[i] != buffer_original->left_tiles[i])
				{
					data[i][CHUNK_SIZE - 1] = buffer->left_tiles[i];
				}
				if (buffer->left_temps[i] != buffer_original->left_temps[i])
				{
					temperature_data[i][CHUNK_SIZE - 1] += buffer->left_temps[i] - buffer_original->left_temps[i];
				}
				if (buffer->left_misc[i] != buffer_original->left_misc[i])
				{
					misc_data[i][CHUNK_SIZE - 1] = buffer->left_misc[i];
				}
			}
			break;
		}

		unlock_chunk();
	}
}