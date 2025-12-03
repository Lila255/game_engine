#include "world_tile_system.hpp"
#include "box2d_system.hpp"
#include "tasks.hpp"
#include "legged_creature_system.hpp"
#include "flying_creature_system.hpp"

namespace game
{
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
		for (int i = 0; i < RENDERED_NUM_CHUNKS; i++)
		{
			int chunk_x = i % RENDERED_CHUNKS_WIDTH;
			int chunk_y = i / RENDERED_CHUNKS_WIDTH;
			tile_data_map[{chunk_x, chunk_y}] = new chunk(chunk_x, chunk_y);
			tile_data_map[{chunk_x, chunk_y}]->create_chunk(chunk_x, chunk_y);
			chunk_entities_map[{chunk_x, chunk_y}] = game_engine::game_engine_pointer->create_entity();
			// tile_data_copy[i] = new chunk(chunk_x, chunk_y);
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

		std::fill(is_tile_fixed.begin(), is_tile_fixed.end(), 0);
		std::fill(is_tile_fixed.begin() + tile_type::GLASS, is_tile_fixed.begin() + tile_type::SOLID_63 + 1, 1);
		is_tile_fixed[SNOW] = 0;
		is_tile_fixed[ASH] = 0;
		is_tile_fixed[SAND] = 0;

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
		tile_max_temperature[INSULATION_FOAM] = 10;
		tile_max_temperature[CHARCOAL] = 200;
		tile_max_temperature[IRON] = 1538;

		// min temps
		tile_min_temperature[STEAM] = 100;
		tile_min_temperature[WATER] = 0;
		tile_min_temperature[LAVA] = 1500;
		tile_min_temperature[LIQUID_GLASS] = 1400;

		// max temp changes
		max_temp_tile_change[WATER] = STEAM;
		max_temp_tile_change[GLASS] = LIQUID_GLASS;

		max_temp_tile_change[LEAF] = LIGHT_EMBER;
		max_temp_tile_change[STONE] = LAVA;
		max_temp_tile_change[DIRT] = SAND;
		max_temp_tile_change[SAND] = LIQUID_GLASS;
		max_temp_tile_change[GRASS] = SAND;
		max_temp_tile_change[WOOD] = EMBER;
		max_temp_tile_change[CHARCOAL] = EMBER;
		max_temp_tile_change[TREE_SEED] = EMBER;
		max_temp_tile_change[ROOT] = EMBER;
		max_temp_tile_change[SNOW] = WATER;
		max_temp_tile_change[INSULATION_FOAM] = INSULATION;

		// min temp changes
		min_temp_tile_change[STEAM] = WATER;
		min_temp_tile_change[WATER] = SNOW;
		min_temp_tile_change[LAVA] = STONE;
		min_temp_tile_change[LIQUID_GLASS] = GLASS;
		min_temp_tile_change[EMBER] = CHARCOAL;
		min_temp_tile_change[LIGHT_EMBER] = ASH;

		// tile heat capacity, with density baked in.
		// Joules to raise 1000kg of tile by 1 degree Celsius.
		// Tile mass is constant, so we can bake it into the heat capacity.
		tile_heat_capacity.fill(0.001f);

		tile_heat_capacity[AIR] = 0.00125f;
		tile_heat_capacity[STEAM] = 1.864f;
		tile_heat_capacity[POLLUTION] = 0.5f;
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
		tile_heat_capacity[INSULATION] = 0.05f;
		tile_heat_capacity[INSULATION_FOAM] = 0.21f;
		tile_heat_capacity[IRON] = 0.45f;

		tile_heat_capacity[BEDROCK] = 1000000.0f;

		tile_thermal_conductivity.fill(1.0f);
		tile_thermal_conductivity[WATER] = 2.6f;
		tile_thermal_conductivity[INSULATION] = 0.0001f;
		tile_thermal_conductivity[INSULATION_FOAM] = 0.001f;
		tile_thermal_conductivity[IRON] = 55.0f;

		tile_debris_fall_speed_multiplier.fill(1.0f);
		tile_debris_fall_speed_multiplier[SNOW] = 0.3f;

		tile_phase_change_count_multiplier.fill(1);
		std::fill(tile_phase_change_count_multiplier.begin() + tile_type::AIR, tile_phase_change_count_multiplier.begin() + tile_type::FLUID_31 + 1, 3);
		// make liquids have a higher phase change count
		std::fill(tile_phase_change_count_multiplier.begin() + tile_type::WATER, tile_phase_change_count_multiplier.begin() + tile_type::FLUID_31 + 1, -3);

		tile_phase_change_count_multiplier[INSULATION] = 4;

		task_scheduler_pointer = new game_engine::task_scheduler();
		task_scheduler_pointer->set_max_threads(16);

		task_scheduler_thread = std::thread(&game_engine::task_scheduler::start, task_scheduler_pointer);
	}

	world_tile_system::~world_tile_system()
	{
		task_scheduler_pointer->shutdown({0, 0});
		task_scheduler_thread.join();

		for (auto &[key, value] : tile_data_map)
		{
			delete value;
			// delete tile_data_copy[i];
		}
	}

	void world_tile_system::set_all_chunk_ent(entity ent)
	{
		all_chunk_ent = ent;
	}

	uint8_t world_tile_system::get_tile_at(int x, int y)
	{
		if (x < 0)
			x--;
		if (y < 0)
			y--;
		int chunk_x = x / CHUNK_SIZE;
		int chunk_y = y / CHUNK_SIZE;
		if (x < 0)
			chunk_x--;
		if (y < 0)
			chunk_y--;
		int tile_x = x % CHUNK_SIZE;
		int tile_y = y % CHUNK_SIZE;
		while (tile_x < 0)
		{
			tile_x += CHUNK_SIZE;
		}
		while (tile_y < 0)
		{
			tile_y += CHUNK_SIZE;
		}

		if (tile_data_map.count({chunk_x, chunk_y}) == 0)
			return (tile_type)0;

		chunk *c = tile_data_map[{chunk_x, chunk_y}];
		c->lock_chunk_copy_shared();
		uint8_t tile = c->get_tile_copy(tile_x, tile_y);
		c->unlock_chunk_copy_shared();

		return tile;
	}

	uint8_t world_tile_system::get_write_tile_at(int x, int y)
	{
		if (x < 0)
			x--;
		if (y < 0)
			y--;
		int chunk_x = x / CHUNK_SIZE;
		int chunk_y = y / CHUNK_SIZE;
		if (x < 0)
			chunk_x--;
		if (y < 0)
			chunk_y--;
		int tile_x = x % CHUNK_SIZE;
		int tile_y = y % CHUNK_SIZE;
		while (tile_x < 0)
		{
			tile_x += CHUNK_SIZE;
		}
		while (tile_y < 0)
		{
			tile_y += CHUNK_SIZE;
		}

		if (tile_data_map.count({chunk_x, chunk_y}) == 0)
			return (tile_type)0;

		chunk *c = tile_data_map[{chunk_x, chunk_y}];
		c->lock_chunk_shared();
		uint8_t tile = c->get_tile(tile_x, tile_y);
		c->unlock_chunk_shared();

		return tile;
	}

	void world_tile_system::set_tile_at_with_lock(int x, int y, uint8_t t)
	{
		if (x < 0)
			x--;
		if (y < 0)
			y--;
		int chunk_x = x / CHUNK_SIZE;
		int chunk_y = y / CHUNK_SIZE;
		if (x < 0)
			chunk_x--;
		if (y < 0)
			chunk_y--;
		if (tile_data_map.count({chunk_x, chunk_y}) == 0)
			return;

		int tile_x = x % CHUNK_SIZE;
		int tile_y = y % CHUNK_SIZE;
		while (tile_x < 0)
		{
			tile_x += CHUNK_SIZE;
		}
		while (tile_y < 0)
		{
			tile_y += CHUNK_SIZE;
		}

		uint8_t current_tile = get_tile_at(x, y);

		if (current_tile == BEDROCK)
			return;

		chunk *c = tile_data_map[{chunk_x, chunk_y}];
		c->lock_chunk();
		if (is_solid_tile[t] != is_solid_tile[current_tile])
		{
			set_modified_chunk(chunk_x, chunk_y, 1);
		}

		c->set_tile(tile_x, tile_y, t);
		c->unlock_chunk();
	}

	void world_tile_system::set_tile_at_with_search_and_lock(int x, int y, uint8_t t)
	{
		if (x < 0)
			x--;
		if (y < 0)
			y--;
		int chunk_x = x / CHUNK_SIZE;
		int chunk_y = y / CHUNK_SIZE;
		if (x < 0)
			chunk_x--;
		if (y < 0)
			chunk_y--;
		if (tile_data_map.count({chunk_x, chunk_y}) == 0)
			return;
		int tile_x = x % CHUNK_SIZE;
		int tile_y = y % CHUNK_SIZE;
		while (tile_x < 0)
		{
			tile_x += CHUNK_SIZE;
		}
		while (tile_y < 0)
		{
			tile_y += CHUNK_SIZE;
		}

		std::unordered_set<tile_coord, global_tile_coord_hash> checked_tiles;
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
		if (x < 0)
			x--;
		if (y < 0)
			y--;
		int chunk_x = x / CHUNK_SIZE;
		int chunk_y = y / CHUNK_SIZE;
		if (x < 0)
			chunk_x--;
		if (y < 0)
			chunk_y--;
		int tile_x = x % CHUNK_SIZE;
		int tile_y = y % CHUNK_SIZE;
		while (tile_x < 0)
		{
			tile_x += CHUNK_SIZE;
		}
		while (tile_y < 0)
		{
			tile_y += CHUNK_SIZE;
		}
		if (tile_data_map.count({chunk_x, chunk_y}) == 0)
			return;

		chunk *c = tile_data_map[{chunk_x, chunk_y}];
		c->lock_chunk_copy();
		c->set_tile_copy(tile_x, tile_y, t);
		c->unlock_chunk_copy();
	}

	float world_tile_system::get_tile_temperature_at(int x, int y)
	{
		if (x < 0)
			x--;
		if (y < 0)
			y--;
		int chunk_x = x / CHUNK_SIZE;
		int chunk_y = y / CHUNK_SIZE;
		if (x < 0)
			chunk_x--;
		if (y < 0)
			chunk_y--;
		int tile_x = x % CHUNK_SIZE;
		int tile_y = y % CHUNK_SIZE;
		while (tile_x < 0)
		{
			tile_x += CHUNK_SIZE;
		}
		while (tile_y < 0)
		{
			tile_y += CHUNK_SIZE;
		}
		if (tile_data_map.count({chunk_x, chunk_y}) == 0)
			return 0;

		chunk *c = tile_data_map[{chunk_x, chunk_y}];
		c->lock_chunk_shared();
		float temp = c->get_tile_temperature(tile_x, tile_y);
		c->unlock_chunk_shared();
		return temp;
	}

