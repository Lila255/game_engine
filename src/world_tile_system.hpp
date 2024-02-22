#pragma once

#include "engine_comp.hpp"
#include "chunk.hpp"

#include <array>

namespace game
{
	// extern const uint16_t NUM_CHUNKS; // 3x3 chunks in world
	// extern const uint16_t CHUNKS_WIDTH;

	const uint16_t NUM_CHUNKS = 16; 
	const uint16_t CHUNKS_WIDTH = 4;

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
		std::array<chunk *, NUM_CHUNKS> chunk_data{};
		std::array<entity, NUM_CHUNKS> chunk_entities;

	public:
		std::array<std::array<uint8_t, game::CHUNKS_WIDTH>, game::CHUNKS_WIDTH> modified_chunks;
		entity all_chunk_ent;
		std::mutex tile_mutex;

		world_tile_system();
		~world_tile_system();

		void set_all_chunk_ent(entity ent);
		uint8_t get_tile_at(int x, int y);
		void set_tile_at(int x, int y, uint8_t tile_type);
		std::array<entity, NUM_CHUNKS> get_chunk_entities();
		void update(){};
		void update(uint64_t tick_count, std::array<std::array<uint8_t, CHUNKS_WIDTH>, CHUNKS_WIDTH> * modified_chunks);
		void generate_world();
		entity get_chunk_entity(int x, int y);
		entity get_chunk_entity(int chunk);
		std::array<chunk *, NUM_CHUNKS> *get_chunks();
		std::array<std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE> *, NUM_CHUNKS> get_chunks_data();
		std::vector<std::vector<std::pair<float, float>>> *create_outlines(int x, int y);
		void delete_circle(int x, int y, int radius);
	};
}