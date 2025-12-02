#pragma once

#include <array>
#include <shared_mutex>

#include "engine_comp.hpp"
#include "chunk.hpp"

namespace game
{
	// extern const uint16_t NUM_CHUNKS; // 3x3 chunks in world
	// extern const uint16_t CHUNKS_WIDTH;

	extern std::array<std::vector<tile_linef>, 16> edges_lines;

	// coordinate pair with hash
	struct tile_coord
	{
		int32_t x, y;
		tile_coord(int32_t x, int32_t y) : x(x), y(y) {}
		tile_coord() : x(0), y(0) {}
		bool operator==(const tile_coord &other) const
		{
			return x == other.x && y == other.y;
		}
	};

	struct intra_chunk_tile_coord_hash
	{
		std::size_t operator()(const tile_coord &p) const
		{
			return std::hash<uint16_t>()(p.x + p.y * CHUNK_SIZE);
		}
	};
	struct intra_chunk_pair_coord_hash
	{
		std::size_t operator()(const std::pair<uint16_t, uint16_t> &p) const
		{
			return std::hash<uint16_t>()(p.first + p.second * CHUNK_SIZE);
		}
	};

	struct global_tile_coord_hash
	{
		std::size_t operator()(const tile_coord &p) const
		{
			return std::hash<int32_t>()(p.x) ^ std::hash<int32_t>()(p.y * 73856093);
		}
	};
	struct global_tile_pair_hash
	{
		std::size_t operator()(const std::pair<int32_t, int32_t> &p) const
		{
			return std::hash<int32_t>()(p.first) ^ std::hash<int32_t>()(p.second * 73856093);
		}
	};

	struct world_tile_system : public game_engine::system
	{
	private:
		// std::array<entity, NUM_CHUNKS> chunk_entities;
		// std::array<chunk *, NUM_CHUNKS> tile_data_base;
		// std::array<uint8_t, game::NUM_CHUNKS> modified_chunks;

		std::unordered_map<std::pair<int32_t, int32_t>, entity, chunk_coord_hash> chunk_entities_map;
		std::unordered_map<std::pair<int32_t, int32_t>, chunk *, chunk_coord_hash> tile_data_map;
		std::unordered_map<std::pair<int32_t, int32_t>, uint8_t, chunk_coord_hash> modified_chunks_map;
		

		// std::array<chunk *, NUM_CHUNKS> tile_data_copy;
		// std::array<uint8_t, game::NUM_CHUNKS>  modified_chunks_1;

		// std::shared_mutex chunk_mutex_base;
		// std::shared_mutex chunk_mutex_copy;

		std::thread task_scheduler_thread;

		void set_tile_at_no_lock(int x, int y, uint8_t tile_type);


	public:
		uint8_t get_write_tile_at(int x, int y);
		entity all_chunk_ent;
		entity midground_tiles_ent;
		// std::mutex tile_mutex;

		world_tile_system();
		~world_tile_system();

		void set_all_chunk_ent(entity ent);
		uint8_t get_tile_at(int x, int y);

		float get_tile_temperature_at(int x, int y);
		void set_tile_temperature_at(int x, int y, float temperature);
		void add_tile_temperature(int x, int y, float temperature);

		uint16_t get_tile_misc_data_at(int x, int y);
		void set_tile_misc_data_at(int x, int y, uint16_t value);
		void add_tile_misc_data_at(int x, int y, int16_t value);

		void set_tile_at_with_lock(int x, int y, uint8_t tile_type);
		void set_tile_at_with_search_and_lock(int x, int y, uint8_t tile_type);
		void set_tile_copy_at(int x, int y, uint8_t tile_type);
		void switch_tiles_no_lock(int x1, int y1, int x2, int y2);
		void switch_tiles_with_lock(int x1, int y1, int x2, int y2);
		bool try_consume_nearby_tile_no_lock(int x, int y, tile_type tile_type);
		bool try_place_tile_with_displacement_no_lock(int x, int y, tile_type tile_type, float temperature, uint16_t misc_data, int recursion_depth, int search_size);
		bool try_place_tile_flush_with_displacement_no_lock(int x, int y, tile_type tile_type, float temperature, uint16_t misc_data, int recursion_depth, int search_size);
		bool has_line_of_sight(int32_t x1, int32_t y1, int32_t x2, int32_t y2, std::unordered_set<tile_type> tile_deny_list);

