#include "tasks.hpp"

#include "projectile_system.hpp"
#include "box2d_system.hpp"
#include "world_tile_system.hpp"
#include "tree_system.hpp"
#include "flying_creature_system.hpp"
#include "legged_creature_system.hpp"

namespace game
{
	void shutdown_task_schedular_task(void *parameters)
	{
		printf("Shutting down task scehdular\n");
	}

	void create_debris_task(void *parameters)
	{
		create_debris_params *params = (create_debris_params *)(parameters);
		projectile_system *proj_sys = (projectile_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<projectile_system>());

		float vel = sqrt(params->v_x * params->v_x + params->v_y * params->v_y);
		for (int i = 0; i < 8; i++)
		{
			entity e = game_engine::game_engine_pointer->create_entity();
			// proj_system->create_projectile(e, (fixture_a->GetBody()->GetPosition().x + glsl_helper::projectile_width / 2), (fixture_a->GetBody()->GetPosition().y + glsl_helper::projectile_height / 2), (rand() % 360) / 360.0f, 250.f, 1.f, b2fixture_types::DEBRIS);
			projectile &proj = proj_sys->create_projectile(e, params->x, params->y, radians((rand() % 360)), vel, params->r, params->millis_lifetime, b2fixture_types::DEBRIS);
			proj.debri_tile_type = (tile_type)(params->debri_tile_type);
			proj.temporary_trail_tile_type = (tile_type)(params->temp_tile_type);
			proj.permanent_trail_tile_type = (tile_type)(params->perm_tile_type);
			proj.tile_temperature = params->tile_temperature;
		}
	}

	bool is_in_player(int x, int y, float player_x, float player_y, float w, float h)
	{
		return x >= floor(player_x) && x <= ceil(player_x) + w && y >= floor(player_y) && y <= ceil(player_y) + h;
	}

	void create_single_debris_task(void *parameters)
	{
		create_debris_params *params = (create_debris_params *)(parameters);
		projectile_system *proj_sys = (projectile_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<projectile_system>());
		world_tile_system *world_tiles = ((world_tile_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<world_tile_system>()));
		box2d_system *b2d_sys = ((box2d_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<box2d_system>()));
		game_engine::box_system *box_system_pointer = ((game_engine::box_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::box_system>()));
		entity player_entity = game_engine::game_engine_pointer->player_entitiy;
		game_engine::box character_box = box_system_pointer->get(player_entity).get_box();
		b2Vec2 player_pos = b2d_sys->get_dynamic_body(player_entity)->GetPosition();

		entity e = game_engine::game_engine_pointer->create_entity();
		float ang = atan2(params->v_y, params->v_x);
		float vel = sqrt(params->v_x * params->v_x + params->v_y * params->v_y);

		std::queue<tile_coord> tiles_to_check;
		std::unordered_set<tile_coord, tile_coord_hash> checked_tiles;
		tiles_to_check.push(tile_coord{(uint32_t)(params->x), (uint32_t)(params->y)});

		bool found_non_solid = false;
		int dx[] = {0, 1, 0, -1};
		int dy[] = {-1, 0, 1, 0};

		float proj_x, proj_y;

		// find free space to spawn debris
		while (tiles_to_check.size())
		{
			tile_coord current_tile = tiles_to_check.front();
			tiles_to_check.pop();
			if (checked_tiles.count(current_tile))
				continue;

			checked_tiles.insert(current_tile);

			if (current_tile.x < 0 || current_tile.x >= CHUNKS_WIDTH * CHUNK_SIZE || current_tile.y < 0 || current_tile.y >= CHUNKS_WIDTH * CHUNK_SIZE)
			{
				continue;
			}

			uint8_t current_tile_type = world_tiles->get_tile_at(current_tile.x, current_tile.y);

			if (found_non_solid && is_solid_tile[current_tile_type])
			{
				continue;
			}

			if (!found_non_solid && !is_solid_tile[current_tile_type])
			{
				found_non_solid = true;
				tiles_to_check = {};
			}

			if (!is_solid_tile[current_tile_type] && !is_in_player(current_tile.x, current_tile.y, character_box.x, character_box.y, character_box.w, character_box.h))
			{
				proj_x = current_tile.x;
				proj_y = current_tile.y;
				break;
			}
			else
			{
				for (int i = 0; i < 4; i++)
				{
					tile_coord new_tile = tile_coord(current_tile.x + dx[i], current_tile.y + dy[i]);

					if (!checked_tiles.count(new_tile))
					{
						tiles_to_check.push(new_tile);
					}
				}
			}
		}

		projectile &proj = proj_sys->create_projectile(e, proj_x, proj_y, ang, vel, params->r, params->millis_lifetime, b2fixture_types::DEBRIS);
		proj.debri_tile_type = (tile_type)(params->debri_tile_type);
		proj.temporary_trail_tile_type = (tile_type)(params->temp_tile_type);
		proj.permanent_trail_tile_type = (tile_type)(params->perm_tile_type);
	}

