#include "world_tile_system.hpp"
#include "box2d_system.hpp"
#include "tasks.hpp"

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
			chunk_data_0[i] = new chunk(chunk_x, chunk_y);
			chunk_data_1[i] = new chunk(chunk_x, chunk_y);
			// chunk_data[i] = new std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE>{};
			set_modified_chunk(chunk_x, chunk_y, 1);
		}
		
		std::fill(is_solid_tile.begin(), is_solid_tile.end(), 0);
		std::fill(is_solid_tile.begin() + tile_type::GLASS, is_solid_tile.begin() + tile_type::SOLID_63 + 1, 1);
		is_solid_tile[SNOW] = 0;
		is_solid_tile[BEDROCK] = 1;
	}

	world_tile_system::~world_tile_system()
	{
		for (int i = 0; i < NUM_CHUNKS; i++)
		{
			delete chunk_data_0[i];
			delete chunk_data_1[i];
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
		std::shared_lock<std::shared_mutex> lock(read_chunk_mutex);
		if (read_buffer == 0)
		{
			return (chunk_data_0[chunk])->get_tile(tile_x, tile_y);
		}
		else
		{
			return (chunk_data_1[chunk])->get_tile(tile_x, tile_y);
		}
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
		if (read_buffer == 1)
		{
			return (chunk_data_0[chunk])->get_tile(tile_x, tile_y);
		}
		else
		{
			return (chunk_data_1[chunk])->get_tile(tile_x, tile_y);
		}
	}

	void world_tile_system::set_tile_at_with_lock(int x, int y, uint8_t tile_type)
	{
		int chunk_x = x / CHUNK_SIZE;
		int chunk_y = y / CHUNK_SIZE;
		int chunk = chunk_x + chunk_y * CHUNKS_WIDTH;
		int tile_x = x % CHUNK_SIZE;
		int tile_y = y % CHUNK_SIZE;
		if (chunk_x < 0 || chunk_x >= CHUNKS_WIDTH || chunk_y < 0 || chunk_y >= CHUNKS_WIDTH)
			return;
		

		std::unique_lock<std::shared_mutex> lock(write_chunk_mutex);
		if(get_tile_at(x, y) == BEDROCK)
			return;
		
		if(is_solid_tile[tile_type] != is_solid_tile[get_tile_at(x, y)])
		{
			set_modified_chunk(chunk_x, chunk_y, 1);
		}

		if (read_buffer == 0)
		{
			(chunk_data_1[chunk])->set_tile(tile_x, tile_y, tile_type);
		}
		else
		{
			(chunk_data_0[chunk])->set_tile(tile_x, tile_y, tile_type);
		}
	}

	void world_tile_system::set_tile_at_with_search_and_lock(int x, int y, uint8_t tile_type)
	{
 		int chunk_x = x / CHUNK_SIZE;
		int chunk_y = y / CHUNK_SIZE;
		int chunk = chunk_x + chunk_y * CHUNKS_WIDTH;
		int tile_x = x % CHUNK_SIZE;
		int tile_y = y % CHUNK_SIZE;
		if (chunk_x < 0 || chunk_x >= CHUNKS_WIDTH || chunk_y < 0 || chunk_y >= CHUNKS_WIDTH)
			return;

		std::unique_lock<std::shared_mutex> lock(write_chunk_mutex);

		std::unordered_set<tile_coord, tile_coord_hash> checked_tiles;
		std::queue<tile_coord> tile_queue;
		tile_queue.push(tile_coord(x, y));

		int dx[] = {0, 1, 0, -1};
		int dy[] = {-1, 0, 1, 0};

		while (!tile_queue.empty() && checked_tiles.size() < 200)
		{
			tile_coord current_tile = tile_queue.front();
			tile_queue.pop();
			if (checked_tiles.count(current_tile))
				continue;
			if(current_tile.x < 0 || current_tile.x >= CHUNKS_WIDTH * CHUNK_SIZE || current_tile.y < 0 || current_tile.y >= CHUNKS_WIDTH * CHUNK_SIZE)
				continue;

			checked_tiles.insert(current_tile);

			if (get_write_tile_at(current_tile.x, current_tile.y) >= SOLID_TILE_START_INDEX)
			{
				for (int i = 0; i < 4; i++)
				{
					tile_coord new_tile = tile_coord(current_tile.x + dx[i], current_tile.y + dy[i]);
					
					if (!checked_tiles.count(new_tile))
					{
						tile_queue.push(new_tile);
					}
				}
			} else {
				
				if (is_solid_tile[get_write_tile_at(current_tile.x, current_tile.y)] != is_solid_tile[tile_type])
				{
					set_modified_chunk(current_tile.x / CHUNK_SIZE, current_tile.y / CHUNK_SIZE, 1);
				}
				lock.unlock();
				
				set_tile_at_with_lock(current_tile.x, current_tile.y, tile_type);

				return;
			}
		}

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

		if((tile_type >= GLASS) != (get_tile_at(x, y) >= GLASS))
		{
			set_modified_chunk(chunk_x, chunk_y, 1);
		}
		
		// std::shared_lock<std::shared_mutex> lock(chunk_mutex);
		if (read_buffer == 0)
		{
			(chunk_data_1[chunk])->set_tile(tile_x, tile_y, tile_type);
		}
		else
		{
			(chunk_data_0[chunk])->set_tile(tile_x, tile_y, tile_type);
		}
	}

	std::array<entity, NUM_CHUNKS> world_tile_system::get_chunk_entities()
	{
		return chunk_entities;
	}

	void world_tile_system::update(uint64_t tick_count)
	{
		write_chunk_mutex.lock();

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

				uint8_t tile_type = get_write_tile_at(x, y);
				switch (tile_type)
				{
				case WATER:
					if (game_engine::in_set(get_write_tile_at(x, y + 1), AIR, SMOKE, TEMPORARY_SMOKE))
					{
						set_tile_at_no_lock(x, y, get_write_tile_at(x, y + 1));
						set_tile_at_no_lock(x, y + 1, WATER);
						break;
					}
					if (game_engine::in_set(get_write_tile_at(x - 1, y + 1), AIR, SMOKE, TEMPORARY_SMOKE))
					{
						set_tile_at_no_lock(x, y, get_write_tile_at(x - 1, y + 1));
						set_tile_at_no_lock(x - 1, y + 1, WATER);
						break;
					}
					if (game_engine::in_set(get_write_tile_at(x + 1, y + 1), AIR, SMOKE, TEMPORARY_SMOKE))
					{
						set_tile_at_no_lock(x, y, get_write_tile_at(x + 1, y + 1));
						set_tile_at_no_lock(x + 1, y + 1, WATER);
						break;
					}
					if (game_engine::in_set(get_write_tile_at(x - 1, y), AIR, SMOKE, TEMPORARY_SMOKE))
					{
						set_tile_at_no_lock(x, y, get_write_tile_at(x - 1, y));
						set_tile_at_no_lock(x - 1, y, WATER);
						break;
					}
					if (game_engine::in_set(get_write_tile_at(x + 1, y), AIR, SMOKE, TEMPORARY_SMOKE))
					{
						set_tile_at_no_lock(x, y, get_write_tile_at(x + 1, y));
						set_tile_at_no_lock(x + 1, y, WATER);
						break;
					}
					break;
				case ACID:
					if (rand() % 10 == 0 && get_write_tile_at(x, y + 1) >= GLASS && get_write_tile_at(x, y + 1) < BEDROCK)
					{
						set_tile_at_no_lock(x, y + 1, AIR);
						set_tile_at_no_lock(x, y, AIR);
						break;
					}

					if (game_engine::in_set(get_write_tile_at(x, y + 1), AIR, SMOKE, TEMPORARY_SMOKE))
					{
						set_tile_at_no_lock(x, y, get_write_tile_at(x, y + 1));
						set_tile_at_no_lock(x, y + 1, ACID);
						break;
					}
					if (game_engine::in_set(get_write_tile_at(x - 1, y + 1), AIR, SMOKE, TEMPORARY_SMOKE))
					{
						set_tile_at_no_lock(x, y, get_write_tile_at(x - 1, y + 1));
						set_tile_at_no_lock(x - 1, y + 1, ACID);
						break;
					}
					if (game_engine::in_set(get_write_tile_at(x + 1, y + 1), AIR, SMOKE, TEMPORARY_SMOKE))
					{
						set_tile_at_no_lock(x, y, get_write_tile_at(x + 1, y + 1));
						set_tile_at_no_lock(x + 1, y + 1, ACID);
						break;
					}
					if (game_engine::in_set(get_write_tile_at(x - 1, y), AIR, SMOKE, TEMPORARY_SMOKE))
					{
						set_tile_at_no_lock(x, y, get_write_tile_at(x - 1, y));
						set_tile_at_no_lock(x - 1, y, ACID);
						break;
					}
					if (game_engine::in_set(get_write_tile_at(x + 1, y), AIR, SMOKE, TEMPORARY_SMOKE))
					{
						set_tile_at_no_lock(x, y, get_write_tile_at(x + 1, y));
						set_tile_at_no_lock(x + 1, y, ACID);
						break;
					}
					if (rand() % 3 == 0 && get_write_tile_at(x, y + 1) >= GLASS && get_write_tile_at(x, y + 1) < BEDROCK)
					{
						set_tile_at_no_lock(x, y + 1, AIR);
						set_tile_at_no_lock(x, y, AIR);
						break;
					}
					if (rand() % 12 == 0 && get_write_tile_at(x - 1, y) >= GLASS && get_write_tile_at(x - 1, y) < BEDROCK)
					{
						set_tile_at_no_lock(x - 1, y, AIR);
						set_tile_at_no_lock(x, y, AIR);
						break;
					}
					if (rand() % 12 == 0 && get_write_tile_at(x + 1, y) >= GLASS && get_write_tile_at(x + 1, y) < BEDROCK)
					{
						set_tile_at_no_lock(x + 1, y, AIR);
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
					if (game_engine::in_set(get_write_tile_at(x, y + 1), AIR, SMOKE, WATER, TEMPORARY_SMOKE))
					{
						set_tile_at_no_lock(x, y, get_write_tile_at(x, y + 1));
						set_tile_at_no_lock(x, y + 1, SAND);
					}
					else if (game_engine::in_set(get_write_tile_at(x - 1, y + 1), AIR, SMOKE, WATER, TEMPORARY_SMOKE))
					{
						set_tile_at_no_lock(x, y, get_write_tile_at(x - 1, y + 1));
						set_tile_at_no_lock(x - 1, y + 1, SAND);
					}
					else if (game_engine::in_set(get_write_tile_at(x + 1, y + 1), AIR, SMOKE, WATER, TEMPORARY_SMOKE))
					{
						set_tile_at_no_lock(x, y, get_write_tile_at(x + 1, y + 1));
						set_tile_at_no_lock(x + 1, y + 1, SAND);
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
					if (game_engine::in_set(get_write_tile_at(x, y + 1), AIR, SMOKE, WATER, TEMPORARY_SMOKE))
					{
						set_tile_at_no_lock(x, y, get_write_tile_at(x, y + 1));
						set_tile_at_no_lock(x, y + 1, SNOW);
					}
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
					if (rand() % 5 == 0)
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
		game_engine::box &character_box = box_system_pointer->get(character_ent);
		// get character velocity from box2d
		box2d_system *box2d_system_pointer = ((box2d_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<box2d_system>()));
		b2Body *character_body = box2d_system_pointer->get_dynamic_body(character_ent);

		b2Vec2 velocity = character_body->GetLinearVelocity();
		// get angle of velocity
		float angle = atan2(velocity.y, velocity.x);

		for (int i = (int)character_box.x; i < (int)character_box.x + character_box.w + 1; i++)
		{
			for (int j = (int)character_box.y; j < (int)character_box.y + character_box.h + 1; j++)
			{
				if (get_write_tile_at(i, j) == SNOW)
				{
					set_tile_at_no_lock(i, j, AIR);
					game_engine::task_scheduler_pointer->add_task({&create_single_debris_task, new create_debris_params(character_box.x + ((velocity.x > 0) ? character_box.w + 2 : -2), j, velocity.x, -10, 0.5f, SNOW, TEMPORARY_SMOKE)});
					// game_engine::task_scheduler_pointer->add_task({&create_single_debris_task, new create_debris_params(character_box.x + ((velocity.x > 0) ? character_box.w + 2 : -2), j, (velocity.x > 0) ? 50 : -50, -10, 0.5f, SNOW, TEMPORARY_SMOKE)});
				}
			}
		}

		read_chunk_mutex.lock();
		// copy modified chunks
		for (int i = 0; i < NUM_CHUNKS; i++)
		{
			if (read_buffer == 0)
			{
				memcpy(chunk_data_0[i]->get_data(), chunk_data_1[i]->get_data(), CHUNK_SIZE * CHUNK_SIZE);
				memcpy(modified_chunks_0.data(), modified_chunks_1.data(), NUM_CHUNKS);
			}
			else
			{
				memcpy(chunk_data_1[i]->get_data(), chunk_data_0[i]->get_data(), CHUNK_SIZE * CHUNK_SIZE);
				memcpy(modified_chunks_1.data(), modified_chunks_0.data(), NUM_CHUNKS);
			}
		}
		read_buffer = !read_buffer;
		read_chunk_mutex.unlock();
		write_chunk_mutex.unlock();

		std::shared_lock<std::shared_mutex> tile_lock(read_chunk_mutex);
		// update textures
		for (int chunk = 0; chunk < NUM_CHUNKS; chunk++)
		{
			entity chunk_ent = chunk_entities[chunk];
			int chunk_x = chunk % CHUNKS_WIDTH;
			int chunk_y = chunk / CHUNKS_WIDTH;
			game_engine::render_system *render_system_pointer = ((game_engine::render_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::render_system>()));
			if (read_buffer == 0)
			{
				render_system_pointer->update_texture_section(all_chunk_ent, (uint8_t *)(chunk_data_0[chunk]->get_data()), chunk_x * CHUNK_SIZE, chunk_y * CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
			}
			else
			{
				render_system_pointer->update_texture_section(all_chunk_ent, (uint8_t *)(chunk_data_1[chunk]->get_data()), chunk_x * CHUNK_SIZE, chunk_y * CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
			}
		}
	}

	void world_tile_system::generate_world()
	{
		for (int chunk = 0; chunk < NUM_CHUNKS; chunk++)
		{
			chunk_data_1[chunk]->create_chunk();
			chunk_entities[chunk] = game_engine::game_engine_pointer->create_entity();
		}
		std::unique_lock<std::shared_mutex> lock(write_chunk_mutex);
		// create solid border around world
		for (int x = 0; x < CHUNKS_WIDTH * CHUNK_SIZE; x++)
		{
			set_tile_at_no_lock(x, 0, BEDROCK);
			set_tile_at_no_lock(x, CHUNKS_WIDTH * CHUNK_SIZE - 1, BEDROCK);
		}
		for (int y = 0; y < CHUNKS_WIDTH * CHUNK_SIZE; y++)
		{
			set_tile_at_no_lock(0, y, BEDROCK);
			set_tile_at_no_lock(CHUNKS_WIDTH * CHUNK_SIZE - 1, y, BEDROCK);
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
		if (read_buffer == 0)
			return &chunk_data_0;
		else
			return &chunk_data_1;
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
		std::shared_lock<std::shared_mutex> lock(read_chunk_mutex);
		if (read_buffer == 0)
		{
			chunk_data_0[x + y * CHUNKS_WIDTH]->create_outlines(outlines);
		}
		else
		{
			chunk_data_1[x + y * CHUNKS_WIDTH]->create_outlines(outlines);
		}
		return outlines;
	}

	void world_tile_system::delete_circle(int x, int y, int radius)
	{
		std::unique_lock<std::shared_mutex> lock(write_chunk_mutex);
		for (int i = 0; i < NUM_CHUNKS; i++)
		{
			bool modified;
			if (read_buffer == 1)
			{
				modified = chunk_data_0[i]->delete_circle(x, y, radius);
			}
			else
			{
				modified = chunk_data_1[i]->delete_circle(x, y, radius);
			}

			if (modified)
			{
				set_modified_chunk(i % CHUNKS_WIDTH, i / CHUNKS_WIDTH, 1);
			}
		}
	}

	std::array<uint8_t, game::NUM_CHUNKS> *world_tile_system::get_modified_chunks()
	{
		if(read_buffer == 0)
			return &modified_chunks_0;
		else
			return &modified_chunks_1;
	}

	void world_tile_system::set_modified_chunk(int x, int y, uint8_t value)
	{
		if(read_buffer == 1)
			modified_chunks_0[y * CHUNKS_WIDTH + x] = value;
		else
			modified_chunks_1[y * CHUNKS_WIDTH + x] = value;
	}

}