		void update() {};
		void update(uint64_t tick_count);
		void generate_world();
		entity get_chunk_entity(int x, int y);
		entity get_chunk_entity(int chunk);
		std::pair<int32_t, int32_t> get_chunk_coords_from_world_coords(int x, int y);
		std::pair<uint16_t, uint16_t> get_intra_chunk_coords_from_world_coords(int x, int y);
		chunk *get_chunk(int chunk_x, int chunk_y);
		uint16_t get_tile_edginess(int chunk_x, int chunk_y, int x, int y);
		std::vector<std::vector<std::pair<float, float>>> *create_outlines(int x, int y);
		std::vector<std::vector<std::pair<float, float>>> *create_outlines_from_chunk(int x, int y);
		std::vector<std::vector<std::pair<float, float>>> *create_outline_from_custom_shape(std::vector<std::vector<tile_type>> tiles);
		uint32_t delete_circle(int x, int y, int radius, std::unordered_set<uint8_t> tyle_deny_list);
		uint32_t explode_circle(int x, int y, int radius, float max_temperature, std::unordered_set<uint8_t> tile_deny_list);
		void set_modified_chunk(int x, int y, uint8_t value);
		uint8_t get_modified_chunk(int x, int y);
		bool find_tile_in_rect(std::pair<int, int> &result, int x, int y, int w, int h, std::unordered_set<uint8_t> tile_types);
		// std::pair<int, int> get_tile_in_rect(int x, int y, int w, int h, std::unordered_set<uint8_t> tile_types);
		std::string to_csv_string();
		

		// // get chunk mutex for writing
		// std::shared_mutex *get_chunk_mutex_base() { return &chunk_mutex_base; }

		game_engine::task_scheduler *task_scheduler_pointer;
		bool update_chunk_tiles_task(void *parameters);
		bool generate_chunk_task(void *parameters);
	};

	struct update_chunk_tiles_parameters
	{
		chunk *chunk_pointer;
		world_tile_system *world_tile_system_pointer;
		uint64_t tick_count;
		bool is_player_chunk;
	};

	bool update_chunk_tiles_task_wrapper(void *parameters);

	struct update_chunk_tiles_texture_parameters
	{
		chunk *chunk_pointer;
		world_tile_system *world_tile_system_pointer;
		game_engine::render_system *render_system_pointer;
		update_chunk_tiles_texture_parameters(chunk *c, world_tile_system *wts, game_engine::render_system *rs)
			: chunk_pointer(c), world_tile_system_pointer(wts), render_system_pointer(rs) {}
	};

	bool update_chunk_tiles_texture_task(void *parameters);

	struct recalculate_chunk_outlines_parameters
	{
		chunk *chunk_pointer;
		world_tile_system *world_tile_system_pointer;
		entity chunk_entity;
		recalculate_chunk_outlines_parameters(chunk *c, world_tile_system *wts, entity ent)
			: chunk_pointer(c), world_tile_system_pointer(wts), chunk_entity(ent) {}
	};

	bool recalculate_chunk_outlines_task(void *parameters);

	struct generate_chunk_parameters
	{
		int32_t chunk_x;
		int32_t chunk_y;
		world_tile_system *world_tile_system_pointer;
		generate_chunk_parameters(int32_t x, int32_t y, world_tile_system *wts)
			: chunk_x(x), chunk_y(y), world_tile_system_pointer(wts) {}
	};
	bool generate_chunk_task_wrapper(void *parameters);
}