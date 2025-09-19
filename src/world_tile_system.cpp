#include "world_tile_system.hpp"
#include "box2d_system.hpp"
#include "tasks.hpp"
#include "legged_creature_system.hpp"

namespace game
{
	// const uint16_t NUM_CHUNKS = 16; // 3x3 chunks in world
	// const uint16_t CHUNKS_WIDTH = 4;
	
	// const std::array<std::vector<tile_linef>, 16> edges_lines = {
	// 	{{},
	// 	 {{-0.5, 0, 0, 0.5}},
	// 	 {{0, 0.5, 0.5, 0}},
	// 	 {{-0.5, 0, 0.5, 0}},
	// 	 {{0, -0.5, 0.5, 0}},
	// 	 {{-0.5, 0, 0, -0.5}, {0, 0.5, 0.5, 0}},
	// 	 {{0, 0.5, 0, -0.5}},
	// 	 {{-0.5, 0, 0, -0.5}},
	// 	 {{-0.5, 0, 0, -0.5}},
	// 	 {{0, -0.5, 0, 0.5}},
	// 	 {{-0.5, 0, 0, 0.5}, {0, -0.5, 0.5, 0}},
	// 	 {{0, -0.5, 0.5, 0}},
	// 	 {{-0.5, 0, 0.5, 0}},
	// 	 {{0, 0.5, 0.5, 0}},
	// 	 {{-0.5, 0, 0, 0.5}},
	// 	 {}}};

	world_tile_system::world_tile_system()
	{
		for (int i = 0; i < NUM_CHUNKS; i++)
		{
			int chunk_x = i % CHUNKS_WIDTH;
			int chunk_y = i / CHUNKS_WIDTH;
			tile_data_base[i] = new chunk(chunk_x, chunk_y);
			tile_data_copy[i] = new chunk(chunk_x, chunk_y);
			// chunk_data[i] = new std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE>{};
			set_modified_chunk(chunk_x, chunk_y, 1);
		}

		std::fill(is_solid_tile.begin(), is_solid_tile.end(), 0);
		std::fill(is_solid_tile.begin() + tile_type::GLASS, is_solid_tile.begin() + tile_type::SOLID_63 + 1, 1);

		is_solid_tile[SNOW] = 0;
		is_solid_tile[ASH] = 0;
		is_solid_tile[TEMPORARY_SNOW] = 0;
		is_solid_tile[BEDROCK] = 1;
		is_solid_tile[ELECTRIC_BLUE] = 0;

		tile_max_temperature.fill(100);
		tile_min_temperature.fill(-32765);

		tile_min_temperature[SMOKE] = -20;
		min_temp_tile_change[SMOKE] = ASH;
		
		tile_min_temperature[STEAM] = 100;
		min_temp_tile_change[STEAM] = WATER;


		// make all gas tiles use the max temp
		for (int i = 0; i < LIQUID_TILE_START_INDEX; i++)
		{
			tile_max_temperature[i] = absolute_max_temperature;
		}
		tile_max_temperature[WATER] = 100;
		tile_max_temperature[LAVA] = 3500;
		tile_max_temperature[ACID] = 250;
		tile_max_temperature[HONEY] = 500;
		// solids
		tile_max_temperature[GLASS] = 1400;
		tile_max_temperature[LEAF] = 120;
		tile_max_temperature[STONE] = 1500;
		tile_max_temperature[DIRT] = 250;
		tile_max_temperature[SAND] = 1400;
		tile_max_temperature[GRASS] = 250;
		tile_max_temperature[WOOD] = 200;
		tile_max_temperature[GOLD] = 1064;
		tile_max_temperature[TREE_SEED] = 250;
		tile_max_temperature[ROOT] = 200;
		tile_max_temperature[SNOW] = 0;

		// min temps
		tile_min_temperature[STEAM] = 100;
		tile_min_temperature[WATER] = 0;
		tile_min_temperature[LAVA] = 1500;
		tile_min_temperature[LIQUID_GLASS] = 1400;

		// max temp changes
		max_temp_tile_change[WATER] = STEAM;
		max_temp_tile_change[GLASS] = LIQUID_GLASS;

		max_temp_tile_change[LEAF] = EMBER;
		max_temp_tile_change[STONE] = LAVA;
		max_temp_tile_change[DIRT] = SAND;
		max_temp_tile_change[SAND] = LIQUID_GLASS;
		max_temp_tile_change[GRASS] = SAND;
		max_temp_tile_change[WOOD] = EMBER;
		// max_temp_tile_change[GOLD] = LIQUID_GOLD;
		max_temp_tile_change[TREE_SEED] = EMBER;
		max_temp_tile_change[ROOT] = EMBER;
		max_temp_tile_change[SNOW] = WATER;

		// min temp changes
		min_temp_tile_change[STEAM] = WATER;
		min_temp_tile_change[WATER] = ICE;
		min_temp_tile_change[LAVA] = STONE;
		min_temp_tile_change[LIQUID_GLASS] = GLASS;

		// tile heat capacity, with density baked in.
		// Joules to raise 1000kg of tile by 1 degree Celsius.
		// Tile mass is constant, so we can bake it into the heat capacity.
		tile_heat_capacity.fill(0.001f);
		
		tile_heat_capacity[AIR] = 0.00125f;
		tile_heat_capacity[STEAM] = 1.864f;
		std::fill(tile_heat_capacity.begin() + tile_type::WATER, tile_heat_capacity.begin() + tile_type::FLUID_31 + 1, 1.0f);
		tile_heat_capacity[WATER] = 4.186f;
		tile_heat_capacity[LAVA] = 10.0f;
		tile_heat_capacity[ACID] = 1.0f;
		tile_heat_capacity[HONEY] = 1.0f;
		tile_heat_capacity[LIQUID_GLASS] = 0.84f;
		std::fill(tile_heat_capacity.begin() + tile_type::GLASS, tile_heat_capacity.begin() + tile_type::SOLID_63 + 1, 1.0f);
		tile_heat_capacity[GLASS] = 0.84f;
		tile_heat_capacity[STONE] = 1.84f;
		tile_heat_capacity[DIRT] = 0.84f;
		tile_heat_capacity[SAND] = 0.84f;
		tile_heat_capacity[GRASS] = tile_heat_capacity[DIRT];
		tile_heat_capacity[WOOD] = 1.0f;
		tile_heat_capacity[GOLD] = 0.129f;
		tile_heat_capacity[TREE_SEED] = tile_heat_capacity[WOOD];
		tile_heat_capacity[ROOT] = tile_heat_capacity[WOOD];
		tile_heat_capacity[EMBER] = tile_heat_capacity[WOOD];
		tile_heat_capacity[SNOW] = 2.09f;
		tile_heat_capacity[WAX] = 3.4f;
		tile_heat_capacity[ICE] = 3.4f;
		tile_heat_capacity[ASH] = 0.25f;


		tile_heat_capacity[BEDROCK] = 1000000.0f;
	}

	world_tile_system::~world_tile_system()
	{
		chunk_mutex_base.lock();
		chunk_mutex_copy.lock();
	
		for (int i = 0; i < NUM_CHUNKS; i++)
		{
			delete tile_data_base[i];
			delete tile_data_copy[i];
		}
	}

	void world_tile_system::set_all_chunk_ent(entity ent)
	{
		all_chunk_ent = ent;
	}

