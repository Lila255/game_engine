#include "world_tile_system.hpp"

namespace game
{
	// const uint16_t NUM_CHUNKS = 16; // 3x3 chunks in world
	// const uint16_t CHUNKS_WIDTH = 4;

	world_tile_system::world_tile_system()
	{
		for (int i = 0; i < NUM_CHUNKS; i++)
		{
			int chunk_x = i % CHUNKS_WIDTH;
			int chunk_y = i / CHUNKS_WIDTH;
			chunk_data[i] = new chunk(chunk_x, chunk_y);
			// chunk_data[i] = new std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE>{};
			modified_chunks[chunk_y][chunk_x] = 1;
		}
	}

	world_tile_system::~world_tile_system()
	{
		for (int i = 0; i < NUM_CHUNKS; i++)
		{
			delete chunk_data[i];
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
		return (chunk_data[chunk])->get_tile(tile_x, tile_y);
	}

	void world_tile_system::set_tile_at(int x, int y, uint8_t tile_type)
	{
		int chunk_x = x / CHUNK_SIZE;
		int chunk_y = y / CHUNK_SIZE;
		int chunk = chunk_x + chunk_y * CHUNKS_WIDTH;
		int tile_x = x % CHUNK_SIZE;
		int tile_y = y % CHUNK_SIZE;
		if (chunk_x < 0 || chunk_x >= CHUNKS_WIDTH || chunk_y < 0 || chunk_y >= CHUNKS_WIDTH)
			return;

		(chunk_data[chunk])->set_tile(tile_x, tile_y, tile_type);
	}

	std::array<entity, NUM_CHUNKS> world_tile_system::get_chunk_entities()
	{
		return chunk_entities;
	}

	void world_tile_system::update(uint64_t tick_count, std::array<std::array<uint8_t, CHUNKS_WIDTH>, CHUNKS_WIDTH> *modified_chunks)
	{
		tile_mutex.lock();
		uint8_t direction = tick_count % 2;
		for (int y = CHUNKS_WIDTH * CHUNK_SIZE - 1; y >= 0; y--)
		{
			int chunk_y = y / CHUNK_SIZE;

			for (int x = direction ? 0 : CHUNKS_WIDTH * CHUNK_SIZE - 1; x < CHUNKS_WIDTH * CHUNK_SIZE && x >= 0; x += direction ? 1 : -1)
			{
				int chunk_x = x / CHUNK_SIZE;
				int chunk = chunk_x + chunk_y * CHUNKS_WIDTH;
				int tile_x = x % CHUNK_SIZE;
				int tile_y = y % CHUNK_SIZE;

				uint8_t tile_type = get_tile_at(x, y);
				switch (tile_type)
				{
				// case SMOKE:
				// 	if (get_tile_at(x, y-1) == AIR)
				// 	{
				// 		set_tile_at(x, y-1, SMOKE);
				// 		set_tile_at(x, y, AIR);
				// 		break;
				// 	}
				// 	if (get_tile_at(x-1, y-1) == AIR)
				// 	{
				// 		set_tile_at(x-1, y-1, SMOKE);
				// 		set_tile_at(x, y, AIR);
				// 		break;
				// 	}
				// 	if (get_tile_at(x+1, y-1) == AIR)
				// 	{
				// 		set_tile_at(x+1, y-1, SMOKE);
				// 		set_tile_at(x, y, AIR);
				// 		break;
				// 	}
				// 	break;

				case WATER:
					if (game_engine::in_set(get_tile_at(x, y + 1), AIR, SMOKE, TEMPORARY_SMOKE))
					{
						set_tile_at(x, y, get_tile_at(x, y + 1));
						set_tile_at(x, y + 1, WATER);
						break;
					}
					if (game_engine::in_set(get_tile_at(x - 1, y + 1), AIR, SMOKE, TEMPORARY_SMOKE))
					{
						set_tile_at(x, y, get_tile_at(x - 1, y + 1));
						set_tile_at(x - 1, y + 1, WATER);
						break;
					}
					if (game_engine::in_set(get_tile_at(x + 1, y + 1), AIR, SMOKE, TEMPORARY_SMOKE))
					{
						set_tile_at(x, y, get_tile_at(x + 1, y + 1));
						set_tile_at(x + 1, y + 1, WATER);
						break;
					}
					if (game_engine::in_set(get_tile_at(x - 1, y), AIR, SMOKE, TEMPORARY_SMOKE))
					{
						set_tile_at(x, y, get_tile_at(x - 1, y));
						set_tile_at(x - 1, y, WATER);
						break;
					}
					if (game_engine::in_set(get_tile_at(x + 1, y), AIR, SMOKE, TEMPORARY_SMOKE))
					{
						set_tile_at(x, y, get_tile_at(x + 1, y));
						set_tile_at(x + 1, y, WATER);
						break;
					}
					break;
				case ACID:
					if (rand() % 10 == 0 && get_tile_at(x, y + 1) >= GLASS && get_tile_at(x, y + 1) < BEDROCK) {
						set_tile_at(x, y + 1, AIR);
						set_tile_at(x, y, AIR);
						(*modified_chunks)[chunk_y][chunk_x] = 1;
						break;
					}

					if (game_engine::in_set(get_tile_at(x, y + 1), AIR, SMOKE, TEMPORARY_SMOKE))
					{
						set_tile_at(x, y, get_tile_at(x, y + 1));
						set_tile_at(x, y + 1, ACID);
						break;
					}
					if (game_engine::in_set(get_tile_at(x - 1, y + 1), AIR, SMOKE, TEMPORARY_SMOKE))
					{
						set_tile_at(x, y, get_tile_at(x - 1, y + 1));
						set_tile_at(x - 1, y + 1, ACID);
						break;
					}
					if (game_engine::in_set(get_tile_at(x + 1, y + 1), AIR, SMOKE, TEMPORARY_SMOKE))
					{
						set_tile_at(x, y, get_tile_at(x + 1, y + 1));
						set_tile_at(x + 1, y + 1, ACID);
						break;
					}
					if (game_engine::in_set(get_tile_at(x - 1, y), AIR, SMOKE, TEMPORARY_SMOKE))
					{
						set_tile_at(x, y, get_tile_at(x - 1, y));
						set_tile_at(x - 1, y, ACID);
						break;
					}
					if (game_engine::in_set(get_tile_at(x + 1, y), AIR, SMOKE, TEMPORARY_SMOKE))
					{
						set_tile_at(x, y, get_tile_at(x + 1, y));
						set_tile_at(x + 1, y, ACID);
						break;
					}
					if (rand() % 3 == 0 && get_tile_at(x, y + 1) >= GLASS && get_tile_at(x, y + 1) < BEDROCK)
					{
						set_tile_at(x, y + 1, AIR);
						set_tile_at(x, y, AIR);
						(*modified_chunks)[chunk_y][chunk_x] = 1;
						break;
					}
					if (rand() % 12 == 0 && get_tile_at(x - 1, y) >= GLASS && get_tile_at(x - 1, y) < BEDROCK)
					{
						set_tile_at(x - 1, y, AIR);
						set_tile_at(x, y, AIR);
						(*modified_chunks)[chunk_y][chunk_x] = 1;
						break;
					}
					if (rand() % 12 == 0 && get_tile_at(x + 1, y) >= GLASS && get_tile_at(x + 1, y) < BEDROCK)
					{
						set_tile_at(x + 1, y, AIR);
						set_tile_at(x, y, AIR);
						(*modified_chunks)[chunk_y][chunk_x] = 1;
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
							if (get_tile_at(xx, yy) == DIRT && get_tile_at(xx, yy - 1) == AIR)
							{
								set_tile_at(xx, yy, GRASS);
								break;
							}
						}
					}
					// if (get_tile_at(x, y - 1) == AIR && rand() % 100 == 0)
					// {
					// 	set_tile_at(x, y, DIRT);
					// }
					break;

				case SAND:
					if (game_engine::in_set(get_tile_at(x, y + 1), AIR, SMOKE, WATER, TEMPORARY_SMOKE))
					{
						set_tile_at(x, y, get_tile_at(x, y + 1));
						set_tile_at(x, y + 1, SAND);
						(*modified_chunks)[chunk_y][chunk_x] = 1;
					}
					else if (game_engine::in_set(get_tile_at(x - 1, y + 1), AIR, SMOKE, WATER, TEMPORARY_SMOKE))
					{
						set_tile_at(x, y, get_tile_at(x - 1, y + 1));
						set_tile_at(x - 1, y + 1, SAND);
						(*modified_chunks)[chunk_y][chunk_x] = 1;
					}
					else if (game_engine::in_set(get_tile_at(x + 1, y + 1), AIR, SMOKE, WATER, TEMPORARY_SMOKE))
					{
						set_tile_at(x, y, get_tile_at(x + 1, y + 1));
						set_tile_at(x + 1, y + 1, SAND);
						(*modified_chunks)[chunk_y][chunk_x] = 1;
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

				uint8_t tile_type = get_tile_at(x, y);
				switch (tile_type)
				{
				case SMOKE:
					if (game_engine::in_set(get_tile_at(x, y - 1), AIR, WATER))
					{
						set_tile_at(x, y, get_tile_at(x, y - 1));
						set_tile_at(x, y - 1, SMOKE);
					}
					else if (game_engine::in_set(get_tile_at(x - 1, y - 1), AIR, WATER))
					{
						set_tile_at(x, y, get_tile_at(x - 1, y - 1));
						set_tile_at(x - 1, y - 1, SMOKE);
					}
					else if (game_engine::in_set(get_tile_at(x + 1, y - 1), AIR, WATER))
					{
						set_tile_at(x, y, get_tile_at(x + 1, y - 1));
						set_tile_at(x + 1, y - 1, SMOKE);
					}
					else if (game_engine::in_set(get_tile_at(x - 1, y), AIR, WATER))
					{
						set_tile_at(x, y, get_tile_at(x - 1, y));
						set_tile_at(x - 1, y, SMOKE);
					}
					else if (game_engine::in_set(get_tile_at(x + 1, y), AIR, WATER))
					{
						set_tile_at(x, y, get_tile_at(x + 1, y));
						set_tile_at(x + 1, y, SMOKE);
					}
					break;
				case TEMPORARY_SMOKE:
					if(rand()%5 == 0)
					{
						set_tile_at(x,y,AIR);
					}
					else if (game_engine::in_set(get_tile_at(x, y - 1), AIR, WATER))
					{
						set_tile_at(x, y, get_tile_at(x, y - 1));
						set_tile_at(x, y - 1, TEMPORARY_SMOKE);
					}
					else if (game_engine::in_set(get_tile_at(x - 1, y - 1), AIR, WATER))
					{
						set_tile_at(x, y, get_tile_at(x - 1, y - 1));
						set_tile_at(x - 1, y - 1, TEMPORARY_SMOKE);
					}
					else if (game_engine::in_set(get_tile_at(x + 1, y - 1), AIR, WATER))
					{
						set_tile_at(x, y, get_tile_at(x + 1, y - 1));
						set_tile_at(x + 1, y - 1, TEMPORARY_SMOKE);
					}
					else if (game_engine::in_set(get_tile_at(x - 1, y), AIR, WATER))
					{
						set_tile_at(x, y, get_tile_at(x - 1, y));
						set_tile_at(x - 1, y, TEMPORARY_SMOKE);
					}
					else if (game_engine::in_set(get_tile_at(x + 1, y), AIR, WATER))
					{
						set_tile_at(x, y, get_tile_at(x + 1, y));
						set_tile_at(x + 1, y, TEMPORARY_SMOKE);
					}
					break;
				}
			}
		}

		// update textures
		for (int chunk = 0; chunk < NUM_CHUNKS; chunk++)
		{
			entity chunk_ent = chunk_entities[chunk];
			int chunk_x = chunk % CHUNKS_WIDTH;
			int chunk_y = chunk / CHUNKS_WIDTH;
			game_engine::render_system *render_system_pointer = ((game_engine::render_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::render_system>()));
			render_system_pointer->update_texture_section(all_chunk_ent, (uint8_t *)(chunk_data[chunk]->get_data()), chunk_x * CHUNK_SIZE, chunk_y * CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
		}
		tile_mutex.unlock();
	}

	void world_tile_system::generate_world()
	{
		for (int chunk = 0; chunk < NUM_CHUNKS; chunk++)
		{
			chunk_data[chunk]->create_chunk();
			chunk_entities[chunk] = game_engine::game_engine_pointer->create_entity();
		}
		// create solid border around world
		for (int x = 0; x < CHUNKS_WIDTH * CHUNK_SIZE; x++)
		{
			set_tile_at(x, 0, BEDROCK);
			set_tile_at(x, CHUNKS_WIDTH * CHUNK_SIZE - 1, BEDROCK);
		}
		for (int y = 0; y < CHUNKS_WIDTH * CHUNK_SIZE; y++)
		{
			set_tile_at(0, y, BEDROCK);
			set_tile_at(CHUNKS_WIDTH * CHUNK_SIZE - 1, y, BEDROCK);
		}
	}

	entity world_tile_system::get_chunk_entity(int x, int y)
	{
		return chunk_entities[x + y * CHUNKS_WIDTH];
	}

	entity world_tile_system::get_chunk_entity(int chunk)
	{
		return chunk_entities[chunk];
	}
	
	std::array<chunk *, NUM_CHUNKS> *world_tile_system::get_chunks()
	{
		return &chunk_data;
	}

	std::array<std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE> *, NUM_CHUNKS> world_tile_system::get_chunks_data()
	{
		std::array<std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE> *, NUM_CHUNKS> chunks_data;
		for (int chunk = 0; chunk < NUM_CHUNKS; chunk++)
		{
			// chunk_data[chunk]->create_texture_from_chunk(textures[chunk]);
			chunks_data[chunk] = chunk_data[chunk]->get_data();
		}
		return chunks_data;
	}

	std::vector<std::vector<std::pair<float, float>>> *world_tile_system::create_outlines(int x, int y)
	{
		std::vector<std::vector<std::pair<float, float>>> *outlines = new std::vector<std::vector<std::pair<float, float>>>;
		chunk_data[x + y * CHUNKS_WIDTH]->create_outlines(outlines);
		return outlines;
	}

	void world_tile_system::delete_circle(int x, int y, int radius)
		{
			for (int i = 0; i < NUM_CHUNKS; i++)
			{
				bool modified = chunk_data[i]->delete_circle(x, y, radius);
				if (modified)
				{
					modified_chunks[i / CHUNKS_WIDTH][i % CHUNKS_WIDTH] = 1;
				}
			}
		}
}