	void delete_circle_task(void *parameters)
	{
		delete_circle_params *params = (delete_circle_params *)(parameters);
		world_tile_system *world_tiles = ((world_tile_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<world_tile_system>()));

		b2d_mutex.lock();
		world_tiles->delete_circle(params->x, params->y, params->radius, {});
		b2d_mutex.unlock();
	}

	void update_tile_task(void *parameters)
	{
		update_tile_params *params = (update_tile_params *)parameters;
		world_tile_system *world_tiles = ((world_tile_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<world_tile_system>()));

		if (params->m_tile_type == TREE_SEED && world_tiles->get_tile_at(params->x, params->y) == TREE_SEED)
		{
			return;
		}

		world_tiles->set_tile_at_with_search_and_lock(params->x, params->y, params->m_tile_type);

		if (params->m_tile_type == TREE_SEED)
		{
			tree_mutex.lock();
			tree_system *tree_sys = ((tree_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<tree_system>()));
			tree t;
			t.seed_x = params->x;
			t.seed_y = params->y;
			t.root_tiles[tile_coord{t.seed_x, t.seed_y}] = tree_tracer{0, 0, 0, 0};
			tree_sys->add_tree(game_engine::game_engine_pointer->create_entity(), t);
			tree_mutex.unlock();
		}
	}

	

	void create_flying_creature_nest_task(void *parameters)
	{
		create_flying_creature_nest_params *params = (create_flying_creature_nest_params *)parameters;
		
		game::flying_creature_system *flying_creature_sys = (game::flying_creature_system *)(game_engine::game_engine_pointer->get_system(game_engine::family::type<game::flying_creature_system>()));

		for(int i = 0; i < 8; i++)
		{
			entity e = game_engine::game_engine_pointer->create_entity();
			flying_creature_sys -> create_flying_creature(e, params->x + rand() % 10 - 5, params->y + rand() % 10 - 5, game::flying_creature_type::BEE);
			flying_creature & c = flying_creature_sys->get(e);
			
			c.target_home = {params->x, params->y};
		}
	}

	void flying_creature_eat_task(void *parameters)
	{
		flying_creature_eat_params *params = (flying_creature_eat_params *)parameters;

		world_tile_system *world_tiles = ((world_tile_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<world_tile_system>()));
		flying_creature_system *flying_creature_sys = ((flying_creature_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<flying_creature_system>()));
		flying_creature & c = flying_creature_sys->get(params->ent);

		b2d_mutex.lock();
		uint32_t eaten = world_tiles->delete_circle(params->x, params->y, 5, {WAX, HONEY});
		c.add_collected_mass(eaten);

		b2d_mutex.unlock();
	}