	uint8_t world_tile_system::get_tile_at(int x, int y)
	{
		int chunk_x = x / CHUNK_SIZE;
		int chunk_y = y / CHUNK_SIZE;
		int chunk = chunk_x + chunk_y * CHUNKS_WIDTH;
		int tile_x = x % CHUNK_SIZE;
		int tile_y = y % CHUNK_SIZE;
		if (chunk_x < 0 || chunk_x >= CHUNKS_WIDTH || chunk_y < 0 || chunk_y >= CHUNKS_WIDTH)
			return (tile_type)0;
		if (tile_x < 0 || tile_x >= CHUNK_SIZE || tile_y < 0 || tile_y >= CHUNK_SIZE)
			return (tile_type)0;
		std::shared_lock<std::shared_mutex> lock(chunk_mutex_copy);

		return (tile_data_copy[chunk])->get_tile(tile_x, tile_y);
	}

	uint8_t world_tile_system::get_write_tile_at(int x, int y)
	{
		int chunk_x = x / CHUNK_SIZE;
		int chunk_y = y / CHUNK_SIZE;
		int chunk = chunk_x + chunk_y * CHUNKS_WIDTH;
		int tile_x = x % CHUNK_SIZE;
		int tile_y = y % CHUNK_SIZE;
		if (chunk_x < 0 || chunk_x >= CHUNKS_WIDTH || chunk_y < 0 || chunk_y >= CHUNKS_WIDTH)
			return (tile_type)0;
		if (tile_x < 0 || tile_x >= CHUNK_SIZE || tile_y < 0 || tile_y >= CHUNK_SIZE)
			return (tile_type)0;
		// std::unique_lock<std::shared_mutex> lock(write_chunk_mutex);
		// std::shared_lock<std::shared_mutex> lock(chunk_mutex_base);

		return (tile_data_base[chunk])->get_tile(tile_x, tile_y);
	}

	void world_tile_system::set_tile_at_with_lock(int x, int y, uint8_t t)
	{
		int chunk_x = x / CHUNK_SIZE;
		int chunk_y = y / CHUNK_SIZE;
		int chunk = chunk_x + chunk_y * CHUNKS_WIDTH;
		int tile_x = x % CHUNK_SIZE;
		int tile_y = y % CHUNK_SIZE;
		if (chunk_x < 0 || chunk_x >= CHUNKS_WIDTH || chunk_y < 0 || chunk_y >= CHUNKS_WIDTH)
			return;

		uint8_t current_tile = get_tile_at(x, y);

		if (current_tile == BEDROCK)
			return;

		std::unique_lock<std::shared_mutex> lock_base(chunk_mutex_base);
		std::unique_lock<std::shared_mutex> lock_copy(chunk_mutex_copy);

		if (is_solid_tile[t] != is_solid_tile[current_tile])
		{
			set_modified_chunk(chunk_x, chunk_y, 1);
		}

		(tile_data_base[chunk])->set_tile(tile_x, tile_y, t);
		(tile_data_copy[chunk])->set_tile(tile_x, tile_y, t);
	}

	void world_tile_system::set_tile_at_with_search_and_lock(int x, int y, uint8_t t)
	{
		int chunk_x = x / CHUNK_SIZE;
		int chunk_y = y / CHUNK_SIZE;
		int chunk = chunk_x + chunk_y * CHUNKS_WIDTH;
		int tile_x = x % CHUNK_SIZE;
		int tile_y = y % CHUNK_SIZE;
		if (chunk_x < 0 || chunk_x >= CHUNKS_WIDTH || chunk_y < 0 || chunk_y >= CHUNKS_WIDTH)
			return;

		std::unique_lock<std::shared_mutex> lock_base(chunk_mutex_base);
		std::unique_lock<std::shared_mutex> lock_copy(chunk_mutex_copy);

		std::unordered_set<tile_coord, tile_coord_hash> checked_tiles;
		std::queue<tile_coord> tile_queue;
		tile_queue.push(tile_coord(x, y));

		int dx[] = {0, 1, 0, -1};
		int dy[] = {-1, 0, 1, 0};

		bool found_non_solid = false;

		bool is_solid = t >= SOLID_TILE_START_INDEX;
		bool is_liquid = t >= LIQUID_TILE_START_INDEX && t < SOLID_TILE_START_INDEX;
		bool is_gas = t < LIQUID_TILE_START_INDEX;

		while (!tile_queue.empty() && checked_tiles.size() < 200)
		{
			tile_coord current_tile = tile_queue.front();
			tile_queue.pop();
			if (checked_tiles.count(current_tile))
				continue;
			if (current_tile.x < 0 || current_tile.x >= CHUNKS_WIDTH * CHUNK_SIZE || current_tile.y < 0 || current_tile.y >= CHUNKS_WIDTH * CHUNK_SIZE)
				continue;

			checked_tiles.insert(current_tile);

			uint8_t current_tile_type = get_write_tile_at(current_tile.x, current_tile.y);

			if (found_non_solid && (is_solid_tile[current_tile_type]))
			{
				continue;
			}

			if (!found_non_solid && !(is_solid_tile[current_tile_type]))
			{
				found_non_solid = true;
				tile_queue = {};
			}

			if (current_tile_type != t && ((is_solid) || (is_liquid && current_tile_type < LIQUID_TILE_START_INDEX) || (is_gas && current_tile_type < LIQUID_TILE_START_INDEX)))
			{
				lock_base.unlock();
				lock_copy.unlock();

				set_tile_at_with_lock(current_tile.x, current_tile.y, t);
				return;
			}
			else
			{
				for (int i = 0; i < 4; i++)
				{
					tile_coord new_tile = tile_coord(current_tile.x + dx[i], current_tile.y + dy[i]);

					if (!checked_tiles.count(new_tile))
					{
						tile_queue.push(new_tile);
					}
				}
			}
			// }

			// if (current_tile_type >= SOLID_TILE_START_INDEX || current_tile_type == t)
			// {
			// }
		}
	}

	void world_tile_system::set_tile_copy_at(int x, int y, uint8_t t)
	{
		int chunk_x = x / CHUNK_SIZE;
		int chunk_y = y / CHUNK_SIZE;
		int chunk = chunk_x + chunk_y * CHUNKS_WIDTH;
		int tile_x = x % CHUNK_SIZE;
		int tile_y = y % CHUNK_SIZE;
		if (chunk_x < 0 || chunk_x >= CHUNKS_WIDTH || chunk_y < 0 || chunk_y >= CHUNKS_WIDTH)
			return;
		std::unique_lock<std::shared_mutex> lock(chunk_mutex_copy);
		(tile_data_copy[chunk])->set_tile(tile_x, tile_y, t);
	}

	float world_tile_system::get_tile_temperature_at(int x, int y)
	{
		int chunk_x = x / CHUNK_SIZE;
		int chunk_y = y / CHUNK_SIZE;
		int chunk = chunk_x + chunk_y * CHUNKS_WIDTH;
		int tile_x = x % CHUNK_SIZE;
		int tile_y = y % CHUNK_SIZE;
		if (chunk_x < 0 || chunk_x >= CHUNKS_WIDTH || chunk_y < 0 || chunk_y >= CHUNKS_WIDTH)
			return 0;
		if (tile_x < 0 || tile_x >= CHUNK_SIZE || tile_y < 0 || tile_y >= CHUNK_SIZE)
			return 0;
		// std::shared_lock<std::shared_mutex> lock(chunk_mutex_base);
		return (tile_data_base[chunk])->get_tile_temperature(tile_x, tile_y);
	}

	void world_tile_system::set_tile_temperature_at(int x, int y, float temperature)
	{
		int chunk_x = x / CHUNK_SIZE;
		int chunk_y = y / CHUNK_SIZE;
		int chunk = chunk_x + chunk_y * CHUNKS_WIDTH;
		int tile_x = x % CHUNK_SIZE;
		int tile_y = y % CHUNK_SIZE;
		if (chunk_x < 0 || chunk_x >= CHUNKS_WIDTH || chunk_y < 0 || chunk_y >= CHUNKS_WIDTH)
			return;

		// if (tile_data_base[chunk]->get_tile_temperature(tile_x, tile_y) != temperature)
		// {
		// 	set_modified_chunk(chunk_x, chunk_y, 1);
		// }

		tile_data_base[chunk]->set_tile_temperature(tile_x, tile_y, temperature);
	}