	uint16_t world_tile_system::get_tile_misc_data_at(int x, int y)
	{
		if (x < 0)
			x--;
		if (y < 0)
			y--;
		int chunk_x = x / CHUNK_SIZE;
		int chunk_y = y / CHUNK_SIZE;
		if (x < 0)
			chunk_x--;
		if (y < 0)
			chunk_y--;
		int tile_x = x % CHUNK_SIZE;
		int tile_y = y % CHUNK_SIZE;
		while (tile_x < 0)
		{
			tile_x += CHUNK_SIZE;
		}
		while (tile_y < 0)
		{
			tile_y += CHUNK_SIZE;
		}
		if (tile_data_map.count({chunk_x, chunk_y}) == 0)
			return 0;

		chunk *c = tile_data_map[{chunk_x, chunk_y}];
		c->lock_chunk_shared();
		uint16_t misc_data = c->get_misc_data_at(tile_x, tile_y);
		c->unlock_chunk_shared();
		return misc_data;
	}

	void world_tile_system::set_tile_misc_data_at(int x, int y, uint16_t value)
	{
		if (x < 0)
			x--;
		if (y < 0)
			y--;
		int chunk_x = x / CHUNK_SIZE;
		int chunk_y = y / CHUNK_SIZE;
		if (x < 0)
			chunk_x--;
		if (y < 0)
			chunk_y--;
		int tile_x = x % CHUNK_SIZE;
		int tile_y = y % CHUNK_SIZE;
		while (tile_x < 0)
		{
			tile_x += CHUNK_SIZE;
		}
		while (tile_y < 0)
		{
			tile_y += CHUNK_SIZE;
		}
		if (tile_data_map.count({chunk_x, chunk_y}) == 0)
			return;

		chunk *c = tile_data_map[{chunk_x, chunk_y}];
		c->lock_chunk();
		c->set_misc_data_at(tile_x, tile_y, value);
		c->unlock_chunk();
	}

	void world_tile_system::add_tile_misc_data_at(int x, int y, int16_t value)
	{
		if (x < 0)
			x--;
		if (y < 0)
			y--;
		int chunk_x = x / CHUNK_SIZE;
		int chunk_y = y / CHUNK_SIZE;
		if (x < 0)
			chunk_x--;
		if (y < 0)
			chunk_y--;
		int tile_x = x % CHUNK_SIZE;
		int tile_y = y % CHUNK_SIZE;
		while (tile_x < 0)
		{
			tile_x += CHUNK_SIZE;
		}
		while (tile_y < 0)
		{
			tile_y += CHUNK_SIZE;
		}
		if (tile_data_map.count({chunk_x, chunk_y}) == 0)
			return;

		chunk *c = tile_data_map[{chunk_x, chunk_y}];
		c->lock_chunk();
		c->add_misc_data_at(tile_x, tile_y, value);
		c->unlock_chunk();
	}

	void world_tile_system::set_tile_temperature_at(int x, int y, float temperature)
	{
		if (x < 0)
			x--;
		if (y < 0)
			y--;
		int chunk_x = x / CHUNK_SIZE;
		int chunk_y = y / CHUNK_SIZE;
		if (x < 0)
			chunk_x--;
		if (y < 0)
			chunk_y--;
		int tile_x = x % CHUNK_SIZE;
		int tile_y = y % CHUNK_SIZE;
		while (tile_x < 0)
		{
			tile_x += CHUNK_SIZE;
		}
		while (tile_y < 0)
		{
			tile_y += CHUNK_SIZE;
		}
		if (tile_data_map.count({chunk_x, chunk_y}) == 0)
			return;
		chunk *c = tile_data_map[{chunk_x, chunk_y}];
		// (tile_data_base[chunk_num])->lock_chunk();
		c->set_tile_temperature(tile_x, tile_y, temperature);
		// (tile_data_base[chunk_num])->unlock_chunk();
	}

	void world_tile_system::add_tile_temperature(int x, int y, float temperature)
	{
		if (x < 0)
			x--;
		if (y < 0)
			y--;
		int chunk_x = x / CHUNK_SIZE;
		int chunk_y = y / CHUNK_SIZE;
		if (x < 0)
			chunk_x--;
		if (y < 0)
			chunk_y--;
		int tile_x = x % CHUNK_SIZE;
		int tile_y = y % CHUNK_SIZE;
		while (tile_x < 0)
		{
			tile_x += CHUNK_SIZE;
		}
		while (tile_y < 0)
		{
			tile_y += CHUNK_SIZE;
		}
		if (tile_data_map.count({chunk_x, chunk_y}) == 0)
			return;
		chunk *c = tile_data_map[{chunk_x, chunk_y}];
		c->lock_chunk();
		c->add_tile_temperature(tile_x, tile_y, temperature);
		c->unlock_chunk();
	}

	// set_tile_at_no_lock(x, y, get_write_tile_at(x + 1, y + 1));
	// set_tile_at_no_lock(x + 1, y + 1, SAND);

	void world_tile_system::switch_tiles_no_lock(int x1, int y1, int x2, int y2)
	{
		tile_type t1 = (tile_type)get_write_tile_at(x1, y1);
		tile_type t2 = (tile_type)get_write_tile_at(x2, y2);

		if (t1 == BEDROCK || t2 == BEDROCK)
			return;

		// get temperatures so we can swap those too
		float t1_temp = get_tile_temperature_at(x1, y1);
		float t2_temp = get_tile_temperature_at(x2, y2);

		uint16_t t1_misc = get_tile_misc_data_at(x1, y1);
		uint16_t t2_misc = get_tile_misc_data_at(x2, y2);

		set_tile_at_no_lock(x1, y1, t2);
		set_tile_at_no_lock(x2, y2, t1);

		set_tile_temperature_at(x1, y1, t2_temp);
		set_tile_temperature_at(x2, y2, t1_temp);

		set_tile_misc_data_at(x1, y1, t2_misc);
		set_tile_misc_data_at(x2, y2, t1_misc);
	}

	void world_tile_system::switch_tiles_with_lock(int x1, int y1, int x2, int y2)
	{
		// std::unique_lock<std::shared_mutex> lock_copy(chunk_mutex_copy);
		tile_type t1 = (tile_type)get_write_tile_at(x1, y1);
		tile_type t2 = (tile_type)get_write_tile_at(x2, y2);

		if (t1 == BEDROCK || t2 == BEDROCK)
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
		if (x < 0)
			x--;
		if (y < 0)
			y--;
		int chunk_x = x / CHUNK_SIZE;
		int chunk_y = y / CHUNK_SIZE;
		if (x < 0)
			chunk_x--;
		if (y < 0)
			chunk_y--;
		int tile_x = x % CHUNK_SIZE;
		int tile_y = y % CHUNK_SIZE;
		while (tile_x < 0)
		{
			tile_x += CHUNK_SIZE;
		}
		while (tile_y < 0)
		{
			tile_y += CHUNK_SIZE;
		}
		if (tile_data_map.count({chunk_x, chunk_y}) == 0)
			return;

		if ((tile_type >= GLASS) != (get_tile_at(x, y) >= GLASS))
		{
			set_modified_chunk(chunk_x, chunk_y, 1);
		}

		// std::shared_lock<std::shared_mutex> lock(chunk_mutex);
		chunk *c = tile_data_map[{chunk_x, chunk_y}];
		c->set_tile(tile_x, tile_y, tile_type);
	}

	bool world_tile_system::try_consume_nearby_tile_no_lock(int x, int y, tile_type tile_type)
	{
		std::unordered_set<tile_coord, global_tile_coord_hash> checked_tiles;
		std::queue<tile_coord> tile_queue;
		tile_queue.push(tile_coord(x, y));
		int dx[] = {0, 1, 0, -1};
		int dy[] = {-1, 0, 1, 0};

		while (!tile_queue.empty() && checked_tiles.size() < 20)
		{
			tile_coord current_tile = tile_queue.front();
			tile_queue.pop();

			uint8_t current_tile_type = get_write_tile_at(current_tile.x, current_tile.y);

			if (current_tile_type == tile_type)
			{
				set_tile_at_no_lock(current_tile.x, current_tile.y, AIR);
				return true;
			}
			else
			{
				for (int i = 0; i < 4; i++)
				{
					tile_coord new_tile = tile_coord(current_tile.x + dx[i], current_tile.y + dy[i]);

					if (!checked_tiles.count(new_tile))
					{
						tile_queue.push(new_tile);
						checked_tiles.insert(new_tile);
					}
				}
			}
		}
		return false;
	}

