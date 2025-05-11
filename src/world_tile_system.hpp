#pragma once

#include <array>
#include <shared_mutex>

#include "engine_comp.hpp"
#include "chunk.hpp"

namespace game
{
	// extern const uint16_t NUM_CHUNKS; // 3x3 chunks in world
	// extern const uint16_t CHUNKS_WIDTH;


	// coordinate pair with hash
	struct tile_coord
	{
		uint32_t x, y;
		tile_coord(uint32_t x, uint32_t y) : x(x), y(y) {}
		tile_coord() : x(0), y(0) {}
		bool operator==(const tile_coord &other) const
		{
			return x == other.x && y == other.y;
		}
	};
	// hash function for tile_coord
	struct tile_coord_hash
	{
		std::size_t operator()(const tile_coord &tc) const
		{
			return std::hash<uint32_t>()(tc.x) + std::hash<uint32_t>()(tc.y * CHUNK_SIZE * CHUNKS_WIDTH);
		}
	};
	

	struct world_tile_system : public game_engine::system
	{
	private:
		std::array<entity, NUM_CHUNKS> chunk_entities;
		std::array<chunk *, NUM_CHUNKS> tile_data_base{};
		std::array<uint8_t, game::NUM_CHUNKS> modified_chunks;
		std::array<chunk *, NUM_CHUNKS> tile_data_copy{};
		// std::array<uint8_t, game::NUM_CHUNKS>  modified_chunks_1;		
		
		std::shared_mutex chunk_mutex_base;
		std::shared_mutex chunk_mutex_copy;
		uint8_t get_write_tile_at(int x, int y);
		void set_tile_at_no_lock(int x, int y, uint8_t tile_type);

		

	public:
		entity all_chunk_ent;
		entity midground_tiles_ent;
		// std::mutex tile_mutex;

		world_tile_system();
		~world_tile_system();

		void set_all_chunk_ent(entity ent);
		uint8_t get_tile_at(int x, int y);
		void set_tile_at_with_lock(int x, int y, uint8_t tile_type);
		void set_tile_at_with_search_and_lock(int x, int y, uint8_t tile_type);
		void set_tile_copy_at(int x, int y, uint8_t tile_type);
		std::array<entity, NUM_CHUNKS> get_chunk_entities();
		void update(){};
		void update(uint64_t tick_count);
		void generate_world();
		entity get_chunk_entity(int x, int y);
		entity get_chunk_entity(int chunk);
		std::array<chunk *, NUM_CHUNKS> *get_chunks_copy();
		std::array<chunk *, NUM_CHUNKS> *get_chunks_base();
		std::array<std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE> *, NUM_CHUNKS> get_chunks_data();
		std::vector<std::vector<std::pair<float, float>>> *create_outlines(int x, int y);
		std::vector<std::vector<std::pair<float, float>>> * create_outline_from_custom_shape(std::vector<std::vector<tile_type>> tiles);
		uint32_t delete_circle(int x, int y, int radius, std::unordered_set<uint8_t> tyle_deny_list);
		std::array<uint8_t, game::NUM_CHUNKS> *get_modified_chunks();
		void set_modified_chunk(int x, int y, uint8_t value);
		bool find_tile_in_rect(std::pair<int, int> &result, int x, int y, int w, int h, std::unordered_set<uint8_t> tile_types);
		// std::pair<int, int> get_tile_in_rect(int x, int y, int w, int h, std::unordered_set<uint8_t> tile_types);
		std::string to_csv_string();
	};
}