	void world_tile_system::add_tile_temperature(int x, int y, float temperature)
	{
		int chunk_x = x / CHUNK_SIZE;
		int chunk_y = y / CHUNK_SIZE;
		int chunk = chunk_x + chunk_y * CHUNKS_WIDTH;
		int tile_x = x % CHUNK_SIZE;
		int tile_y = y % CHUNK_SIZE;
		if (chunk_x < 0 || chunk_x >= CHUNKS_WIDTH || chunk_y < 0 || chunk_y >= CHUNKS_WIDTH)
			return;
		
		tile_data_base[chunk]->add_tile_temperature(tile_x, tile_y, temperature);

	}

						// set_tile_at_no_lock(x, y, get_write_tile_at(x + 1, y + 1));
						// set_tile_at_no_lock(x + 1, y + 1, SAND);

	void world_tile_system::switch_tiles_no_lock(int x1, int y1, int x2, int y2)
	{
		tile_type t1 = (tile_type)get_write_tile_at(x1, y1);
		tile_type t2 = (tile_type)get_write_tile_at(x2, y2);

		if (t1 == t2 || t1 == BEDROCK || t2 == BEDROCK)
			return;
		
		// get temperatures so we can swap those too
		float t1_temp = get_tile_temperature_at(x1, y1);
		float t2_temp = get_tile_temperature_at(x2, y2);

		set_tile_at_no_lock(x1, y1, t2);
		set_tile_at_no_lock(x2, y2, t1);

		set_tile_temperature_at(x1, y1, t2_temp);
		set_tile_temperature_at(x2, y2, t1_temp);
	}

	void world_tile_system::set_tile_at_no_lock(int x, int y, uint8_t tile_type)
	{
		int chunk_x = x / CHUNK_SIZE;
		int chunk_y = y / CHUNK_SIZE;
		int chunk = chunk_x + chunk_y * CHUNKS_WIDTH;
		int tile_x = x % CHUNK_SIZE;
		int tile_y = y % CHUNK_SIZE;
		if (chunk_x < 0 || chunk_x >= CHUNKS_WIDTH || chunk_y < 0 || chunk_y >= CHUNKS_WIDTH)
			return;

		if ((tile_type >= GLASS) != (get_tile_at(x, y) >= GLASS))
		{
			set_modified_chunk(chunk_x, chunk_y, 1);
		}

		// std::shared_lock<std::shared_mutex> lock(chunk_mutex);
		(tile_data_base[chunk])->set_tile(tile_x, tile_y, tile_type);
	}

	std::array<entity, NUM_CHUNKS> world_tile_system::get_chunk_entities()
	{
		return chunk_entities;
	}

	enum tile_simple_type
	{
		GAS,
		LIQUID,
		SOLID,
		BACKGROUND_TILE
	};

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

