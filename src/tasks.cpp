#include "tasks.hpp"

#include "projectile_system.hpp"

namespace game
{
	void create_debris_task(void *parameters)
	{
		create_debris_params * params = (create_debris_params*)(parameters);
		projectile_system *proj_sys = (projectile_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<projectile_system>());

		for (int i = 0; i < 16; i++)
		{
			entity e = game_engine::game_engine_pointer->create_entity();
			// proj_system->create_projectile(e, (fixture_a->GetBody()->GetPosition().x + glsl_helper::projectile_width / 2), (fixture_a->GetBody()->GetPosition().y + glsl_helper::projectile_height / 2), (rand() % 360) / 360.0f, 250.f, 1.f, b2fixture_types::DEBRIS);
			proj_sys->create_projectile(e, params->x, params->y, radians((rand() % 360)), params->v, params->r, b2fixture_types::DEBRIS);
		}
	}

	void shutdown_task_schedular_task(void * parameters)
	{
		printf("Shutting down task scehdular\n");
	}
}