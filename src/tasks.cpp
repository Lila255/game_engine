#include "tasks.hpp"

#include "projectile_system.hpp"
#include "box2d_system.hpp"
#include "world_tile_system.hpp"
#include "tree_system.hpp"

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
			projectile &proj = proj_sys->create_projectile(e, params->x, params->y, radians((rand() % 360)), vel, params->r, b2fixture_types::DEBRIS);
			proj.debri_tile_type = (tile_type)(params->debri_tile_type);
			proj.trail_tile_type = (tile_type)(params->trail_tile_type);
		}
	}

	void create_single_debris_task(void *parameters)
	{
		create_debris_params *params = (create_debris_params *)(parameters);
		projectile_system *proj_sys = (projectile_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<projectile_system>());
		entity e = game_engine::game_engine_pointer->create_entity();
		float ang = atan2(params->v_y, params->v_x);
		float vel = sqrt(params->v_x * params->v_x + params->v_y * params->v_y);
		projectile &proj = proj_sys->create_projectile(e, params->x, params->y, ang, vel, params->r, b2fixture_types::DEBRIS);
		proj.debri_tile_type = (tile_type)(params->debri_tile_type);
		proj.trail_tile_type = (tile_type)(params->trail_tile_type);
	}

	void delete_circle_task(void *parameters)
	{
		delete_circle_params *params = (delete_circle_params *)(parameters);
		world_tile_system *world_tiles = ((world_tile_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<world_tile_system>()));

		b2d_mutex.lock();
		world_tiles->delete_circle(params->x, params->y, params->radius);
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
			tree_system *tree_sys = ((tree_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<tree_system>()));
			tree t;
			t.seed_x = params->x;
			t.seed_y = params->y;
			t.root_tiles[tile_coord{t.seed_x, t.seed_y}] = tree_tracer{0, 0, 0, 0};
			tree_sys->add_tree(game_engine::game_engine_pointer->create_entity(), t);
		}
	}
}