	void world_tile_system::update(uint64_t tick_count)
	{

		std::unique_lock<std::shared_mutex> lock_base(chunk_mutex_base);

		uint8_t direction = tick_count % 2;
		
		int heat_dx[] = {0, -1};
		int heat_dy[] = {-1, 0};
		// int heat_dx[] = {0, 1, 0, -1};
		// int heat_dy[] = {-1, 0, 1, 0};
				
		
		// std::array<std::array<std::array<int16_t, CHUNK_SIZE>, CHUNK_SIZE>, NUM_CHUNKS> tile_temperature_changes;
		// for (int i = 0; i < NUM_CHUNKS; i++)
		// {
		// 	tile_temperature_changes[i].fill({});
		// }

		for (int y = 1; y < CHUNKS_WIDTH * CHUNK_SIZE - 1; y++)
		{
			int chunk_y = y / CHUNK_SIZE;

			for (int x = 1; x < CHUNKS_WIDTH * CHUNK_SIZE - 1; x++)
			// for (int x = direction ? 0 : CHUNKS_WIDTH * CHUNK_SIZE - 1; x < CHUNKS_WIDTH * CHUNK_SIZE && x >= 0; x += direction ? 1 : -1)
			{
				int chunk_x = x / CHUNK_SIZE;
				int chunk = chunk_x + chunk_y * CHUNKS_WIDTH;
				int tile_x = x % CHUNK_SIZE;
				int tile_y = y % CHUNK_SIZE;

				uint8_t tile_t = get_write_tile_at(x, y);
				float tile_temp = get_tile_temperature_at(x, y);

				// check if tile is overheated
				if (tile_temp > tile_max_temperature[tile_t] + 2)
				{
					if(max_temp_tile_change.count((tile_type)tile_t))
					{
						set_tile_at_no_lock(x, y, max_temp_tile_change[(tile_type)tile_t]);
					} 
					else 
					{
						// printf(" overheated with temperaTile %d at (%d, %d) is overheated with temperature %d, but no max temp change defined.\n", tile_t, x, y, tile_temp);
					}
				}
				// check if tile is too cold
				else if (tile_temp < tile_min_temperature[tile_t] - 2)
				{
					if(min_temp_tile_change.count((tile_type)tile_t))
					{
						set_tile_at_no_lock(x, y, min_temp_tile_change[(tile_type)tile_t]);
					} 
					else 
					{
						// printf(" too cold with temperaTile %d at (%d, %d) is too cold with temperature %d, but no min temp change defined.\n", tile_t, x, y, tile_temp);
					}
				}

				// transfer heat to adjacent tiles
				// TODO: make the direction random order
				for(int i = 0; i < 2; i++)
				{
					int adjacent_x = x + heat_dx[i];
					int adjacent_y = y + heat_dy[i];

					if(adjacent_x < 0 || adjacent_x >= CHUNKS_WIDTH * CHUNK_SIZE || adjacent_y < 0 || adjacent_y >= CHUNKS_WIDTH * CHUNK_SIZE)
					{
						continue;
					}

					uint8_t adjacent_tile_t = get_write_tile_at(adjacent_x, adjacent_y);

					float adjacent_tile_temp = get_tile_temperature_at(adjacent_x, adjacent_y);
					if (abs(adjacent_tile_temp - tile_temp) > .25)
					{
						// TODO: Create tile temperature storage units + tile temperature transfer rates
						float temp_difference = (adjacent_tile_temp - tile_temp) / 50;
						
						float tile_new_temp = tile_temp + temp_difference / tile_heat_capacity[tile_t];
						float adjacent_tile_new_temp = adjacent_tile_temp - temp_difference / tile_heat_capacity[adjacent_tile_t];

						if(adjacent_tile_temp > 200 && adjacent_tile_t == LAVA && tile_t == SAND)
						{
							tile_t = tile_t;
						}

						if (tile_new_temp > tile_temp && tile_new_temp > adjacent_tile_temp // overheated tile
							|| tile_new_temp < tile_temp && tile_new_temp < adjacent_tile_temp // overcooled tile
							|| adjacent_tile_new_temp > adjacent_tile_temp && adjacent_tile_new_temp > tile_temp // overheated adjacent tile
							|| adjacent_tile_new_temp < adjacent_tile_temp && adjacent_tile_new_temp < tile_temp // overcooled adjacent tile
						)
						{
							temp_difference = (adjacent_tile_temp - tile_temp) / (1.0f / tile_heat_capacity[tile_t] + 1.0f / tile_heat_capacity[adjacent_tile_t]);
							tile_new_temp = tile_temp + temp_difference / tile_heat_capacity[tile_t];
							adjacent_tile_new_temp = adjacent_tile_temp - temp_difference / tile_heat_capacity[adjacent_tile_t];
						}
						
						// {
						// 	temp_difference = (adjacent_tile_temp - tile_temp) / (1.0f / tile_heat_capacity[tile_t] + 1.0f / tile_heat_capacity[adjacent_tile_t]);
						// 	tile_new_temp = tile_temp + temp_difference / tile_heat_capacity[tile_t];
						// 	adjacent_tile_new_temp = adjacent_tile_temp - temp_difference / tile_heat_capacity[adjacent_tile_t];
						// }
						// else if (adjacent_tile_new_temp < tile_temp) // overshoot on destination
						// {
						// 	temp_difference = (tile_temp - adjacent_tile_temp) / (1.0f / tile_heat_capacity[tile_t] + 1.0f / tile_heat_capacity[adjacent_tile_t]);
						// 	tile_new_temp = tile_temp + temp_difference / tile_heat_capacity[tile_t];
						// 	adjacent_tile_new_temp = adjacent_tile_temp - temp_difference / tile_heat_capacity[adjacent_tile_t];
						// }
						set_tile_temperature_at(x, y, tile_new_temp);
						set_tile_temperature_at(adjacent_x, adjacent_y, adjacent_tile_new_temp);

						// add_tile_temperature(x, y, temp_difference / tile_heat_capacity[tile_t]);
						// add_tile_temperature(adjacent_x, adjacent_y, -temp_difference / tile_heat_capacity[adjacent_tile_t]);

						tile_temp = tile_new_temp;
						// tile_temp = get_tile_temperature_at(x, y);

						// float delta_tile_temp = (effective_conductivity / 20.0) / tile_heat_capacity[tile_t];
						// float adjacent_delta_tile_temp = -(effective_conductivity / 20.0) / tile_heat_capacity[adjacent_tile_t];

						// if (delta_tile_temp > 0)
						// {
							// set_tile_temperature_at(adjacent_x, adjacent_y, adjacent_tile_temp + temp_change);
							// set_tile_temperature_at(x, y, tile_temp - temp_change);

							// tile_temperature_changes[chunk][adjacent_y % CHUNK_SIZE][adjacent_x % CHUNK_SIZE] += adjacent_delta_tile_temp;
							// tile_temperature_changes[chunk][tile_y % CHUNK_SIZE][tile_x % CHUNK_SIZE] -= delta_tile_temp;
						// }



					}
				}
			}
		}
		// apply temperature changes
		// for (int i = 0; i < NUM_CHUNKS; i++)
		// {
		// 	int chunk_x = i % CHUNKS_WIDTH;
		// 	int chunk_y = i / CHUNKS_WIDTH;

		// 	// std::unique_lock<std::shared_mutex> lock_copy(chunk_mutex_copy);
		// 	// std::unique_lock<std::shared_mutex> lock_base(chunk_mutex_base);

		// 	for (int y = 0; y < CHUNK_SIZE; y++)
		// 	{
		// 		for (int x = 0; x < CHUNK_SIZE; x++)
		// 		{
		// 			int16_t temp_change = tile_temperature_changes[i][y][x];
		// 			if (temp_change != 0)
		// 			{
		// 				int tile_x = x + chunk_x * CHUNK_SIZE;
		// 				int tile_y = y + chunk_y * CHUNK_SIZE;
		// 				set_tile_temperature_at(tile_x, tile_y, get_tile_temperature_at(tile_x, tile_y) + temp_change);
		// 			}
		// 		}
		// 	}
		// }

		for (int y = CHUNKS_WIDTH * CHUNK_SIZE - 1; y >= 0; y--)
		{
			int chunk_y = y / CHUNK_SIZE;

			for (int x = direction ? 0 : CHUNKS_WIDTH * CHUNK_SIZE - 1; x < CHUNKS_WIDTH * CHUNK_SIZE && x >= 0; x += direction ? 1 : -1)
			{
				int chunk_x = x / CHUNK_SIZE;
				int chunk = chunk_x + chunk_y * CHUNKS_WIDTH;
				int tile_x = x % CHUNK_SIZE;
				int tile_y = y % CHUNK_SIZE;

				uint8_t tile_type = get_write_tile_at(x, y);

				switch (tile_type)
				{
				case AIR:
					if(((tick_count / 64) % 6 == 5 || (tick_count / 50) % 8 == 7) && rand() % 1200 == 0)
					{
						set_tile_at_no_lock(x, y, SNOW);
						set_tile_temperature_at(x, y, -20.f);
						// set_tile_at_no_lock(x, y, TEMPORARY_SMOKE);
					}
					break;

				// case WATER:
				// 	if (game_engine::in_set(get_write_tile_at(x, y + 1), AIR, SMOKE, TEMPORARY_SMOKE))
				// 	{
				// 		// set_tile_at_no_lock(x, y, get_write_tile_at(x, y + 1));
				// 		// set_tile_at_no_lock(x, y + 1, WATER);
				// 		switch_tiles_no_lock(x, y, x, y + 1);
				// 		break;
				// 	}
				// 	if (game_engine::in_set(get_write_tile_at(x - 1, y + 1), AIR, SMOKE, TEMPORARY_SMOKE))
				// 	{
				// 		// set_tile_at_no_lock(x, y, get_write_tile_at(x - 1, y + 1));
				// 		// set_tile_at_no_lock(x - 1, y + 1, WATER);
				// 		switch_tiles_no_lock(x, y, x - 1, y + 1);
				// 		break;
				// 	}
				// 	if (game_engine::in_set(get_write_tile_at(x + 1, y + 1), AIR, SMOKE, TEMPORARY_SMOKE))
				// 	{
				// 		// set_tile_at_no_lock(x, y, get_write_tile_at(x + 1, y + 1));
				// 		// set_tile_at_no_lock(x + 1, y + 1, WATER);
				// 		switch_tiles_no_lock(x, y, x + 1, y + 1);
				// 		break;
				// 	}
				// 	if (!(rand() % 4))
				// 	{
				// 		if (get_simple_tile_type(get_write_tile_at(x + 1, y)) == GAS && get_simple_tile_type(get_write_tile_at(x - 1, y)) == GAS)
				// 		{
				// 			if (rand() % 3)
				// 			{
				// 				// set_tile_at_no_lock(x, y, get_write_tile_at(x - 1, y));
				// 				// set_tile_at_no_lock(x - 1, y, WATER);
				// 				switch_tiles_no_lock(x, y, x - 1, y);
				// 			}
				// 			else
				// 			{
				// 				// set_tile_at_no_lock(x, y, get_write_tile_at(x + 1, y));
				// 				// set_tile_at_no_lock(x + 1, y, WATER);
				// 				switch_tiles_no_lock(x, y, x + 1, y);
				// 			}
				// 		}
				// 		else if (get_simple_tile_type(get_write_tile_at(x + 1, y)) == GAS)
				// 		{
				// 			// set_tile_at_no_lock(x, y, get_write_tile_at(x + 1, y));
				// 			// set_tile_at_no_lock(x + 1, y, WATER);
				// 			switch_tiles_no_lock(x, y, x + 1, y);
				// 			break;
				// 		}
				// 		else if (get_simple_tile_type(get_write_tile_at(x - 1, y)) == GAS)
				// 		{
				// 			// set_tile_at_no_lock(x, y, get_write_tile_at(x - 1, y));
				// 			// set_tile_at_no_lock(x - 1, y, WATER);
				// 			switch_tiles_no_lock(x, y, x - 1, y);
				// 			break;
				// 		}
				// 	}
				//	 break;

					// if (game_engine::in_set(get_write_tile_at(x - 1, y), AIR, SMOKE, TEMPORARY_SMOKE))
					// {
					// 	set_tile_at_no_lock(x, y, get_write_tile_at(x - 1, y));
					// 	set_tile_at_no_lock(x - 1, y, WATER);
					// 	break;
					// }
					// if (game_engine::in_set(get_write_tile_at(x + 1, y), AIR, SMOKE, TEMPORARY_SMOKE))
					// {
					// 	set_tile_at_no_lock(x, y, get_write_tile_at(x + 1, y));
					// 	set_tile_at_no_lock(x + 1, y, WATER);
					// 	break;
					// }
				case ACID:
					// if (rand() % 10 == 0 && get_write_tile_at(x, y + 1) >= GLASS && get_write_tile_at(x, y + 1) < BEDROCK)
					// {
					// 	set_tile_at_no_lock(x, y + 1, TEMPORARY_SMOKE);
					// 	set_tile_at_no_lock(x, y, AIR);
					// 	break;
					// }

					if (game_engine::in_set(get_write_tile_at(x, y + 1), AIR, SMOKE, TEMPORARY_SMOKE))
					{
						// set_tile_at_no_lock(x, y, get_write_tile_at(x, y + 1));
						// set_tile_at_no_lock(x, y + 1, ACID);
						switch_tiles_no_lock(x, y, x, y + 1);
						break;
					}
					if (game_engine::in_set(get_write_tile_at(x - 1, y + 1), AIR, SMOKE, TEMPORARY_SMOKE))
					{
						// set_tile_at_no_lock(x, y, get_write_tile_at(x - 1, y + 1));
						// set_tile_at_no_lock(x - 1, y + 1, ACID);
						switch_tiles_no_lock(x, y, x - 1, y + 1);
						break;
					}
					if (game_engine::in_set(get_write_tile_at(x + 1, y + 1), AIR, SMOKE, TEMPORARY_SMOKE))
					{
						// set_tile_at_no_lock(x, y, get_write_tile_at(x + 1, y + 1));
						// set_tile_at_no_lock(x + 1, y + 1, ACID);
						switch_tiles_no_lock(x, y, x + 1, y + 1);
						break;
					}
					if (game_engine::in_set(get_write_tile_at(x - 1, y), AIR, SMOKE, TEMPORARY_SMOKE))
					{
						// set_tile_at_no_lock(x, y, get_write_tile_at(x - 1, y));
						// set_tile_at_no_lock(x - 1, y, ACID);
						switch_tiles_no_lock(x, y, x - 1, y);
						break;
					}
					if (game_engine::in_set(get_write_tile_at(x + 1, y), AIR, SMOKE, TEMPORARY_SMOKE))
					{
						// set_tile_at_no_lock(x, y, get_write_tile_at(x + 1, y));
						// set_tile_at_no_lock(x + 1, y, ACID);
						switch_tiles_no_lock(x, y, x + 1, y);
						break;
					}
					if (rand() % 3 == 0 && get_write_tile_at(x, y + 1) >= GLASS && get_write_tile_at(x, y + 1) < BEDROCK)
					{
						set_tile_at_no_lock(x, y + 1, TEMPORARY_SMOKE);
						set_tile_at_no_lock(x, y, AIR);
						break;
					}
					if (rand() % 12 == 0 && get_write_tile_at(x - 1, y) >= GLASS && get_write_tile_at(x - 1, y) < BEDROCK)
					{
						set_tile_at_no_lock(x - 1, y, TEMPORARY_SMOKE);
						set_tile_at_no_lock(x, y, AIR);
						break;
					}
					if (rand() % 12 == 0 && get_write_tile_at(x + 1, y) >= GLASS && get_write_tile_at(x + 1, y) < BEDROCK)
					{
						set_tile_at_no_lock(x + 1, y, TEMPORARY_SMOKE);
						set_tile_at_no_lock(x, y, AIR);
						break;
					}
					break;
				case GRASS:
					if (rand() % 10 == 0)
					{
						int dx[] = {0, 1, 1, 1, 0, -1, -1, -1};
						int dy[] = {-1, -1, 0, 1, 1, 1, 0, -1};
						int r = rand() % 8;
						for (int i = 0; i < 8; i++)
						{
							int xx = x + dx[(r + i) % 8];
							int yy = y + dy[(r + i) % 8];
							if (get_write_tile_at(xx, yy) == DIRT && get_write_tile_at(xx, yy - 1) == AIR)
							{
								set_tile_at_no_lock(xx, yy, GRASS);
								break;
							}
						}
					}
					// if (get_write_tile_at(x, y - 1) == AIR && rand() % 100 == 0)
					// {
					// 	set_tile_at(x, y, DIRT);
					// }
					break;

				case SAND:
					if (get_write_tile_at(x, y + 1) < SOLID_TILE_START_INDEX)
					{
						// set_tile_at_no_lock(x, y, get_write_tile_at(x, y + 1));
						// set_tile_at_no_lock(x, y + 1, SAND);
						switch_tiles_no_lock(x, y, x, y + 1);
					}
					else if (get_write_tile_at(x - 1, y + 1) < SOLID_TILE_START_INDEX)
					{
						// set_tile_at_no_lock(x, y, get_write_tile_at(x - 1, y + 1));
						// set_tile_at_no_lock(x - 1, y + 1, SAND);
						switch_tiles_no_lock(x, y, x - 1, y + 1);
					}
					else if (get_write_tile_at(x + 1, y + 1) < SOLID_TILE_START_INDEX)
					{
						// set_tile_at_no_lock(x, y, get_write_tile_at(x + 1, y + 1));
						// set_tile_at_no_lock(x + 1, y + 1, SAND);
						switch_tiles_no_lock(x, y, x + 1, y + 1);
					}
					break;

				case EMBER:

					if (get_write_tile_at(x, y - 1) == AIR)
					{
						set_tile_at_no_lock(x, y - 1, TEMPORARY_SMOKE);
					}
					else if (get_write_tile_at(x, y + 1) == AIR)
					{
						set_tile_at_no_lock(x, y + 1, TEMPORARY_SMOKE);
					}
					else if (get_write_tile_at(x - 1, y) == AIR)
					{
						set_tile_at_no_lock(x - 1, y, TEMPORARY_SMOKE);
					}
					else if (get_write_tile_at(x + 1, y) == AIR)
					{
						set_tile_at_no_lock(x + 1, y, TEMPORARY_SMOKE);
					}

					if (rand() % 25 == 0)
					{
						set_tile_at_no_lock(x, y, TEMPORARY_SMOKE);
					}
					else
					{
						if (rand() % 2 == 0) // try spread
						{
							if (game_engine::in_set(get_write_tile_at(x, y - 1), WOOD, ROOT, LEAF))
							{
								set_tile_at_no_lock(x, y - 1, EMBER);
							}
							else if (game_engine::in_set(get_write_tile_at(x, y + 1), WOOD, ROOT, LEAF))
							{
								set_tile_at_no_lock(x, y + 1, EMBER);
							}
							else if (game_engine::in_set(get_write_tile_at(x - 1, y), WOOD, ROOT, LEAF))
							{
								set_tile_at_no_lock(x - 1, y, EMBER);
							}
							else if (game_engine::in_set(get_write_tile_at(x + 1, y), WOOD, ROOT, LEAF))
							{
								set_tile_at_no_lock(x + 1, y, EMBER);
							}
						}
					}
					break;

				case SNOW:
					if (rand() % 2 == 0 && get_simple_tile_type(get_write_tile_at(x, y + 1)) == GAS)
					{
						// set_tile_at_no_lock(x, y, get_write_tile_at(x, y + 1));
						// set_tile_at_no_lock(x, y + 1, SNOW);
						switch_tiles_no_lock(x, y, x, y + 1);
					}
					break;
				
				case TEMPORARY_SNOW:
				
					if (game_engine::in_set(get_write_tile_at(x, y + 1), AIR, SMOKE, WATER, TEMPORARY_SMOKE))
					{
						// set_tile_at_no_lock(x, y, get_write_tile_at(x, y + 1));
						// set_tile_at_no_lock(x, y + 1, TEMPORARY_SNOW);
						switch_tiles_no_lock(x, y, x, y + 1);
					}
					else if (rand() % 8 == 0)
					{
						set_tile_at_no_lock(x, y, SNOW);
						set_tile_temperature_at(x, y, -100);
					}
					break;

				default:
					game::tile_simple_type simple_type = get_simple_tile_type(tile_type);

					switch (simple_type)
					{
					case LIQUID:
						if (get_simple_tile_type(get_write_tile_at(x, y + 1)) == GAS)
						{
							// set_tile_at_no_lock(x, y, get_write_tile_at(x, y + 1));
							// set_tile_at_no_lock(x, y + 1, tile_type);
							switch_tiles_no_lock(x, y, x, y + 1);
						}
						else if (get_simple_tile_type(get_write_tile_at(x - 1, y + 1)) == GAS)
						{
							// set_tile_at_no_lock(x, y, get_write_tile_at(x - 1, y + 1));
							// set_tile_at_no_lock(x - 1, y + 1, tile_type);
							switch_tiles_no_lock(x, y, x - 1, y + 1);
						}
						else if (get_simple_tile_type(get_write_tile_at(x + 1, y + 1)) == GAS)
						{
							// set_tile_at_no_lock(x, y, get_write_tile_at(x + 1, y + 1));
							// set_tile_at_no_lock(x + 1, y + 1, tile_type);
							switch_tiles_no_lock(x, y, x + 1, y + 1);
						}
						else if (get_simple_tile_type(get_write_tile_at(x, y + 1)) == LIQUID)
						{
							if (get_write_tile_at(x, y + 1) < tile_type)
							{
								switch_tiles_no_lock(x, y, x, y + 1);
							}
						}
						break;
					case GAS:
						if (get_simple_tile_type(get_write_tile_at(x, y - 1)) == GAS && get_write_tile_at(x, y - 1) < tile_type)
						{
							switch_tiles_no_lock(x, y, x, y - 1);
						}
						else if (get_simple_tile_type(get_write_tile_at(x + 1, y)) == GAS && get_write_tile_at(x + 1, y) < tile_type)
						{
							switch_tiles_no_lock(x, y, x + 1, y);
						}
						break;
					}
					break;
				}
			}
		}

		for (int y = 0; y < CHUNKS_WIDTH * CHUNK_SIZE; y++)
		{
			for (int x = direction ? 0 : CHUNKS_WIDTH * CHUNK_SIZE - 1; x < CHUNKS_WIDTH * CHUNK_SIZE && x >= 0; x += direction ? 1 : -1)
			{
				int chunk_x = x / CHUNK_SIZE;
				int chunk_y = y / CHUNK_SIZE;
				int chunk = chunk_x + chunk_y * CHUNKS_WIDTH;
				int tile_x = x % CHUNK_SIZE;
				int tile_y = y % CHUNK_SIZE;

				uint8_t tile_type = get_write_tile_at(x, y);
				switch (tile_type)
				{
				case SMOKE:
					if (game_engine::in_set(get_write_tile_at(x, y - 1), AIR, WATER))
					{
						set_tile_at_no_lock(x, y, get_write_tile_at(x, y - 1));
						set_tile_at_no_lock(x, y - 1, SMOKE);
					}
					else if (game_engine::in_set(get_write_tile_at(x - 1, y - 1), AIR, WATER))
					{
						set_tile_at_no_lock(x, y, get_write_tile_at(x - 1, y - 1));
						set_tile_at_no_lock(x - 1, y - 1, SMOKE);
					}
					else if (game_engine::in_set(get_write_tile_at(x + 1, y - 1), AIR, WATER))
					{
						set_tile_at_no_lock(x, y, get_write_tile_at(x + 1, y - 1));
						set_tile_at_no_lock(x + 1, y - 1, SMOKE);
					}
					else if (game_engine::in_set(get_write_tile_at(x - 1, y), AIR, WATER))
					{
						set_tile_at_no_lock(x, y, get_write_tile_at(x - 1, y));
						set_tile_at_no_lock(x - 1, y, SMOKE);
					}
					else if (game_engine::in_set(get_write_tile_at(x + 1, y), AIR, WATER))
					{
						set_tile_at_no_lock(x, y, get_write_tile_at(x + 1, y));
						set_tile_at_no_lock(x + 1, y, SMOKE);
					}
					break;
				case TEMPORARY_SMOKE:
					if (rand() % 15 == 0)
					{
						set_tile_at_no_lock(x, y, AIR);
					}
					else if (game_engine::in_set(get_write_tile_at(x, y - 1), AIR, WATER))
					{
						set_tile_at_no_lock(x, y, get_write_tile_at(x, y - 1));
						set_tile_at_no_lock(x, y - 1, TEMPORARY_SMOKE);
					}
					else if (game_engine::in_set(get_write_tile_at(x - 1, y - 1), AIR, WATER))
					{
						set_tile_at_no_lock(x, y, get_write_tile_at(x - 1, y - 1));
						set_tile_at_no_lock(x - 1, y - 1, TEMPORARY_SMOKE);
					}
					else if (game_engine::in_set(get_write_tile_at(x + 1, y - 1), AIR, WATER))
					{
						set_tile_at_no_lock(x, y, get_write_tile_at(x + 1, y - 1));
						set_tile_at_no_lock(x + 1, y - 1, TEMPORARY_SMOKE);
					}
					else if (game_engine::in_set(get_write_tile_at(x - 1, y), AIR, WATER))
					{
						set_tile_at_no_lock(x, y, get_write_tile_at(x - 1, y));
						set_tile_at_no_lock(x - 1, y, TEMPORARY_SMOKE);
					}
					else if (game_engine::in_set(get_write_tile_at(x + 1, y), AIR, WATER))
					{
						set_tile_at_no_lock(x, y, get_write_tile_at(x + 1, y));
						set_tile_at_no_lock(x + 1, y, TEMPORARY_SMOKE);
					}
					break;
				}
			}
		}

		// get character position
		entity character_ent = game_engine::game_engine_pointer->player_entitiy;
		game_engine::box_system *box_system_pointer = ((game_engine::box_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::box_system>()));
		game_engine::box character_box = box_system_pointer->get(character_ent).get_box();
		// get character velocity from box2d
		box2d_system *box2d_system_pointer = ((box2d_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<box2d_system>()));
		b2Body *character_body = box2d_system_pointer->get_dynamic_body(character_ent);

		b2Vec2 velocity = character_body->GetLinearVelocity();

		for (int i = (int)character_box.x; i < (int)character_box.x + character_box.w + 1; i++)
		{
			for (int j = (int)character_box.y; j < (int)character_box.y + character_box.h + 1; j++)
			{
				tile_type tile = (tile_type)get_write_tile_at(i, j);
				float tile_temp = get_tile_temperature_at(i, j);

				if (tile >= SOLID_TILE_START_INDEX && !is_solid_tile[tile])
				{
					set_tile_at_no_lock(i, j, AIR);
					game_engine::task_scheduler_pointer->add_task({&create_single_debris_task, new create_debris_params(character_box.x + ((velocity.x > 0) ? character_box.w + 2 : -2), j - 1, velocity.x, -velocity.y, 0.5f, tile, AIR, tile, 250, tile_temp)});
					// game_engine::task_scheduler_pointer->add_task({&create_single_debris_task, new create_debris_params(character_box.x + ((velocity.x > 0) ? character_box.w + 2 : -2), j, (velocity.x > 0) ? 50 : -50, -10, 0.5f, SNOW, TEMPORARY_SMOKE)});
				}
				else if (velocity.y * velocity.y + velocity.x * velocity.x > 3 && tile >= LIQUID_TILE_START_INDEX && tile < SOLID_TILE_START_INDEX)
				{
					set_tile_at_no_lock(i, j, AIR);
					game_engine::task_scheduler_pointer->add_task({&create_single_debris_task, new create_debris_params(character_box.x + ((velocity.x > 0) ? character_box.w + 2 : -2), j - 1, velocity.x, -velocity.y, 0.5f, tile, AIR, tile, 250, tile_temp)});
					// game_engine::task_scheduler_pointer->add_task({&create_single_debris_task, new create_debris_params(character_box.x + ((velocity.x > 0) ? character_box.w + 2 : -2), j, (velocity.x > 0) ? 50 : -50, -10, 0.5f, SNOW, TEMPORARY_SMOKE)});
				}
			}
		}
		// do the same as ^, but for the bodies in the legged creature system
		legged_creature_system *legged_creature_system_pointer = ((legged_creature_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<legged_creature_system>()));
		std::vector<entity> legged_creatures = legged_creature_system_pointer->get_entities();

		// for (entity legged_creature : legged_creatures)
		// {
		// 	game_engine::box legged_creature_box = box_system_pointer->get(legged_creature).get_box();
		// 	b2Body *legged_creature_body = box2d_system_pointer->get_dynamic_body(legged_creature);

		// 	b2Vec2 legged_creature_velocity = legged_creature_body->GetLinearVelocity();

		// 	for (int i = (int)legged_creature_box.x; i < (int)legged_creature_box.x + legged_creature_box.w + 1; i++)
		// 	{
		// 		for (int j = (int)legged_creature_box.y; j < (int)legged_creature_box.y + legged_creature_box.h + 1; j++)
		// 		{
		// 			tile_type tile = (tile_type)get_write_tile_at(i, j);

		// 			if (tile >= SOLID_TILE_START_INDEX && !is_solid_tile[tile])
		// 			{
		// 				set_tile_at_no_lock(i, j, AIR);
		// 				game_engine::task_scheduler_pointer->add_task({&create_single_debris_task, new create_debris_params(legged_creature_box.x + ((legged_creature_velocity.x > 0) ? legged_creature_box.w + 2 : -2), j - 1, legged_creature_velocity.x, -legged_creature_velocity.y, 0.5f, tile, AIR, tile, 250)});
		// 				// game_engine::task_scheduler_pointer->add_task({&create_single_debris_task, new create_debris_params(character_box.x + ((velocity.x > 0) ? character_box.w + 2 : -2), j, (velocity.x > 0) ? 50 : -50, -10, 0.5f, SNOW, TEMPORARY_SMOKE)});
		// 			}
		// 			else if (legged_creature_velocity.y * legged_creature_velocity.y + legged_creature_velocity.x * legged_creature_velocity.x > 3 && tile >= LIQUID_TILE_START_INDEX && tile < SOLID_TILE_START_INDEX)
		// 			{
		// 				set_tile_at_no_lock(i, j, AIR);
		// 				game_engine::task_scheduler_pointer->add_task({&create_single_debris_task, new create_debris_params(legged_creature_box.x + ((legged_creature_velocity.x > 0) ? legged_creature_box.w + 2 : -2), j - 1, legged_creature_velocity.x, -legged_creature_velocity.y, 0.5f, tile, AIR, tile, 250)});
		// 				// game_engine::task_scheduler_pointer->add_task({&create_single_debris_task, new create_debris_params(character_box.x + ((velocity.x > 0) ? character_box.w + 2 : -2), j, (velocity.x > 0) ? 50 : -50, -10, 0.5f, SNOW, TEMPORARY_SMOKE)});
		// 			}
		// 		}
		// 	}
		// }
		// that, but the legged creatures body is a circle instead of a box. so create projectiles with the right velocity and position
		for (entity legged_creature : legged_creatures)
		{
			game_engine::box legged_creature_box = box_system_pointer->get(legged_creature).get_box();
			b2Body *legged_creature_body = box2d_system_pointer->get_dynamic_body(legged_creature);

			b2Vec2 legged_creature_velocity = legged_creature_body->GetLinearVelocity();

			b2Vec2 legged_creature_position = legged_creature_body->GetPosition();

			for (int i = (int)legged_creature_box.x; i < (int)legged_creature_box.x + legged_creature_box.w + 1; i++)
			{
				for (int j = (int)legged_creature_box.y; j < (int)legged_creature_box.y + legged_creature_box.h + 1; j++)
				{
					tile_type tile = (tile_type)get_write_tile_at(i, j);
					float temperature = get_tile_temperature_at(i, j);

					// Calculate the collision point on the circle
					b2Vec2 collision_point = b2Vec2(i, j);
					
					// Calculate the normal of the collision point
					b2Vec2 normal = collision_point - legged_creature_position;
					normal.Normalize();

					// Calculate the velocity of the collision point
					b2Vec2 collision_velocity = legged_creature_velocity;// + b2Cross(legged_creature_body->GetAngularVelocity(), collision_point - legged_creature_position);

					
					// Calculate the relative velocity in terms of the normal direction
					float normal_velocity = b2Dot(collision_velocity, normal);

					// Calculate the relative velocity in terms of the tangent direction
					b2Vec2 tangent = b2Vec2(-normal.y, normal.x);
					float tangent_velocity = b2Dot(collision_velocity, tangent);	

					// debris_velocity
					b2Vec2 debris_velocity = normal_velocity * normal + tangent_velocity * tangent;



					if (tile >= SOLID_TILE_START_INDEX && !is_solid_tile[tile])
					{
						set_tile_at_no_lock(i, j, AIR);
						game_engine::task_scheduler_pointer->add_task({&create_single_debris_task, new create_debris_params(legged_creature_box.x + ((legged_creature_velocity.x > 0) ? legged_creature_box.w + 2 : -2), j - 1, debris_velocity.x, debris_velocity.y, 0.5f, tile, AIR, tile, 250, temperature)});
					}
					else if (legged_creature_velocity.y * legged_creature_velocity.y + legged_creature_velocity.x * legged_creature_velocity.x > 3 && tile >= LIQUID_TILE_START_INDEX && tile < SOLID_TILE_START_INDEX)
					{
						set_tile_at_no_lock(i, j, AIR);
						game_engine::task_scheduler_pointer->add_task({&create_single_debris_task, new create_debris_params(legged_creature_box.x + ((legged_creature_velocity.x > 0) ? legged_creature_box.w + 2 : -2), j - 1, debris_velocity.x, debris_velocity.y, 0.5f, tile, AIR, tile, 250, temperature)});
					}


				}
			}
		}



		std::unique_lock<std::shared_mutex> lock_copy(chunk_mutex_copy);

		// copy modified chunks
		for (int i = 0; i < NUM_CHUNKS; i++)
		{
			memcpy(tile_data_copy[i]->get_data(), tile_data_base[i]->get_data(), CHUNK_SIZE * CHUNK_SIZE);
			memcpy(tile_data_copy[i]->get_temperature_data(), tile_data_base[i]->get_temperature_data(), CHUNK_SIZE * CHUNK_SIZE * sizeof(float));
		}

		

		lock_copy.unlock();
		lock_base.unlock();

		std::shared_lock<std::shared_mutex> tile_lock(chunk_mutex_copy);
		// update textures
		for (int chunk = 0; chunk < NUM_CHUNKS; chunk++)
		{
			entity chunk_ent = chunk_entities[chunk];
			int chunk_x = chunk % CHUNKS_WIDTH;
			int chunk_y = chunk / CHUNKS_WIDTH;
			game_engine::render_system *render_system_pointer = ((game_engine::render_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::render_system>()));
			render_system_pointer->update_texture_section(all_chunk_ent, (uint8_t *)(tile_data_copy[chunk]->get_data()), chunk_x * CHUNK_SIZE, chunk_y * CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
		}
	}

	void world_tile_system::generate_world()
	{
		for (int chunk = 0; chunk < NUM_CHUNKS; chunk++)
		{
			tile_data_base[chunk]->create_chunk(chunk % CHUNKS_WIDTH, chunk / CHUNKS_WIDTH);
			chunk_entities[chunk] = game_engine::game_engine_pointer->create_entity();
		}

		// // create solid border around world
		// for (int x = 0; x < CHUNKS_WIDTH * CHUNK_SIZE; x++)
		// {
		// 	set_tile_at_no_lock(x, 0, BEDROCK);
		// 	set_tile_at_no_lock(x, CHUNKS_WIDTH * CHUNK_SIZE - 1, BEDROCK);
		// }
		// for (int y = 0; y < CHUNKS_WIDTH * CHUNK_SIZE; y++)
		// {
		// 	set_tile_at_no_lock(0, y, BEDROCK);
		// 	set_tile_at_no_lock(CHUNKS_WIDTH * CHUNK_SIZE - 1, y, BEDROCK);
		// }
	}

	entity world_tile_system::get_chunk_entity(int x, int y)
	{
		return chunk_entities[x + y * CHUNKS_WIDTH];
	}

	entity world_tile_system::get_chunk_entity(int chunk)
	{
		return chunk_entities[chunk];
	}

	std::array<chunk *, NUM_CHUNKS> *world_tile_system::get_chunks_copy()
	{
		return &tile_data_copy;
	}

	std::array<chunk *, NUM_CHUNKS> *world_tile_system::get_chunks_base()
	{
		return &tile_data_base;
	}

	// std::array<std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE> *, NUM_CHUNKS> world_tile_system::get_chunks_data()
	// {
	// 	std::array<std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE> *, NUM_CHUNKS> chunks_data;
	// 	for (int chunk = 0; chunk < NUM_CHUNKS; chunk++)
	// 	{
	// 		// chunk_data[chunk]->create_texture_from_chunk(textures[chunk]);
	// 		chunks_data[chunk] = chunk_data[chunk]->get_data();
	// 	}
	// 	return chunks_data;
	// }

	std::vector<std::vector<std::pair<float, float>>> *world_tile_system::create_outlines(int x, int y)
	{
		std::vector<std::vector<std::pair<float, float>>> *outlines = new std::vector<std::vector<std::pair<float, float>>>;
		std::shared_lock<std::shared_mutex> lock(chunk_mutex_copy);
		tile_data_copy[x + y * CHUNKS_WIDTH]->create_outlines(outlines);

		return outlines;
	}

	const int adjacent_tiles_dx[4] = {-1, 0, 0, -1}; // bottom left, bottom right, top right, top left
	const int adjacent_tiles_dy[4] = {0, 0, -1, -1};
	const int offset[4] = {1, 2, 4, 8};
	uint16_t world_tile_system::get_tile_edginess(int chunk_x, int chunk_y, int x, int y)
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
			// if (is_solid_tile[data[current_frame][adjacent_y][adjacent_x]])
			if(tile_data_copy[chunk_x + chunk_y * CHUNKS_WIDTH]->get_tile(adjacent_x, adjacent_y) >= SOLID_TILE_START_INDEX)
			{
				edginess += offset[i];
			}
		}