	// check 8 directions
	int dx[] = {0,   1, 1, 1, 0, -1, -1, -1};
	int dy[] = {-1, -1, 0, 1, 1,  1,  0, -1};	
	std::unordered_map<int, std::pair<int, int>> bee_tile_joints_reverse = {
		{5, {0, 3}},
		{3, {0, 5}},
		{0, {3, 5}},
		{7, {1, 4}},
		{1, {4, 7}},
		{4, {1, 7}},
	};
	std::pair<int, int> find_creature_nest_end(std::pair<int, int> start_tile)
	{
		world_tile_system *world_tiles = ((world_tile_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<world_tile_system>()));
		std::queue<std::pair<int, int>> tiles_to_check;
		std::unordered_set<std::pair<int, int>, chunk_coord_pair_hash> checked_tiles;

		tiles_to_check.push(start_tile);


		while(tiles_to_check.size())
		{
			std::pair<int, int> current_tile = tiles_to_check.front();
			tiles_to_check.pop();
			if(checked_tiles.count(current_tile))
				continue;
			
			checked_tiles.insert(current_tile);

			if(world_tiles->get_tile_at(current_tile.first, current_tile.second) != WAX)
			{
				continue;
			}


			for(int i = 0; i < 8; i++)
			{
				std::pair<int, int> new_tile = {current_tile.first + dx[i], current_tile.second + dy[i]};

				if(!checked_tiles.count(new_tile))
				{
					if(world_tiles->get_tile_at(new_tile.first, new_tile.second) == WAX)
					{
						bool found_next_tiles = false;

						std::pair<int, int> same_dir_tile = {new_tile.first + dx[i], new_tile.second + dy[i]};
						if(world_tiles->get_tile_at(same_dir_tile.first, same_dir_tile.second) == WAX)
						{
							tiles_to_check.push(new_tile);
							found_next_tiles = true;
						}
						// uint8_t right_turn_index = (i + 1) % 8;
						// uint8_t left_turn_index = (i + 7) % 8;
						uint8_t right_turn_index;
						uint8_t left_turn_index;
						uint8_t reverse_index = (i + 4) % 8;
						if(bee_tile_joints_reverse.count(i))
						{
							std::pair<int, int> opposite_tile = bee_tile_joints_reverse[i];
							right_turn_index = opposite_tile.first;
							left_turn_index = opposite_tile.second;
						}
						else
						{
							printf("Error: bee_tile_joints_reverse does not contain key %d\n", i);
							continue;
						}

						std::pair<int, int> right_turn_tile = {new_tile.first + dx[right_turn_index], new_tile.second + dy[right_turn_index]};
						std::pair<int, int> left_turn_tile = {new_tile.first + dx[left_turn_index], new_tile.second + dy[left_turn_index]};

						bool right_turn_tile_is_wax = world_tiles->get_tile_at(right_turn_tile.first, right_turn_tile.second) == WAX;
						bool left_turn_tile_is_wax = world_tiles->get_tile_at(left_turn_tile.first, left_turn_tile.second) == WAX;

						if(!found_next_tiles && (right_turn_tile_is_wax + left_turn_tile_is_wax == 1))
						{
							return new_tile;
						}


						if(right_turn_tile_is_wax)
						{
							tiles_to_check.push(new_tile);
							found_next_tiles = true;
						}
						
						if(left_turn_tile_is_wax)
						{
							tiles_to_check.push(new_tile);
							found_next_tiles = true;
						}


						if(!found_next_tiles)
						{
							return new_tile;
						}
					}
				}
			}
		}
		return start_tile;
	}

	const int nest_wall_length = 6;
	std::unordered_map<std::pair<int, int>, int, chunk_coord_pair_hash> bee_tile_joints = {
		{{0, 3}, 5},
		{{0, 5}, 3},
		{{3, 5}, 0},
		{{1, 4}, 7},
		{{4, 7}, 1},
		{{1, 7}, 4},
	};


	std::pair<int, int> place_creature_nest_tile(tile_type type, std::pair<int, int> end_tile)
	{
		world_tile_system *world_tiles = ((world_tile_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<world_tile_system>()));

		int8_t direction = -1;
		for(int i = 0; i < 8; i++)
		{
			if(world_tiles->get_tile_at(end_tile.first + dx[i], end_tile.second + dy[i]) == WAX)
			{
				if(direction == -1)
				{
					direction = i;
				}
				else
				{
					if(bee_tile_joints.count({direction, i}))
					{
						direction = bee_tile_joints[{direction, i}];
					}
					else
					{
						printf("Error: bee_tile_joints does not contain key %d, %d\n", direction, i);
						return end_tile;
					}
				}
			}
		}
		
		if(direction == -1)
		{
			world_tiles->set_tile_at_with_lock(end_tile.first, end_tile.second - 1, type);
			return {end_tile.first, end_tile.second - 1};
		}

		uint8_t tile_count = 1;
		std::pair<int, int> current_tile = end_tile;
		while(1)
		{
			std::pair<int, int> next_tile = {current_tile.first + dx[direction], current_tile.second + dy[direction]};
			if(world_tiles->get_tile_at(next_tile.first, next_tile.second) == WAX)
			{
				current_tile = next_tile;
				tile_count++;
			}
			else
			{
				break;
			}
		}

		if(tile_count < nest_wall_length)
		{
			uint8_t opposite_direction = (direction + 4) % 8;
			std::pair<int, int> opposite_tile = {end_tile.first + dx[opposite_direction], end_tile.second + dy[opposite_direction]};
			world_tiles->set_tile_at_with_lock(opposite_tile.first, opposite_tile.second, type);
			return opposite_tile;
		}
		else
		{
			uint8_t opposite_direction = (direction + 4) % 8;
			uint8_t right_turn_index;// = (opposite_direction + 1) % 8;
			uint8_t left_turn_index;// = (opposite_direction + 7) % 8;
			if(direction == 0 || direction == 4)
			{
				right_turn_index = (opposite_direction + 1) % 8;
				left_turn_index = (opposite_direction + 7) % 8;
			}
			else if(direction == 1)
			{
				right_turn_index = (opposite_direction + 2) % 8;
				left_turn_index = (opposite_direction + 7) % 8;
			}
			else if(direction == 3)
			{
				right_turn_index = (opposite_direction + 1) % 8;
				left_turn_index = (opposite_direction + 6) % 8;
			}
			else if(direction == 5)
			{
				right_turn_index = (opposite_direction + 2) % 8;
				left_turn_index = (opposite_direction + 7) % 8;
			}
			else if(direction == 7)
			{
				right_turn_index = (opposite_direction + 1) % 8;
				left_turn_index = (opposite_direction + 6) % 8;
			}
			else
			{
				right_turn_index = (opposite_direction + 2) % 8;
				left_turn_index = (opposite_direction + 7) % 8;
			}



			std::pair<int, int> right_turn_tile = {end_tile.first + dx[right_turn_index], end_tile.second + dy[right_turn_index]};
			std::pair<int, int> left_turn_tile = {end_tile.first + dx[left_turn_index], end_tile.second + dy[left_turn_index]};

			if(world_tiles->get_tile_at(right_turn_tile.first, right_turn_tile.second) != WAX)
			{
				world_tiles->set_tile_at_with_lock(right_turn_tile.first, right_turn_tile.second, type);
				return right_turn_tile;
			}
			else if(world_tiles->get_tile_at(left_turn_tile.first, left_turn_tile.second) != WAX)
			{
				world_tiles->set_tile_at_with_lock(left_turn_tile.first, left_turn_tile.second, type);
				return left_turn_tile;
			}
			// else
			// {
			// 	uint8_t opposite_direction = (direction + 4) % 8;
			// 	std::pair<int, int> opposite_tile = {end_tile.first + dx[opposite_direction], end_tile.second + dy[opposite_direction]};
			// 	world_tiles->set_tile_at_with_lock(opposite_tile.first, opposite_tile.second, type);
			// 	return opposite_tile;
			// }
				
		}


		return end_tile;
	}

