#include "tasks.hpp"

#include "projectile_system.hpp"
#include "box2d_system.hpp"
#include "world_tile_system.hpp"
#include "tree_system.hpp"
#include "flying_creature_system.hpp"

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

		for(int i = 0; i < 20; i++)
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

		printf("Depositing %d wax at %d, %d\n", c.get_collected_mass(), start_tile.first, start_tile.second);

		for(int i = 0; i < c.get_collected_mass(); i++)
		{
			world_tiles->set_tile_at_with_search_and_lock(start_tile.first, start_tile.second, WAX);
		}
		c.set_collected_mass(0);
		c.target_home = {params->x, params->y};
	}
	
	
}