	bool world_tile_system::try_place_tile_with_displacement_no_lock(int x, int y, tile_type tile_type, float temperature, uint16_t misc_data, int recursion_depth, int search_size)
	{
		// int chunk_x = x / CHUNK_SIZE;
		// int chunk_y = y / CHUNK_SIZE;
		// int chunk_num = chunk_x + chunk_y * CHUNKS_WIDTH;
		// if (chunk_x < 0 || chunk_x >= CHUNKS_WIDTH || chunk_y < 0 || chunk_y >= CHUNKS_WIDTH)
		// 	return false;
		// if (chunk_num < 0 || chunk_num >= NUM_CHUNKS)
		// 	return false;

		// chunk *c = tile_data_base[chunk_num];
		// bool success = c->try_place_tile_with_displacement_no_lock(x, y, tile_type, temperature, misc_data, recursion_depth, search_size);
		// return success;

		std::unordered_set<tile_coord, global_tile_coord_hash> checked_tiles;
		std::queue<tile_coord> tile_queue;
		tile_queue.push(tile_coord(x, y));
		int dx[] = {0, 1, 0, -1};
		int dy[] = {-1, 0, 1, 0};

		tile_simple_type simple_type = get_simple_tile_type((uint8_t)tile_type);

		bool found_non_solid = false;
		while (!tile_queue.empty() && (!found_non_solid && checked_tiles.size() < search_size || found_non_solid && checked_tiles.size() < search_size))
		{
			tile_coord current_tile = tile_queue.front();
			tile_queue.pop();

			uint8_t current_tile_type = get_write_tile_at(current_tile.x, current_tile.y);
			float current_tile_temp = get_tile_temperature_at(current_tile.x, current_tile.y);
			uint16_t current_tile_misc = get_tile_misc_data_at(current_tile.x, current_tile.y);

			if (!found_non_solid && is_solid_tile[current_tile_type] == 0)
			{
				found_non_solid = true;
				tile_queue = {};
			}

			if ((found_non_solid && is_solid_tile[current_tile_type] == 0) && (current_tile_type != tile_type || recursion_depth == 0))
			{
				// try place the tile here
				tile_simple_type current_simple_type = get_simple_tile_type(current_tile_type);
				if (simple_type >= current_simple_type || is_solid_tile[current_tile_type] == 0)
				{
					switch (current_tile_type)
					{
					case AIR:
						// case VACUUM:
						set_tile_at_no_lock(current_tile.x, current_tile.y, tile_type);
						set_tile_temperature_at(current_tile.x, current_tile.y, temperature);
						set_tile_misc_data_at(current_tile.x, current_tile.y, misc_data);
						return true;

					default:
						// if the current tile is not fixed, we can replace it
						if (simple_type > current_simple_type)
						{
							// we can try displace this tile

							if (recursion_depth > 10)
							{
								printf("recursion depth exceeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeded\n");
								return false;
							}
							bool success = try_place_tile_with_displacement_no_lock(current_tile.x, current_tile.y, (game::tile_type)current_tile_type, current_tile_temp, current_tile_misc, recursion_depth + 1, search_size);

							if (success)
							{
								set_tile_at_no_lock(current_tile.x, current_tile.y, tile_type);
								set_tile_temperature_at(current_tile.x, current_tile.y, temperature);
								set_tile_misc_data_at(current_tile.x, current_tile.y, misc_data);
								return true;
							}
							else
							{
								return false;
							}
						}
						break;
					}
				}
			}

			for (int i = 0; i < 4; i++)
			{
				tile_coord new_tile = tile_coord(current_tile.x + dx[i], current_tile.y + dy[i]);

				if (!checked_tiles.count(new_tile))
				{
					if (found_non_solid)
					{
						if (is_solid_tile[get_write_tile_at(new_tile.x, new_tile.y)] == 0)
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
		if (found_non_solid)
		{
			// printf("couldn't place tile, found non-solid tile\n");
		}
		else
		{
			// printf("couldn't place tile, no non-solid tile found\n");
		}
		return false;
	}


	bool world_tile_system::try_place_tile_flush_with_displacement_no_lock(int x, int y, tile_type tile_type, float temperature, uint16_t misc_data, int recursion_depth, int search_size)
	{
		uint8_t search_width = 3;
		std::pair<int, int> best_tile = {-1, -1};
		int8_t best_solid_neighbours = -1;
		int dx[] = {0, 1, 0, -1};
		int dy[] = {-1, 0, 1, 0};

		for (int i = -search_width; i <= search_width; i++)
		{
			for (int j = -search_width; j <= search_width; j++)
			{
				int check_x = x + i;
				int check_y = y + j;

				uint8_t current_tile_type = get_write_tile_at(check_x, check_y);

				if (is_solid_tile[current_tile_type] == 0)
				{
					// count solid neighbours
					uint8_t solid_neighbours = 0;
					for (int k = 0; k < 4; k++)
					{
						int neighbour_x = check_x + dx[k];
						int neighbour_y = check_y + dy[k];
						uint8_t neighbour_tile_type = get_write_tile_at(neighbour_x, neighbour_y);
						if (is_solid_tile[neighbour_tile_type])
						{
							solid_neighbours++;
						}
					}

					if (solid_neighbours > best_solid_neighbours || solid_neighbours == best_solid_neighbours && (i == 0 && j == 0))
					{
						best_solid_neighbours = solid_neighbours;
						best_tile = {check_x, check_y};
					}
				}
			}
		}
		if (best_solid_neighbours >= 0)
		{
			std::pair<int32_t, int32_t> chunk_coord = get_chunk_coords_from_world_coords(best_tile.first, best_tile.second);
			if (tile_data_map.count(chunk_coord) == 0)
			{
				return false;
			}
			std::pair<uint16_t, uint16_t> local_coords = get_intra_chunk_coords_from_world_coords(best_tile.first, best_tile.second);
			chunk *c = tile_data_map[chunk_coord];
			c -> lock_chunk();
			bool success = c->try_place_tile_with_displacement_no_lock(local_coords.first, local_coords.second, tile_type, temperature, misc_data, recursion_depth, search_size);
			// if (!success)
			// {
			// 	bool success2 = c->try_place_tile_with_displacement_no_lock(local_coords.first, local_coords.second, tile_type, temperature, misc_data, recursion_depth, search_size);
			// 	printf("failed to place tile at best tile\n");
			// }
			c -> unlock_chunk();
			return success;
			// return try_place_tile_with_displacement_no_lock(best_tile.first, best_tile.second, tile_type, temperature, misc_data, recursion_depth, search_size);
		}
		return false;
	}

	void world_tile_system::update(uint64_t tick_count)
	{
		increment_counter();

		game_engine::box player_box = ((game_engine::box_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::box_system>()))->get(game_engine::game_engine_pointer->player_entitiy).get_box();
		int player_x = (int)(player_box.x + player_box.w / 2);
		int player_y = (int)(player_box.y + player_box.h / 2);
		// int chunk_x = player_x / CHUNK_SIZE;
		// int chunk_y = player_y / CHUNK_SIZE;
		std::pair<int32_t, int32_t> player_chunk = get_chunk_coords_from_world_coords(player_x, player_y);
		int32_t chunk_x = player_chunk.first;
		int32_t chunk_y = player_chunk.second;

		// game::RENDERED_CHUNKS_WIDTH
		int32_t start_chunk_x = chunk_x - game::RENDERED_CHUNKS_WIDTH / 2;
		int32_t start_chunk_y = chunk_y - game::RENDERED_CHUNKS_WIDTH / 2;

		for (int32_t y = start_chunk_y; y < start_chunk_y + game::RENDERED_CHUNKS_WIDTH; y++)
		{
			for (int32_t x = start_chunk_x; x < start_chunk_x + game::RENDERED_CHUNKS_WIDTH; x++)
			{
				if (tile_data_map.count({x, y}) == 0)
				{
					task_scheduler_pointer->add_task({&generate_chunk_task_wrapper, new generate_chunk_parameters{x, y, this}});
				}
				else
				{
					chunk *c = tile_data_map[{x, y}];
					task_scheduler_pointer->add_task({&update_chunk_tiles_task_wrapper, new update_chunk_tiles_parameters{c, this, tick_count, x == chunk_x && y == chunk_y}});
				}
			}
		}

		std::unordered_set<entity> nearby_chunks;
		for (int32_t y = start_chunk_y; y < start_chunk_y + game::RENDERED_CHUNKS_WIDTH; y++)
		{
			for (int32_t x = start_chunk_x; x < start_chunk_x + game::RENDERED_CHUNKS_WIDTH; x++)
			{
				if (tile_data_map.count({x, y}) == 0 || chunk_entities_map.count({x, y}) == 0)
				{
					continue;
				}
				nearby_chunks.insert(chunk_entities_map[{x, y}]);
				if (x == start_chunk_x || y == start_chunk_y || x == start_chunk_x + game::RENDERED_CHUNKS_WIDTH - 1 || y == start_chunk_y + game::RENDERED_CHUNKS_WIDTH - 1)
				{
					continue;	// only update 3x3, but dont delete 5x5
				}

				chunk *c = tile_data_map[{x, y}];
				task_scheduler_pointer->add_task({&recalculate_chunk_outlines_task, new recalculate_chunk_outlines_parameters{c, this, chunk_entities_map[{x, y}]}});
			}
		}

		box2d_system *b2d_system = (box2d_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<box2d_system>());
		std::vector<entity> static_body_entities = b2d_system->get_static_body_entities();

		for (auto &ent : static_body_entities)
		{
			if (nearby_chunks.count(ent) == 0)
			{
				b2d_system->remove_static_body(ent);
			}
		}
		// task_scheduler_pointer->add_task({&recalculate_chunk_outlines_task, new recalculate_chunk_outlines_parameters{tile_data_map[player_chunk], this, chunk_entities_map[player_chunk]}});

		// for (int i = 0; i < NUM_CHUNKS; i++)
		// for (auto &[key, c] : tile_data_map)
		// {
		// 	task_scheduler_pointer->add_task({&update_chunk_tiles_task_wrapper, new update_chunk_tiles_parameters{c, this, tick_count}});
		// }

		// for (auto &[key, c] : tile_data_map)
		// {
		// 	task_scheduler_pointer->add_task({&recalculate_chunk_outlines_task, new recalculate_chunk_outlines_parameters{c, this, chunk_entities_map[key]}});
		// }

		return;
		/*
		{
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
					uint16_t tile_misc = get_tile_misc_data_at(x, y);

					// check if tile is overheated
					if (tile_temp > tile_max_temperature[tile_t] + 2)
					{
						if (max_temp_tile_change.count((tile_type)tile_t))
						{
							tile_type new_tile_type = max_temp_tile_change[(tile_type)tile_t];
							if (tile_phase_change_count_multiplier[new_tile_type] > 0)
							{
								// need to place multiple tiles
								int changes = tile_phase_change_count_multiplier[new_tile_type];
								std::pair<int, int> original_pos = {x, y};
								std::pair<int, int> current_pos = {x, y};
								tile_type original_tile_type = (tile_type)tile_t;
								float original_tile_temp = tile_temp;

								set_tile_at_no_lock(x, y, AIR);

								for (int i = 0; i < changes; i++)
								{
									bool success = try_place_tile_with_displacement_no_lock(current_pos.first, current_pos.second, max_temp_tile_change[(tile_type)tile_t], tile_temp, tile_misc, 0, 512);
									if (!success)
									{
										// couldn't place the tile, try role back
										if (i > 0)
										{
											for (int j = 0; j < i; j++)
											{
												bool rollback_success = try_consume_nearby_tile_no_lock(x, y, max_temp_tile_change[(tile_type)tile_t]);
												if (!rollback_success)
												{
													// fucked it
												}
											}
										}
										set_tile_at_no_lock(x, y, original_tile_type);
										break;
									}
								}
							}
							else if (tile_phase_change_count_multiplier[new_tile_type] < 0)
							{
								int changes = -tile_phase_change_count_multiplier[new_tile_type];
								bool all_success = true;
								for (int i = 0; i < changes; i++)
								{
									bool success = try_consume_nearby_tile_no_lock(x, y, max_temp_tile_change[(tile_type)tile_t]);
									if (!success)
									{
										all_success = false;
										// couldn't consume the tile, stop and rollback
										if (i > 0)
										{
											for (int j = 0; j < i; j++)
											{
												bool rollback_success = try_place_tile_with_displacement_no_lock(x, y, max_temp_tile_change[(tile_type)tile_t], tile_temp, tile_misc, 0, 512);
												if (!rollback_success)
												{
													// fucked it
												}
											}
										}
										break;
									}
								}
								if (all_success)
								{
									set_tile_at_no_lock(x, y, new_tile_type);
								}
							}
							else
							{
								set_tile_at_no_lock(x, y, max_temp_tile_change[(tile_type)tile_t]);
							}
						}
						else
						{
							// printf(" overheated with temperaTile %d at (%d, %d) is overheated with temperature %d, but no max temp change defined.\n", tile_t, x, y, tile_temp);
						}
					}
					// check if tile is too cold
					else if (tile_temp < tile_min_temperature[tile_t] - 2)
					{
						if (min_temp_tile_change.count((tile_type)tile_t))
						{
							tile_type new_tile_type = min_temp_tile_change[(tile_type)tile_t];
							if (tile_phase_change_count_multiplier[new_tile_type] > 0)
							{
								// need to place multiple tiles
								int changes = tile_phase_change_count_multiplier[new_tile_type];
								std::pair<int, int> original_pos = {x, y};
								std::pair<int, int> current_pos = {x, y};
								tile_type original_tile_type = (tile_type)tile_t;
								float original_tile_temp = tile_temp;
								set_tile_at_no_lock(x, y, AIR);

								for (int i = 0; i < changes; i++)
								{
									bool success = try_place_tile_with_displacement_no_lock(current_pos.first, current_pos.second, min_temp_tile_change[(tile_type)tile_t], tile_temp, tile_misc, 0, 512);
									if (!success)
									{
										// couldn't place the tile, try role back
										if (i > 0)
										{
											for (int j = 0; j < i; j++)
											{
												bool rollback_success = try_consume_nearby_tile_no_lock(x, y, min_temp_tile_change[(tile_type)tile_t]);
												if (!rollback_success)
												{
													// fucked it
												}
											}
										}
										set_tile_at_no_lock(x, y, original_tile_type);
										break;
									}
								}
							}
							else if (tile_phase_change_count_multiplier[tile_t] < 0)
							{
								int changes = -tile_phase_change_count_multiplier[tile_t];
								bool all_success = true;
								for (int i = 0; i < changes; i++)
								{
									bool success = try_consume_nearby_tile_no_lock(x, y, min_temp_tile_change[(tile_type)tile_t]);
									if (!success)
									{
										all_success = false;
										// couldn't consume the tile, stop and rollback
										if (i > 0)
										{
											for (int j = 0; j < i; j++)
											{
												bool rollback_success = try_place_tile_with_displacement_no_lock(x, y, min_temp_tile_change[(tile_type)tile_t], tile_temp, tile_misc, 0, 512);
												if (!rollback_success)
												{
													// fucked it
												}
												// try place back the consumed tiles
											}
										}
										break;
									}
								}
								if (all_success)
								{
									set_tile_at_no_lock(x, y, new_tile_type);
								}
							}
							else
							{
								set_tile_at_no_lock(x, y, min_temp_tile_change[(tile_type)tile_t]);
							}
						}
					}
					else
					{
					}

					// transfer heat to adjacent tiles
					for (int i = 0; i < 2; i++)
					{
						int adjacent_x = x + heat_dx[i];
						int adjacent_y = y + heat_dy[i];

						if (adjacent_x < 0 || adjacent_x >= CHUNKS_WIDTH * CHUNK_SIZE || adjacent_y < 0 || adjacent_y >= CHUNKS_WIDTH * CHUNK_SIZE)
						{
							continue;
						}

						uint8_t adjacent_tile_t = get_write_tile_at(adjacent_x, adjacent_y);

						float adjacent_tile_temp = get_tile_temperature_at(adjacent_x, adjacent_y);
						if (abs(adjacent_tile_temp - tile_temp) > .25)
						{
							// TODO: Create tile temperature storage units + tile temperature transfer rates
							float temp_difference = (adjacent_tile_temp - tile_temp) / 50;
							float tile_conductivity = tile_thermal_conductivity[tile_t];
							float adjacent_tile_conductivity = tile_thermal_conductivity[adjacent_tile_t];

							float effective_conductivity = tile_conductivity == adjacent_tile_conductivity
															   ? tile_conductivity
															   : 1.0f / (1.0f / tile_conductivity + 1.0f / adjacent_tile_conductivity);
							temp_difference *= effective_conductivity;

							float tile_new_temp = tile_temp + temp_difference / tile_heat_capacity[tile_t];
							float adjacent_tile_new_temp = adjacent_tile_temp - temp_difference / tile_heat_capacity[adjacent_tile_t];

							if (tile_new_temp > tile_temp && tile_new_temp > adjacent_tile_temp						 // overheated tile
								|| tile_new_temp < tile_temp && tile_new_temp < adjacent_tile_temp					 // overcooled tile
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

					uint8_t current_tile_type = get_write_tile_at(x, y);

					switch (current_tile_type)
					{
					case AIR:
						if ((tick_count < 1000 && 1 == 2 && ((tick_count / 64) % 6 == 5 || (tick_count / 50) % 8 == 7)) && rand() % 1200 == 0)
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

						if (tick_count % 40 == 0)
						{
							uint16_t tile_misc = get_tile_misc_data_at(x, y);
							float tile_temp = get_tile_temperature_at(x, y);

							if (tile_misc == 0)
							{
								set_tile_at_no_lock(x, y, TEMPORARY_SMOKE);
								break;
							}
							else
							{
								bool success = try_place_tile_with_displacement_no_lock(x, y, POLLUTION, tile_temp, 0, 0, 16);
								if (success)
								{
									add_tile_misc_data_at(x, y, -1);
									add_tile_temperature(x, y, 100);
								}
							}
						}

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

						if (rand() % 2 == 0) // try spread
						{
							tile_type adjacent_tile = (tile_type)get_write_tile_at(x, y - 1);
							if (game_engine::in_set(adjacent_tile, WOOD, ROOT, LEAF))
							{
								set_tile_at_no_lock(x, y - 1, max_temp_tile_change[adjacent_tile]);
								break;
							}
							adjacent_tile = (tile_type)get_write_tile_at(x, y + 1);
							if (game_engine::in_set(adjacent_tile, WOOD, ROOT, LEAF))
							{
								set_tile_at_no_lock(x, y + 1, max_temp_tile_change[adjacent_tile]);
								break;
							}
							adjacent_tile = (tile_type)get_write_tile_at(x - 1, y);
							if (game_engine::in_set(adjacent_tile, WOOD, ROOT, LEAF))
							{
								set_tile_at_no_lock(x - 1, y, max_temp_tile_change[adjacent_tile]);
								break;
							}
							adjacent_tile = (tile_type)get_write_tile_at(x + 1, y);
							if (game_engine::in_set(adjacent_tile, WOOD, ROOT, LEAF))
							{
								set_tile_at_no_lock(x + 1, y, max_temp_tile_change[adjacent_tile]);
								break;
							}
						}
						break;

					case INSULATION_FOAM:
						if (get_tile_misc_data_at(x, y) == 0)
						{
							set_tile_temperature_at(x, y, tile_max_temperature[INSULATION_FOAM] + 3);
							break;
						}
						else
						{
							add_tile_misc_data_at(x, y, -1);
						}
						break;

					case SNOW:
						if (get_simple_tile_type(get_write_tile_at(x, y + 1)) == GAS)
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
						game::tile_simple_type simple_type = get_simple_tile_type(current_tile_type);

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
							else if (get_simple_tile_type(get_write_tile_at(x - 1, y)) == GAS)
							{
								// set_tile_at_no_lock(x, y, get_write_tile_at(x - 1, y));
								// set_tile_at_no_lock(x - 1, y, tile_type);
								switch_tiles_no_lock(x, y, x - 1, y);
							}
							else if (get_simple_tile_type(get_write_tile_at(x + 1, y)) == GAS)
							{
								// set_tile_at_no_lock(x, y, get_write_tile_at(x + 1, y));
								// set_tile_at_no_lock(x + 1, y, tile_type);
								switch_tiles_no_lock(x, y, x + 1, y);
							}
							else if (get_simple_tile_type(get_write_tile_at(x - 1, y)) == LIQUID)
							{
								if (get_write_tile_at(x - 1, y) < current_tile_type)
								{
									switch_tiles_no_lock(x, y, x - 1, y);
								}
							}
							else if (get_simple_tile_type(get_write_tile_at(x + 1, y)) == LIQUID)
							{
								if (get_write_tile_at(x + 1, y) < current_tile_type)
								{
									switch_tiles_no_lock(x, y, x + 1, y);
								}
							}
							else if (get_simple_tile_type(get_write_tile_at(x, y + 1)) == LIQUID)
							{
								if (get_write_tile_at(x, y + 1) < current_tile_type)
								{
									switch_tiles_no_lock(x, y, x, y + 1);
								}
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
					// case VACUUM:
					// 	break;
					case AIR:
						break;

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

					default:
						game::tile_simple_type simple_type = get_simple_tile_type(tile_type);

						switch (simple_type)
						{
						case GAS:

							if (get_simple_tile_type(get_write_tile_at(x, y - 1)) == GAS && get_write_tile_at(x, y - 1) < tile_type)
							{
								if (rand() % 3 && get_simple_tile_type(get_write_tile_at(x, y - 2)) == GAS && get_write_tile_at(x, y - 2) < tile_type)
								{
									switch_tiles_no_lock(x, y, x, y - 2);
								}
								else
								{
									switch_tiles_no_lock(x, y, x, y - 1);
								}
							}
							else if (get_simple_tile_type(get_write_tile_at(x - 1, y - 1)) == GAS && get_write_tile_at(x - 1, y - 1) < tile_type)
							{
								switch_tiles_no_lock(x, y, x - 1, y - 1);
							}
							else if (get_simple_tile_type(get_write_tile_at(x + 1, y - 1)) == GAS && get_write_tile_at(x + 1, y - 1) < tile_type)
							{
								switch_tiles_no_lock(x, y, x + 1, y - 1);
							}
							else if (get_simple_tile_type(get_write_tile_at(x + 1, y)) == GAS && get_write_tile_at(x + 1, y) < tile_type)
							{
								switch_tiles_no_lock(x, y, x + 1, y);
							}
							else if (get_simple_tile_type(get_write_tile_at(x - 1, y)) == GAS && get_write_tile_at(x - 1, y) < tile_type)
							{
								switch_tiles_no_lock(x, y, x - 1, y);
							}

							break;
						}
					}
				}
			}

			// game::b2d_mutex
			game::b2d_mutex.lock();
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
					uint16_t tile_misc = get_tile_misc_data_at(i, j);

					bool displace_tile = false;
					if (tile >= SOLID_TILE_START_INDEX && !is_solid_tile[tile])
					{
						displace_tile = true;
					}
					else if (velocity.y * velocity.y + velocity.x * velocity.x > 3 && tile >= LIQUID_TILE_START_INDEX && tile < SOLID_TILE_START_INDEX)
					{
						displace_tile = true;
					}

					if (displace_tile)
					{
						set_tile_at_no_lock(i, j, AIR);
						game_engine::task_scheduler_pointer->add_task({&create_single_debris_task, new create_debris_params(character_box.x + ((velocity.x > 0) ? character_box.w + 2 : -2), j - 1, velocity.x, -velocity.y, 0.5f, tile, AIR, tile, 250, tile_temp, tile_misc)});
						// game_engine::task_scheduler_pointer->add_task({&create_single_debris_task, new create_debris_params(character_box.x + ((velocity.x > 0) ? character_box.w + 2 : -2), j, (velocity.x > 0) ? 50 : -50, -10, 0.5f, SNOW, TEMPORARY_SMOKE)});

						// make the player_velocity * 0.9;
						velocity *= 0.99f;
					}
				}
			}
			character_body->SetLinearVelocity(b2Vec2(velocity.x, velocity.y * 0.9f));
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
						uint16_t misc_data = get_tile_misc_data_at(i, j);

						// Calculate the collision point on the circle
						b2Vec2 collision_point = b2Vec2(i, j);

						// Calculate the normal of the collision point
						b2Vec2 normal = collision_point - legged_creature_position;
						normal.Normalize();

						// Calculate the velocity of the collision point
						b2Vec2 collision_velocity = legged_creature_velocity; // + b2Cross(legged_creature_body->GetAngularVelocity(), collision_point - legged_creature_position);

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
							game_engine::task_scheduler_pointer->add_task({&create_single_debris_task, new create_debris_params(legged_creature_box.x + ((legged_creature_velocity.x > 0) ? legged_creature_box.w + 2 : -2), j - 1, debris_velocity.x, debris_velocity.y, 0.5f, tile, AIR, tile, 250, temperature, misc_data)});
						}
						else if (legged_creature_velocity.y * legged_creature_velocity.y + legged_creature_velocity.x * legged_creature_velocity.x > 3 && tile >= LIQUID_TILE_START_INDEX && tile < SOLID_TILE_START_INDEX)
						{
							set_tile_at_no_lock(i, j, AIR);
							game_engine::task_scheduler_pointer->add_task({&create_single_debris_task, new create_debris_params(legged_creature_box.x + ((legged_creature_velocity.x > 0) ? legged_creature_box.w + 2 : -2), j - 1, debris_velocity.x, debris_velocity.y, 0.5f, tile, AIR, tile, 250, temperature, misc_data)});
						}
					}
				}
			}
			// this, but for the flying creature system
			flying_creature_system *flying_creature_system_pointer = ((flying_creature_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<flying_creature_system>()));
			std::vector<entity> flying_creatures = flying_creature_system_pointer->get_entities();

			for (entity flying_creature : flying_creatures)
			{
				game_engine::box flying_creature_box = box_system_pointer->get(flying_creature).get_box();
				b2Body *flying_creature_body = box2d_system_pointer->get_dynamic_body(flying_creature);

				b2Vec2 flying_creature_velocity = flying_creature_body->GetLinearVelocity();

				b2Vec2 flying_creature_position = flying_creature_body->GetPosition();

				for (int i = (int)flying_creature_box.x; i < (int)flying_creature_box.x + flying_creature_box.w + 1; i++)
				{
					for (int j = (int)flying_creature_box.y; j < (int)flying_creature_box.y + flying_creature_box.h + 1; j++)
					{
						tile_type tile = (tile_type)get_write_tile_at(i, j);
						float temperature = get_tile_temperature_at(i, j);
						uint16_t misc_data = get_tile_misc_data_at(i, j);

						// Calculate the collision point on the circle
						b2Vec2 collision_point = b2Vec2(i, j);

						// Calculate the normal of the collision point
						b2Vec2 normal = collision_point - flying_creature_position;
						normal.Normalize();

						// Calculate the velocity of the collision point
						b2Vec2 collision_velocity = flying_creature_velocity; // + b2Cross(legged_creature_body->GetAngularVelocity(), collision_point - legged_creature_position);

						// Calculate the relative velocity in terms of the normal direction
						float normal_velocity = b2Dot(collision_velocity, normal);

						// Calculate the relative velocity in terms of the tangent direction
						b2Vec2 tangent = b2Vec2(-normal.y, normal.x);
						float tangent_velocity = b2Dot(collision_velocity, tangent);

						// debris_velocity
						b2Vec2 debris_velocity = normal_velocity * normal + tangent_velocity * tangent;

						bool displace_tile = false;
						if (tile >= SOLID_TILE_START_INDEX && !is_solid_tile[tile])
						{
							displace_tile = true;
						}
						else if (flying_creature_velocity.y * flying_creature_velocity.y + flying_creature_velocity.x * flying_creature_velocity.x > 3 && tile >= LIQUID_TILE_START_INDEX && tile < SOLID_TILE_START_INDEX)
						{
							displace_tile = true;
						}
						if (displace_tile)
						{
							set_tile_at_no_lock(i, j, AIR);
							game_engine::task_scheduler_pointer->add_task({&create_single_debris_task, new create_debris_params(flying_creature_box.x + ((flying_creature_velocity.x > 0) ? flying_creature_box.w + 2 : -2), j - 1, debris_velocity.x, debris_velocity.y, 0.5f, tile, AIR, tile, 250, temperature, misc_data)});

							flying_creature_body->SetLinearVelocity(b2Vec2(flying_creature_velocity.x * 0.59f, flying_creature_velocity.y * 0.59f));
						}
					}
				}
			}
			game::b2d_mutex.unlock();

			// copy modified chunks
			for (int i = 0; i < NUM_CHUNKS; i++)
			{
				// memcpy(tile_data_copy[i]->get_data(), tile_data_base[i]->get_data(), CHUNK_SIZE * CHUNK_SIZE);
				// memcpy(tile_data_copy[i]->get_temperature_data(), tile_data_base[i]->get_temperature_data(), CHUNK_SIZE * CHUNK_SIZE * sizeof(float));
				// memcpy(tile_data_copy[i]->get_misc_data(), tile_data_base[i]->get_misc_data(), CHUNK_SIZE * CHUNK_SIZE * sizeof(uint16_t));
			}

			// update textures
			// for (int chunk = 0; chunk < NUM_CHUNKS; chunk++)
			// {
			// 	entity chunk_ent = chunk_entities[chunk];
			// 	int chunk_x = chunk % CHUNKS_WIDTH;
			// 	int chunk_y = chunk / CHUNKS_WIDTH;
			// 	game_engine::render_system *render_system_pointer = ((game_engine::render_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::render_system>()));
			// 	render_system_pointer->update_texture_section(all_chunk_ent, (uint8_t *)(tile_data_copy[chunk]->get_data()), chunk_x * CHUNK_SIZE, chunk_y * CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
			// }
		}
		*/
	}

	void world_tile_system::generate_world()
	{
		// for (int chunk = 0; chunk < NUM_CHUNKS; chunk++)
		// {
		// 	tile_data_base[chunk]-> create_chunk(chunk % CHUNKS_WIDTH, chunk / CHUNKS_WIDTH);
		// 	chunk_entities[chunk] = game_engine::game_engine_pointer->create_entity();
		// }

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
		return chunk_entities_map[{x, y}];
	}

	chunk *world_tile_system::get_chunk(int chunk_x, int chunk_y)
	{
		if (!tile_data_map.count({chunk_x, chunk_y}))
		{
			return nullptr;
		}
		return tile_data_map[{chunk_x, chunk_y}];
	}

	std::vector<std::vector<std::pair<float, float>>> *world_tile_system::create_outlines(int x, int y)
	{
		std::vector<std::vector<std::pair<float, float>>> *outlines = new std::vector<std::vector<std::pair<float, float>>>;
		// std::shared_lock<std::shared_mutex> lock(chunk_mutex_copy);
		tile_data_map[{x, y}]->create_outlines(outlines);

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
			if (tile_data_map[{chunk_x, chunk_y}]->get_tile_copy(adjacent_x, adjacent_y) >= SOLID_TILE_START_INDEX)
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
		uint32_t num_deleted = 0;
		std::unordered_set<std::pair<int32_t, int32_t>, global_tile_pair_hash> affected_chunks;

		affected_chunks.insert(get_chunk_coords_from_world_coords(x, y));
		affected_chunks.insert(get_chunk_coords_from_world_coords(x + radius, y));
		affected_chunks.insert(get_chunk_coords_from_world_coords(x - radius, y));
		affected_chunks.insert(get_chunk_coords_from_world_coords(x, y + radius));
		affected_chunks.insert(get_chunk_coords_from_world_coords(x, y - radius));
		

		for (auto &[chunk_x, chunk_y] : affected_chunks)
		{
			chunk *target_chunk = get_chunk(chunk_x, chunk_y);
			if (target_chunk != nullptr)
			{
				uint32_t modified;
				modified = target_chunk->delete_circle(x, y, radius, tile_deny_list);
				num_deleted += modified;

				if (modified)
				{
					set_modified_chunk(chunk_x, chunk_y, 1);
				}
			}
		}
		// for (int i = 0; i < NUM_CHUNKS; i++)
		// {
		// 	uint32_t modified;
		// 	modified = tile_data_base[i]->delete_circle(x, y, radius, tile_deny_list);
		// 	num_deleted += modified;

		// 	if (modified)
		// 	{
		// 		set_modified_chunk(i % CHUNKS_WIDTH, i / CHUNKS_WIDTH, 1);
		// 	}
		// }
		return num_deleted;
	}

	uint32_t world_tile_system::explode_circle(int x, int y, int radius, float max_temperature, std::unordered_set<uint8_t> tile_deny_list)
	{
		uint32_t num_exploded = 0;
		std::pair<int32_t, int32_t> chunk_coords = get_chunk_coords_from_world_coords(x, y);
		std::pair<uint16_t, uint16_t> intra_chunk_coords = get_intra_chunk_coords_from_world_coords(x, y);
		
		chunk *target_chunk = get_chunk(chunk_coords.first, chunk_coords.second);
		if (target_chunk != nullptr)
		{
			uint32_t modified;
			modified = target_chunk->explode_circle(intra_chunk_coords.first, intra_chunk_coords.second, radius, max_temperature, tile_deny_list);
			num_exploded += modified;

			if (modified)
			{
				set_modified_chunk(chunk_coords.first, chunk_coords.second, 1);
			}
		}
		// for (int i = 0; i < NUM_CHUNKS; i++)
		// {
		// 	uint32_t modified;
		// 	modified = tile_data_base[i]->explode_circle(x, y, radius, max_temperature, tile_deny_list);
		// 	num_exploded += modified;

		// 	if (modified)
		// 	{
		// 		set_modified_chunk(i % CHUNKS_WIDTH, i / CHUNKS_WIDTH, 1);
		// 	}
		// }
		return num_exploded;
	}

	void world_tile_system::set_modified_chunk(int x, int y, uint8_t value)
	{
		modified_chunks_map[{x, y}] = value;
	}

	uint8_t world_tile_system::get_modified_chunk(int x, int y)
	{
		return modified_chunks_map[{x, y}];
	}

	std::string world_tile_system::to_csv_string()
	{
		std::string csv_string = "TODO: implement a to string again";
		// for (int y = 0; y < CHUNKS_WIDTH * CHUNK_SIZE; y++)
		// {
		// 	for (int x = 0; x < CHUNKS_WIDTH * CHUNK_SIZE; x++)
		// 	{
		// 		csv_string += std::to_string(get_write_tile_at(x, y));
		// 		if (x < CHUNKS_WIDTH * CHUNK_SIZE - 1)
		// 		{
		// 			csv_string += ",";
		// 		}
		// 	}
		// 	csv_string += "\n";
		// }

		return csv_string;
	}

	bool world_tile_system::find_tile_in_rect(std::pair<int, int> &result, int x, int y, int w, int h, std::unordered_set<uint8_t> tile_types)
	{
		for (auto &[chunk_coords, chunk_ptr] : tile_data_map)
		{
			if (chunk_ptr->find_tile_in_rect(result, x, y, w, h, tile_types))
			{
				return true;
			}
		}
		// for (int i = 0; i < NUM_CHUNKS; i++)
		// {
		// 	if (tile_data_base[i]->find_tile_in_rect(result, x, y, w, h, tile_types))
		// 	{
		// 		return true;
		// 	}
		// }
		return false;
	}

	bool update_chunk_tiles_task_wrapper(void *parameters)
	{
		update_chunk_tiles_parameters *params = (update_chunk_tiles_parameters *)parameters;
		world_tile_system *wts = params->world_tile_system_pointer;
		return wts->update_chunk_tiles_task(params);
	}

	bool world_tile_system::update_chunk_tiles_task(void *parameters)
	{
		update_chunk_tiles_parameters *params = (update_chunk_tiles_parameters *)parameters;
		chunk *c = params->chunk_pointer;

		uint8_t forwards_direction = params->tick_count % 2;
		int8_t heat_dx[] = {1, 0, -1, 0};
		int8_t heat_dy[] = {0, 1, 0, -1};

		// retrieve neighbour tile buffers
		int n_chunk_x;
		int n_chunk_y;
		// up, right, down, left chunks
		int8_t chunk_neighbour_dx[] = {0, 1, 0, -1};
		int8_t chunk_neighbour_dy[] = {-1, 0, 1, 0};
		std::array<chunk *, 4> neighbour_chunks;
		for (int i = 0; i < 4; i++)
		{
			n_chunk_x = c->chunk_x + chunk_neighbour_dx[i];
			n_chunk_y = c->chunk_y + chunk_neighbour_dy[i];
			if (tile_data_map.count({n_chunk_x, n_chunk_y}) == 0)
			{
				neighbour_chunks[i] = nullptr;
			}
			else
			{
				neighbour_chunks[i] = tile_data_map[{n_chunk_x, n_chunk_y}];
			}
		}
		c->clear_neighbour_tile_buffer();
		for (int i = 0; i < 4; i++)
		{
			if (neighbour_chunks[i] != nullptr)
			{
				neighbour_chunks[i]->get_neighbour_tile_buffer(c->get_neighbour_tile_buffer_pointer(), i);
				c->get_neighbour_tile_buffer_pointer()->side_filled[i] = true;
			}
		}

		chunk_neighbour_tile_buffer neighbour_tile_copy = *c->get_neighbour_tile_buffer_pointer();

		bool chunk_terrain_modified = false;

		c->lock_chunk();
		auto start_time = std::chrono::high_resolution_clock::now();
		// update temperatures
		for (int y = forwards_direction ? 0 : CHUNK_SIZE - 1; y < CHUNK_SIZE && y >= 0; y += forwards_direction ? 1 : -1)
		{
			for (int x = forwards_direction ? 0 : CHUNK_SIZE - 1; x < CHUNK_SIZE && x >= 0; x += forwards_direction ? 1 : -1)
			{
				tile_type tile = (tile_type)c->get_tile(x, y);
				float temp = c->get_tile_temperature(x, y);
				uint16_t misc = c->get_misc_data_at(x, y);

				// check if tile has overheated
				if (temp > tile_max_temperature[tile] + 2)
				{
					if (!max_temp_tile_change.count(tile))
					{
						// printf(" overheated tile %d at (%d, %d) is overheated with temperature %f, but no max temp change defined.\n", tile, x, y, temp);
						continue;
					}

					tile_type new_tile = max_temp_tile_change[tile];
					int changes = tile_phase_change_count_multiplier[new_tile];

					c->set_tile(x, y, AIR);

					if (changes > 0)
					{
						// need to place multiple tiles
						for (int i = 0; i < changes; i++)
						{
							bool success = c->try_place_tile_with_displacement_no_lock(x, y, new_tile, temp, misc, 0, 128);
							if (!success)
							{
								// couldn't place the tile, try role back
								for (int j = 0; j <= i; j++)
								{
									bool rollback_success = c->try_consume_nearby_tile_no_lock(x, y, new_tile, 128);
									if (!rollback_success)
									{
										// fucked it
										break;
									}
								}
								c->set_tile(x, y, tile);
								break;
							}
						}
					}
					else
					{
						bool consumed_all = true;
						// need to consume multiple tiles
						for (int i = 0; i < -changes; i++)
						{
							bool success = c->try_consume_nearby_tile_no_lock(x, y, tile, 128);
							if (!success)
							{
								consumed_all = false;
								// couldn't consume the tile, try role back
								for (int j = 0; j <= i; j++)
								{
									bool rollback_success = c->try_place_tile_with_displacement_no_lock(x, y, tile, temp, misc, 0, 128);
									if (!rollback_success)
									{
										printf("Failed to rollback tile consumption at (%d, %d) for tile %d\n", x, y, tile);
										// fucked it
										break;
									}
								}
								c->set_tile(x, y, tile);
								break;
							}
						}
						if (consumed_all)
						{
							c->set_tile(x, y, new_tile);
						}
					}
					if (is_solid_tile[new_tile] != is_solid_tile[tile] || (is_solid_tile[new_tile] || is_solid_tile[tile]) && changes != 1)
					{
						chunk_terrain_modified = true;
					}
				}
				else if (temp < tile_min_temperature[tile] - 2)
				{
					if (!min_temp_tile_change.count(tile))
					{
						// printf(" undercooled tile %d at (%d, %d) is undercooled with temperature %d, but no min temp change defined.\n", tile, x, y, temp);
						continue;
					}
					tile_type new_tile = min_temp_tile_change[tile];
					int changes = tile_phase_change_count_multiplier[new_tile];

					c->set_tile(x, y, AIR);

					if (changes > 0)
					{
						// need to place multiple tiles
						for (int i = 0; i < changes; i++)
						{
							bool success = c->try_place_tile_with_displacement_no_lock(x, y, new_tile, temp, misc, 0, 128);
							if (!success)
							{
								// couldn't place the tile, try role back
								for (int j = 0; j <= i; j++)
								{
									bool rollback_success = c->try_consume_nearby_tile_no_lock(x, y, new_tile, 128);
									if (!rollback_success)
									{
										// fucked it
										break;
									}
								}
								c->set_tile(x, y, tile);
								break;
							}
						}
					}
					else
					{
						bool consumed_all = true;
						// need to consume multiple tiles
						for (int i = 0; i < -changes; i++)
						{
							bool success = c->try_consume_nearby_tile_no_lock(x, y, tile, 128);
							if (!success)
							{
								consumed_all = false;
								// couldn't consume the tile, try role back
								for (int j = 0; j <= i; j++)
								{
									bool rollback_success = c->try_place_tile_with_displacement_no_lock(x, y, tile, temp, misc, 0, 128);
									if (!rollback_success)
									{
										// fucked it
										break;
									}
								}
								c->set_tile(x, y, tile);
								break;
							}
						}
						if (consumed_all)
						{
							c->set_tile(x, y, new_tile);
						}
					}
					if (is_solid_tile[new_tile] != is_solid_tile[tile] || (is_solid_tile[new_tile] || is_solid_tile[tile]) && changes != 1)
					{
						chunk_terrain_modified = true;
					}
				}

				// transfer heat to adjacent tiles

				for (int i = 0; i < 2; i++)
				{
					int nx = x + heat_dx[i + 2 * forwards_direction];
					int ny = y + heat_dy[i + 2 * forwards_direction];

					tile_type neighbor_tile = (tile_type)c->get_tile_using_buffer(nx, ny);
					float neighbor_temp = c->get_tile_temperature_using_buffer(nx, ny);
					if (neighbor_temp <= -280.0f)
					{
						continue;
					}

					if (neighbor_temp > temp)
					{
						float temp_difference = (neighbor_temp - temp) / 20;
						float tile_heat_conductivity = tile_thermal_conductivity[tile];
						float neighbor_tile_heat_conductivity = tile_thermal_conductivity[neighbor_tile];

						float effective_conductivity = tile_heat_conductivity == neighbor_tile_heat_conductivity ? tile_heat_conductivity
																												 : 1.0f / ((1.0f / tile_heat_conductivity) + (1.0f / neighbor_tile_heat_conductivity));

						temp_difference *= effective_conductivity;

						float tile_new_temp = temp + temp_difference / tile_heat_capacity[tile];
						float neighbor_tile_new_temp = neighbor_temp - temp_difference / tile_heat_capacity[neighbor_tile];

						if (tile_new_temp > neighbor_tile_new_temp)
						{
							temp_difference = (neighbor_temp - temp) / (1.0f / tile_heat_capacity[tile] + 1.0f / tile_heat_capacity[neighbor_tile]);
							tile_new_temp = temp + temp_difference / tile_heat_capacity[tile];
							neighbor_tile_new_temp = neighbor_temp - temp_difference / tile_heat_capacity[neighbor_tile];
						}
						c->set_tile_temperature(x, y, tile_new_temp);
						c->set_tile_temperature(nx, ny, neighbor_tile_new_temp);

						temp = tile_new_temp;
					}
				}
			}
		}

		auto mid_time = std::chrono::high_resolution_clock::now();
		c->reset_tile_moved_this_frame();
		auto mid2_time = std::chrono::high_resolution_clock::now();
		// printf("reset time: %lld us\n", std::chrono::duration_cast<std::chrono::microseconds>(mid2_time - mid_time).count());

		for (int y = CHUNK_SIZE - 1; y >= 0; y--)
		{
			for (int x = forwards_direction ? CHUNK_SIZE - 1 : 0; x < CHUNK_SIZE && x >= 0; x += forwards_direction ? -1 : 1)
			{
				if (c->get_tile_moved_this_frame(x, y))
					continue;
				tile_type tile = (tile_type)c->get_tile(x, y);
				float tile_temperature = c->get_tile_temperature(x, y);
				uint16_t tile_misc_data = c->get_misc_data_at(x, y);

				tile_type below_tile = (tile_type)(c->get_tile_using_buffer(x, y + 1));
				tile_type left_tile = (tile_type)(c->get_tile_using_buffer(x - 1, y));
				tile_type right_tile = (tile_type)(c->get_tile_using_buffer(x + 1, y));
				tile_type left_down_tile = (tile_type)(c->get_tile_using_buffer(x - 1, y + 1));
				tile_type right_down_tile = (tile_type)(c->get_tile_using_buffer(x + 1, y + 1));
				tile_type right_up_tile = (tile_type)(c->get_tile_using_buffer(x + 1, y - 1));
				tile_type left_up_tile = (tile_type)(c->get_tile_using_buffer(x - 1, y - 1));

				switch (tile)
				{
					// case AIR:
					// 	if (below_tile != AIR && get_simple_tile_type(below_tile) == GAS)
					// 	{
					// 		bool below_greater = tile < below_tile;
					// 		float below_tile_temperature = c->get_tile_temperature_using_buffer(x, y + 1);
					// 		if ((below_greater ? -1 : 1) * 10 + below_tile_temperature - tile_temperature >= 5)
					// 		{
					// 			c->switch_tiles_no_lock(x, y, x, y + 1);
					// 		}
					// 	}
					// 	break;

				case VACCUUM:
					if (left_tile == AIR || right_tile == AIR || below_tile == AIR)
					{
						c->set_tile(x, y, AIR);
					}
					break;

				case GRASS:
					if (right_tile == DIRT && get_simple_tile_type(right_up_tile) < SOLID && rand() % 50 == 0)
					{
						c->set_tile(x + 1, y, GRASS);
					}
					if (left_tile == DIRT && get_simple_tile_type(left_up_tile) < SOLID && rand() % 50 == 0)
					{
						c->set_tile(x - 1, y, GRASS);
					}
					if (right_down_tile == DIRT && get_simple_tile_type(right_tile) < SOLID && rand() % 50 == 0)
					{
						c->set_tile(x + 1, y + 1, GRASS);
					}
					if (left_down_tile == DIRT && get_simple_tile_type(left_tile) < SOLID && rand() % 50 == 0)
					{
						c->set_tile(x - 1, y + 1, GRASS);
					}
					if (y > 1 && right_up_tile == DIRT && get_simple_tile_type(c -> get_tile_using_buffer(x + 1, y - 2)) < SOLID && rand() % 50 == 0)
					{
						c->set_tile(x + 1, y - 1, GRASS);
					}
					if (y > 1 && left_up_tile == DIRT && get_simple_tile_type(c -> get_tile_using_buffer(x - 1, y - 2)) < SOLID && rand() % 50 == 0)
					{
						c->set_tile(x - 1, y - 1, GRASS);
					}
					break;
				case SAND:
					if (below_tile < SOLID_TILE_START_INDEX)
					{
						chunk_terrain_modified = true;
						c->switch_tiles_no_lock(x, y, x, y + 1);
					}
					else if (left_down_tile < SOLID_TILE_START_INDEX)
					{
						chunk_terrain_modified = true;
						c->switch_tiles_no_lock(x, y, x - 1, y + 1);
					}
					else if (right_down_tile < SOLID_TILE_START_INDEX)
					{
						chunk_terrain_modified = true;
						c->switch_tiles_no_lock(x, y, x + 1, y + 1);
					}
					break;
					case ASH:
					case SNOW:
					if (get_simple_tile_type(below_tile) < SOLID)
					{
						c->switch_tiles_no_lock(x, y, x, y + 1);
					}
					break;
				case EMBER:
					if (tile_misc_data == 0)
					{
						if (tile_temperature < 200.0f)
						{
							chunk_terrain_modified = true;
							c->set_tile(x, y, ASH);
						}
						break;
					}
					if (tile_misc_data % 20 == 0)
					{
						bool success = c->try_place_tile_with_displacement_no_lock(x, y - 1, POLLUTION, tile_temperature, 0, 0, 32);
						if (success)
						{
							c->add_tile_temperature(x, y, 750.0f);
							c->add_misc_data_at(x, y, -1);
						}
					}
					else
					{
						c->try_place_tile_with_displacement_no_lock(x, y - 1, TEMPORARY_SMOKE, tile_temperature, 0, 0, 32);
						c->add_misc_data_at(x, y, -1);
					}
					break;

				case INSULATION_FOAM:
					if (c->get_misc_data_at(x, y) > 0)
					{
						c->add_misc_data_at(x, y, -1);
					}
					else
					{
						set_tile_temperature_at(x, y, tile_max_temperature[INSULATION_FOAM] + 3);
					}
					break;

				default:
					tile_simple_type simple_tile = get_simple_tile_type(tile);

					switch (simple_tile)
					{
					case GAS:
						// if (game_engine::in_set(tile, AIR))
						// {
						// 	if (params -> tick_count > 100 && ((params -> tick_count / 64) % 6 == 5 || (params -> tick_count / 50) % 8 == 7) && rand() % 999 == 0)
						// 	{
						// 		c -> set_tile(x, y, SNOW);
						// 		break;
						// 	}
						// }

						if (below_tile != tile && get_simple_tile_type(below_tile) == GAS && c->get_tile_moved_this_frame(x, y + 1) == false)
						{
							bool below_lighter = tile > below_tile;
							float below_tile_temperature = c->get_tile_temperature_using_buffer(x, y + 1);
							if ((below_lighter ? 1 : -1) * 10 + below_tile_temperature - tile_temperature > 5)
							{
								c->switch_tiles_no_lock(x, y, x, y + 1);
							}
						}

						break;

					case LIQUID:
						// if (tile_temperature > tile_max_temperature[tile] * 0.9f)
						// {
						// 	c -> set_tile(x, y, VACCUUM);
						// 	c -> set_tile_temperature(x, y, 0);

						// 	game_engine::task_scheduler_pointer->add_task({&create_single_debris_task, new create_debris_params(x + c->chunk_x * CHUNK_SIZE, y + c->chunk_y * CHUNK_SIZE - 1, 0, -1, 0.5f, tile, AIR, tile, 250, tile_temperature, tile_misc_data)});

						// 	break;
						// }

						if (get_simple_tile_type(below_tile) < LIQUID)
						{
							c->switch_tiles_no_lock(x, y, x, y + 1);
						}
						else if (get_simple_tile_type(right_down_tile) < LIQUID && get_simple_tile_type(right_tile) < LIQUID)
						{
							c->switch_tiles_no_lock(x, y, x + 1, y + 1);
						}
						else if (get_simple_tile_type(left_down_tile) < LIQUID && get_simple_tile_type(left_tile) < LIQUID)
						{
							c->switch_tiles_no_lock(x, y, x - 1, y + 1);
						}
						else if (get_simple_tile_type(right_tile) < LIQUID)
						{
							c->switch_tiles_no_lock(x, y, x + 1, y);
						}
						else if (get_simple_tile_type(left_tile) < LIQUID)
						{
							c->switch_tiles_no_lock(x, y, x - 1, y);
						}
						else
						{
							// tile_type below_tile = (tile_type)(c -> get_tile_using_buffer(x, y + 1));
							if (get_simple_tile_type(below_tile) == LIQUID)
							{
								bool greater_tile = tile > below_tile;
								float below_tile_temperature = c->get_tile_temperature_using_buffer(x, y + 1);
								if (greater_tile * 10 + below_tile_temperature - tile_temperature >= 5)
								{
									c->switch_tiles_no_lock(x, y, x, y + 1);
									break;
								}
							}
							// tile_type left_down_tile = (tile_type)(c -> get_tile_using_buffer(x - 1, y + 1));
							if (get_simple_tile_type(left_down_tile) == LIQUID)
							{
								bool greater_tile = tile > left_down_tile;
								float left_down_tile_temperature = c->get_tile_temperature_using_buffer(x - 1, y + 1);
								if (greater_tile * 10 + left_down_tile_temperature - tile_temperature >= 5)
								{
									c->switch_tiles_no_lock(x, y, x - 1, y + 1);
									break;
								}
							}
							// tile_type right_down_tile = (tile_type)(c -> get_tile_using_buffer(x + 1, y + 1));
							if (get_simple_tile_type(right_down_tile) == LIQUID)
							{
								bool greater_tile = tile > right_down_tile;
								float right_down_tile_temperature = c->get_tile_temperature_using_buffer(x + 1, y + 1);
								if (greater_tile * 10 + right_down_tile_temperature - tile_temperature >= 5)
								{
									c->switch_tiles_no_lock(x, y, x + 1, y + 1);
									break;
								}
							}
						}
						break;
					}
				}
			}
		}

		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int x = forwards_direction ? 0 : CHUNK_SIZE - 1; x < CHUNK_SIZE && x >= 0; x += forwards_direction ? 1 : -1)
			{
				if (c->get_tile_moved_this_frame(x, y))
					continue;

				tile_type tile = (tile_type)c->get_tile_using_buffer(x, y);
				float tile_temperature = c->get_tile_temperature(x, y);

				tile_type above_tile = (tile_type)(c->get_tile_using_buffer(x, y - 1));
				tile_type left_tile = (tile_type)(c->get_tile_using_buffer(x - 1, y));
				tile_type right_tile = (tile_type)(c->get_tile_using_buffer(x + 1, y));
				tile_type below_tile = (tile_type)(c->get_tile_using_buffer(x, y + 1));
				tile_type left_up_tile = (tile_type)(c->get_tile_using_buffer(x - 1, y - 1));
				tile_type right_up_tile = (tile_type)(c->get_tile_using_buffer(x + 1, y - 1));

				switch (tile)
				{
				case VACCUUM:
					if (above_tile == AIR || left_tile == AIR || right_tile == AIR || below_tile == AIR)
					{
						c->set_tile(x, y, AIR);
					}
					break;
				case AIR:
					break;

				case TEMPORARY_SMOKE:
					if (rand() % 15 == 0)
					{
						c->set_tile(x, y, AIR);
						break;
					}

				default:
					tile_simple_type simple_tile = get_simple_tile_type(tile);

					switch (simple_tile)
					{
					case GAS:
						if (above_tile != tile && get_simple_tile_type(above_tile) == GAS) // && c -> get_tile_moved_this_frame(x, y - 1) == false)
						{
							bool above_lighter = tile > above_tile;
							float above_tile_temperature = c->get_tile_temperature_using_buffer(x, y - 1);

							if ((above_lighter ? -1 : 1) * 10 + tile_temperature - above_tile_temperature > 5)
							{
								c->switch_tiles_no_lock(x, y, x, y - 1);
								break;
							}
						}
						if (left_up_tile != tile && get_simple_tile_type(left_up_tile) == GAS && get_simple_tile_type(left_tile) == GAS) // && c -> get_tile_moved_this_frame(x - 1, y - 1) == false)
						{
							bool left_up_lighter = tile > left_up_tile;
							float left_up_tile_temperature = c->get_tile_temperature_using_buffer(x - 1, y - 1);

							if ((left_up_lighter ? -1 : 1) * 10 + tile_temperature - left_up_tile_temperature >= 5)
							{
								c->switch_tiles_no_lock(x, y, x - 1, y - 1);
								break;
							}
						}
						if (right_up_tile != tile && get_simple_tile_type(right_up_tile) == GAS && get_simple_tile_type(right_tile) == GAS) // && c -> get_tile_moved_this_frame(x + 1, y - 1) == false)
						{
							bool right_up_lighter = tile > right_up_tile;
							float right_up_tile_temperature = c->get_tile_temperature_using_buffer(x + 1, y - 1);

							if ((right_up_lighter ? -1 : 1) * 10 + tile_temperature - right_up_tile_temperature >= 5)
							{
								c->switch_tiles_no_lock(x, y, x + 1, y - 1);
								break;
							}
						}
						if (left_tile != tile && get_simple_tile_type(left_tile) == GAS) // && c -> get_tile_moved_this_frame(x - 1, y) == false)
						{
							bool left_lighter = tile > left_tile;
							float left_tile_temperature = c->get_tile_temperature_using_buffer(x - 1, y);

							if ((left_lighter ? -1 : 1) * 10 + tile_temperature - left_tile_temperature >= 5)
							{
								c->switch_tiles_no_lock(x, y, x - 1, y);
								break;
							}
						}
						if (right_tile != tile && get_simple_tile_type(right_tile) == GAS) // && c -> get_tile_moved_this_frame(x + 1, y) == false)
						{
							bool right_lighter = tile > right_tile;
							float right_tile_temperature = c->get_tile_temperature_using_buffer(x + 1, y);

							if ((right_lighter ? -1 : 1) * 10 + tile_temperature - right_tile_temperature >= 5)
							{
								c->switch_tiles_no_lock(x, y, x + 1, y);
								break;
							}
						}
						break;
					}
				}
			}
		}
		
		if (params -> is_player_chunk)
		{
			entity character_ent = game_engine::game_engine_pointer->player_entitiy;
			b2Body *character_body = ((box2d_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<box2d_system>()))->get_dynamic_body(character_ent);
			// b2d_mutex.lock();
			b2Vec2 velocity = character_body->GetLinearVelocity();
			// b2d_mutex.unlock();
			game_engine::box character_box = ((game_engine::box_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::box_system>()))->get(character_ent).get_box();

			for (int y = (int)character_box.y; y < (int)(character_box.y + character_box.h + 2); y++)
			{
				for (int x = (int)character_box.x - 1; x < (int)(character_box.x + character_box.w + 2); x++)
				{
					if (get_chunk_coords_from_world_coords(x, y) != std::pair<int32_t, int32_t>{c->chunk_x, c->chunk_y})
					{
						continue;
					}
					std::pair<uint16_t, uint16_t> intra_chunk_coords = get_intra_chunk_coords_from_world_coords(x, y);
					if (intra_chunk_coords.first >= 0 && intra_chunk_coords.first < CHUNK_SIZE && intra_chunk_coords.second >= 0 && intra_chunk_coords.second < CHUNK_SIZE)
					{
						tile_type tile = (tile_type)c->get_tile(intra_chunk_coords.first, intra_chunk_coords.second);
						
							
						bool displace_tile = false;
						if (tile >= SOLID_TILE_START_INDEX && !is_solid_tile[tile])
						{
							displace_tile = true;
						}
						else if (velocity.y * velocity.y + velocity.x * velocity.x > 3 && tile >= LIQUID_TILE_START_INDEX && tile < SOLID_TILE_START_INDEX)
						{
							displace_tile = true;
						}
						if (displace_tile)
						{
							c -> set_tile(intra_chunk_coords.first, intra_chunk_coords.second, VACCUUM);
							float tile_temp = c -> get_tile_temperature(intra_chunk_coords.first, intra_chunk_coords.second);
							uint16_t tile_misc = c -> get_misc_data_at(intra_chunk_coords.first, intra_chunk_coords.second);
							float debri_y = character_box.y - 2 > y ? character_box.y - 1 : y;
							float debri_x = character_box.x + (velocity.x > 0 ? character_box.w + 1 : -1);
							game_engine::task_scheduler_pointer->add_task({&create_single_debris_task, new create_debris_params(debri_x , debri_y, velocity.x * 0.1f, velocity.y * 0.1f, 0.5f, tile, VACCUUM, tile, 250, tile_temp, tile_misc)});
						}
					}
				}
			}
		}
		
		if (chunk_terrain_modified)
		{
			set_modified_chunk(c->chunk_x, c->chunk_y, 1);
		}

		c->unlock_chunk();



		c->lock_chunk_copy();
		c->lock_chunk_shared();

		c->copy_to_data_copy();

		c->unlock_chunk_shared();
		c->unlock_chunk_copy();

		auto end_time = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
		// printf("Chunk (%d, %d) update took %lld microseconds\n", c->chunk_x, c->chunk_y, duration.count());
		for (int i = 0; i < 4; i++)
		{
			if (neighbour_chunks[i] != nullptr)
			{
				neighbour_chunks[i]->update_neighbour_tiles(c->get_neighbour_tile_buffer_pointer(), &neighbour_tile_copy, i);
			}
		}

		// game_engine::render_system *render_system_pointer = ((game_engine::render_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::render_system>()));
		// c -> lock_chunk_copy_shared();
		// render_system_pointer->update_texture_section(all_chunk_ent, (uint8_t *)(c->get_data_copy()), c -> chunk_x * CHUNK_SIZE, c -> chunk_y * CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
		// c -> unlock_chunk_copy_shared();
		// render_system_pointer->add_task_to_scheduler({&update_chunk_tiles_texture_task, new update_chunk_tiles_texture_parameters{c, this, render_system_pointer}});

		return true;
	}

	bool update_chunk_tiles_texture_task(void *parameters)
	{
		update_chunk_tiles_texture_parameters *params = (update_chunk_tiles_texture_parameters *)parameters;
		chunk *c = params->chunk_pointer;
		world_tile_system *wts = params->world_tile_system_pointer;
		game_engine::render_system *render_system_pointer = params->render_system_pointer;

		c->lock_chunk_copy_shared();
		render_system_pointer->update_texture_section(wts->all_chunk_ent, (uint8_t *)(c->get_data_copy()), c->chunk_x * CHUNK_SIZE, c->chunk_y * CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
		c->unlock_chunk_copy_shared();

		return true;
	}

	bool recalculate_chunk_outlines_task(void *parameters)
	{
		recalculate_chunk_outlines_parameters *params = (recalculate_chunk_outlines_parameters *)parameters;
		chunk *c = params->chunk_pointer;
		world_tile_system *wts = params->world_tile_system_pointer;

		c->lock_chunk_shared();
		auto start_time = std::chrono::high_resolution_clock::now();
		if (wts->get_modified_chunk(c->chunk_x, c->chunk_y) == 0)
		{
			c->unlock_chunk_shared();
			return true;
		}
		c->unlock_chunk_shared();

		std::vector<std::vector<std::pair<float, float>>> *outlines = new std::vector<std::vector<std::pair<float, float>>>();
		c->create_outlines(outlines);
		wts->set_modified_chunk(c->chunk_x, c->chunk_y, 0);

		game::box2d_system *box2d_system_pointer = (game::box2d_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<game::box2d_system>());
		box2d_system_pointer->update_static_outlines(params->chunk_entity, outlines);
		auto end_time = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time);
		// printf("Chunk (%d, %d) outline recalculation took %lld microseconds\n", c->chunk_x, c->chunk_y, duration.count());
		return true;
	}

	bool generate_chunk_task_wrapper(void *parameters)
	{
		generate_chunk_parameters *params = (generate_chunk_parameters *)parameters;
		world_tile_system *wts = params->world_tile_system_pointer;
		return wts->generate_chunk_task(params);
	}

	bool world_tile_system::generate_chunk_task(void *parameters)
	{
		generate_chunk_parameters *params = (generate_chunk_parameters *)parameters;

		if (tile_data_map.count({params->chunk_x, params->chunk_y}) > 0)
		{
			// chunk already exists
			return true;
		}
		chunk *new_chunk = new chunk(params->chunk_x, params->chunk_y);

		new_chunk->create_chunk(params->chunk_x, params->chunk_y);
		entity chunk_entity = game_engine::game_engine_pointer->create_entity();
		tile_data_map[{params->chunk_x, params->chunk_y}] = new_chunk;
		chunk_entities_map[{params->chunk_x, params->chunk_y}] = chunk_entity;
		modified_chunks_map[{params->chunk_x, params->chunk_y}] = 1;

		return true;
	}

	bool world_tile_system::has_line_of_sight(int32_t x1, int32_t y1, int32_t x2, int32_t y2, std::unordered_set<tile_type> tile_deny_list)
	{
		float current_x_f = x1;
		float current_y_f = y1;

		float angle = atan2f((float)(y2 - y1), (float)(x2 - x1));
		float dx = cosf(angle);
		float dy = sinf(angle);
		float distance = sqrtf((float)((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1)));

		for (float d = 0; d < distance; d += 0.5f)
		{
			current_x_f = x1 + dx * d;
			current_y_f = y1 + dy * d;
			int32_t current_x = (int32_t)roundf(current_x_f);
			int32_t current_y = (int32_t)roundf(current_y_f);

			tile_type tile = (tile_type)get_tile_at(current_x, current_y);
			if (tile >= SOLID_TILE_START_INDEX && tile_deny_list.count(tile) == 0)
			{
				return false;
			}
		}
		return true;
	}

	std::pair<int32_t, int32_t> world_tile_system::get_chunk_coords_from_world_coords(int32_t x, int32_t y)
	{
		if (x < 0)
			x--;
		if (y < 0)
			y--;

		int32_t chunk_x = x / CHUNK_SIZE;
		int32_t chunk_y = y / CHUNK_SIZE;
		if (x < 0)
			chunk_x--;
		if (y < 0)
			chunk_y--;
		return {chunk_x, chunk_y};
	}

	std::pair<uint16_t, uint16_t> world_tile_system::get_intra_chunk_coords_from_world_coords(int32_t x, int32_t y)
	{
		if (x < 0)
			x--;
		if (y < 0)
			y--;

		int32_t intra_chunk_x = x % CHUNK_SIZE;
		int32_t intra_chunk_y = y % CHUNK_SIZE;
		if (intra_chunk_x < 0)
			intra_chunk_x += CHUNK_SIZE;
		if (intra_chunk_y < 0)
			intra_chunk_y += CHUNK_SIZE;
		return {intra_chunk_x, intra_chunk_y};
	}

}