	void flying_creature_deposit_task(void *parameters)
	{
		flying_creature_deposit_params *params = (flying_creature_deposit_params *)parameters;

		world_tile_system *world_tiles = ((world_tile_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<world_tile_system>()));
		flying_creature_system *flying_creature_sys = ((flying_creature_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<flying_creature_system>()));

		flying_creature & c = flying_creature_sys->get(params->ent);
		
		std::pair<int, int> target_home = c.target_home;
		std::pair<int, int> start_tile;
		if(world_tiles -> get_tile_at(params->x, params->y) == WAX)
		{
			start_tile = {params->x, params->y};
		}
		else
		{
			world_tiles -> find_tile_in_rect(start_tile, params->x - 5, params->y - 5, 10, 10, {WAX});
		}

		std::pair<int, int> end_tile = find_creature_nest_end(start_tile);

		int32_t collected_mas = c.get_collected_mass();
		int32_t deposited_mass = 0;

		for(int i = 0; i < collected_mas; i++)
		{
			std::pair<int, int> next = place_creature_nest_tile(WAX, end_tile);
			if(next == end_tile)
			{
				break;
			}
			deposited_mass++;
			end_tile = next;
		}

		// printf("Depositing %d wax at %d, %d\n", c.get_collected_mass(), start_tile.first, start_tile.second);

		// for(int i = 0; i < c.get_collected_mass(); i++)
		// {
		// 	world_tiles->set_tile_at_with_search_and_lock(start_tile.first, start_tile.second, WAX);
		// }
		c.set_collected_mass(collected_mas - deposited_mass);
		c.target_home = end_tile;
	}
	
	// legged_creature_step_task
	void legged_creature_step_task(void *parameters)
	{
		legged_creature_step_params *params = (legged_creature_step_params *)parameters;
		box2d_system *b2d_sys = ((box2d_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<box2d_system>()));
		legged_creature_system *legged_creature_sys = ((legged_creature_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<legged_creature_system>()));
		legged_creature &c = legged_creature_sys->get(params->legged_creature_entity);
		b2d_mutex.lock();
		b2Body *foot_body = b2d_sys->get_dynamic_body(params->foot_entity);
		b2Body *legged_creature_body = b2d_sys->get_dynamic_body(params->legged_creature_entity);

		
		// lock the foot to ground tile
		foot_body->SetType(b2_staticBody);
		foot_body->SetLinearVelocity(b2Vec2(0, 0));
		foot_body->SetAngularVelocity(0);

		c.connected_legs.insert(params->leg_index);
		b2d_mutex.unlock();
	}
}