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

	struct world_tile_system : public game_engine::system
	{
	private:
		std::array<chunk *, NUM_CHUNKS> chunk_data{};
		std::array<entity, NUM_CHUNKS> chunk_entities;

	public:
		std::array<std::array<uint8_t, game::CHUNKS_WIDTH>, game::CHUNKS_WIDTH> modified_chunks;
		entity all_chunk_ent;

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