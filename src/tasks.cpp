#include "tasks.hpp"

#include "projectile_system.hpp"
#include "box2d_system.hpp"
#include "world_tile_system.hpp"

namespace game
{
	void shutdown_task_schedular_task(void * parameters)
	{
		printf("Shutting down task scehdular\n");
	}

	void create_debris_task(void *parameters)
	{
		create_debris_params * params = (create_debris_params*)(parameters);
		projectile_system *proj_sys = (projectile_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<projectile_system>());

		for (int i = 0; i < 64; i++)
		{
			entity e = game_engine::game_engine_pointer->create_entity();
			// proj_system->create_projectile(e, (fixture_a->GetBody()->GetPosition().x + glsl_helper::projectile_width / 2), (fixture_a->GetBody()->GetPosition().y + glsl_helper::projectile_height / 2), (rand() % 360) / 360.0f, 250.f, 1.f, b2fixture_types::DEBRIS);
			projectile & proj = proj_sys->create_projectile(e, params->x, params->y, radians((rand() % 360)), params->v, params->r, b2fixture_types::DEBRIS);
			proj.debri_tile_type = TREE_SEED;
			proj.trail_tile_type = TEMPORARY_SMOKE;
		}
	}

	void delete_circle_task(void *parameters)
	{
		delete_circle_params * params = (delete_circle_params*)(parameters);
		world_tile_system *world_tiles = ((world_tile_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<world_tile_system>()));

		b2d_mutex.lock();
		world_tiles->delete_circle(params->x, params->y, params->radius);
		b2d_mutex.unlock();
	}

	void update_tile_task(void * parameters)
	{
		update_tile_params * params = (update_tile_params *) parameters;
		world_tile_system *world_tiles = ((world_tile_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<world_tile_system>()));
		world_tiles -> set_tile_at(params -> x, params -> y, params -> m_tile_type);
	}
}