		return edginess;
	}


	// std::vector<std::vector<std::pair<float, float>>> *world_tile_system::create_outlines_from_chunk(int chunk_x, int chunk_y)
	// {
	// 	std::vector<std::vector<std::pair<float, float>>> *outlines = new std::vector<std::vector<std::pair<float, float>>>;
	// 	std::shared_lock<std::shared_mutex> lock(chunk_mutex_copy);

	// 	std::unordered_map<std::pair<float, float>, line_mapping_pair, chunk_coord_pair_hash> edge_lines;
	// 	std::unordered_map<std::pair<float, float>, line_mapping_pair, chunk_coord_pair_hash> edge_lines_reverse;

	// 	for (int y = 0; y < CHUNK_SIZE + 1; y++)
	// 	{
	// 		for (int x = 0; x < CHUNK_SIZE + 1; x++)
	// 		{
	// 			// if (data[y][x] >= SOLID_TILE_START_INDEX)
	// 			// {
	// 			uint16_t edginess = get_tile_edginess(chunk_x, chunk_y, x, y);
	// 			for (tile_linef line : edges_lines[edginess])
	// 			{
	// 				line.x1 += x;
	// 				line.y1 += y;
	// 				line.x2 += x;
	// 				line.y2 += y;
	// 				if (!edge_lines.count({line.x1, line.y1}))
	// 				{
	// 					edge_lines[{line.x1, line.y1}] = line_mapping_pair({line.x2, line.y2});

	// 					if (!edge_lines_reverse.count({line.x2, line.y2}))
	// 						edge_lines_reverse[{line.x2, line.y2}] = line_mapping_pair({line.x1, line.y1});
	// 					else
	// 						edge_lines_reverse[{line.x2, line.y2}].insert({line.x1, line.y1});
	// 				}
	// 				else
	// 				{
	// 					edge_lines[{line.x1, line.y1}].insert({line.x2, line.y2});
	// 					if (!edge_lines_reverse.count({line.x2, line.y2}))
	// 						edge_lines_reverse[{line.x2, line.y2}] = line_mapping_pair({line.x1, line.y1});
	// 					else
	// 						edge_lines_reverse[{line.x2, line.y2}].insert({line.x1, line.y1});
	// 				}
	// 			}
	// 			// }
	// 		}
	// 	}
	// }

	// std::vector<std::vector<std::pair<float, float>>> *world_tile_system::create_outline_from_custom_shape(std::vector<std::vector<tile_type>> tiles)
	// {
	// 	std::vector<std::pair<float, float>> outline;
		

	// 	return 
	// }

	uint32_t world_tile_system::delete_circle(int x, int y, int radius, std::unordered_set<uint8_t> tile_deny_list)
	{
		std::unique_lock<std::shared_mutex> lock(chunk_mutex_base);
		uint32_t num_deleted = 0;
		for (int i = 0; i < NUM_CHUNKS; i++)
		{
			uint32_t modified;
			modified = tile_data_base[i]->delete_circle(x, y, radius, tile_deny_list);
			num_deleted += modified;

			if (modified)
			{
				set_modified_chunk(i % CHUNKS_WIDTH, i / CHUNKS_WIDTH, 1);
			}
		}
		return num_deleted;
	}

	std::array<uint8_t, game::NUM_CHUNKS> *world_tile_system::get_modified_chunks()
	{
		return &modified_chunks;
	}

	void world_tile_system::set_modified_chunk(int x, int y, uint8_t value)
	{
		modified_chunks[y * CHUNKS_WIDTH + x] = value;
	}

	std::string world_tile_system::to_csv_string()
	{
		std::string csv_string = "";
		for (int y = 0; y < CHUNKS_WIDTH * CHUNK_SIZE; y++)
		{
			for (int x = 0; x < CHUNKS_WIDTH * CHUNK_SIZE; x++)
			{
				csv_string += std::to_string(get_write_tile_at(x, y));
				if (x < CHUNKS_WIDTH * CHUNK_SIZE - 1)
				{
					csv_string += ",";
				}
			}
			csv_string += "\n";
		}

		return csv_string;
	}

	bool world_tile_system::find_tile_in_rect(std::pair<int, int> &result, int x, int y, int w, int h, std::unordered_set<uint8_t> tile_types)
	{
		std::shared_lock<std::shared_mutex> lock(chunk_mutex_copy);
		for (int i = 0; i < NUM_CHUNKS; i++)
		{
			if (tile_data_copy[i]->find_tile_in_rect(result, x, y, w, h, tile_types))
			{
				return true;
			}
		}
		return false;
	}
}