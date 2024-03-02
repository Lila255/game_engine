#pragma once
#include <array>
#include <chrono>
#include <cmath>
#include <fstream>
// #include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
// #include <CGAL/Delaunay_triangulation_2.h>
#include <utility>
#include <poly2tri/poly2tri.h>
// box2d
#define b2_maxPolygonVertices 8
// #include <box2d/box2d.h>

#include "engine_comp.hpp"
#include "chunk.hpp"
#include "projectile_system.hpp"
#include "world_tile_system.hpp"
#include "box2d_system.hpp"
#include "tree_system.hpp"

#include "tasks.hpp"

// #define M_PI 3.14159265358979323846		/* pi */
// #define radians(x) ((x) * M_PI / 180.0) // degrees to radians
// #define raise(x) (1 << x)

#define PIXEL_SCALE 6

// typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
// typedef K::Point_2 point_2;
// typedef CGAL::Delaunay_triangulation_2<K>  Triangulation;
// typedef Triangulation::Edge_iterator  Edge_iterator;

namespace game
{

	struct b2_contact_listener : public b2ContactListener
	{
		world_tile_system *world_tiles;
		projectile_system *proj_system;

		b2_contact_listener()
		{
			world_tiles = (world_tile_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<world_tile_system>());
			proj_system = (projectile_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<projectile_system>());
		}

		void BeginContact(b2Contact *contact) override
		{
			b2Fixture *fixture_a = contact->GetFixtureA();
			b2Fixture *fixture_b = contact->GetFixtureB();
			b2_user_data *ud_a = (b2_user_data *)fixture_a->GetUserData().pointer;
			b2_user_data *ud_b = (b2_user_data *)fixture_b->GetUserData().pointer;

			if (ud_a == nullptr || ud_b == nullptr)
				return;

			if (ud_a->type == b2fixture_types::PROJECTILE || ud_b->type == b2fixture_types::PROJECTILE)
			{
				// if (fixture_a->IsSensor() || fixture_b->IsSensor())
				// 	return;

				if (ud_a->type == b2fixture_types::PLAYER || ud_b->type == b2fixture_types::PLAYER)
				{
					// printf("Player hit\n");
					return;
				}
				if (ud_a->type == b2fixture_types::TERRAIN || ud_b->type == b2fixture_types::TERRAIN)
				{
					uint16_t explosion_radius = 8;
					// printf("Terrain hit\n");
					if (ud_a->type == b2fixture_types::PROJECTILE)
					{
						// delete circle shape around projectile
						// world_tiles->delete_circle((int)(fixture_a->GetBody()->GetPosition().x + glsl_helper::projectile_width / 2), (int)(fixture_a->GetBody()->GetPosition().y + glsl_helper::projectile_height / 2), explosion_radius);
						game_engine::task_scheduler_pointer->add_task({&delete_circle_task, new delete_circle_params((int)(fixture_a->GetBody()->GetPosition().x + glsl_helper::projectile_width / 2), (int)(fixture_a->GetBody()->GetPosition().y + glsl_helper::projectile_height / 2), explosion_radius)});
						game_engine::task_scheduler_pointer->add_task({(&create_debris_task), new create_debris_params(fixture_a->GetBody()->GetPosition().x + glsl_helper::projectile_width / 2.f, fixture_a->GetBody()->GetPosition().y + glsl_helper::projectile_height / 2.f, 40.f, 40.f, 0.5f, SNOW, SNOW, AIR)});
						// for (int i = 0; i < explosion_radius; i++)
						// {
						// 	entity e = game_engine::game_engine_pointer->create_entity();
						// 	proj_system->create_projectile(e, (fixture_a->GetBody()->GetPosition().x + glsl_helper::projectile_width / 2), (fixture_a->GetBody()->GetPosition().y + glsl_helper::projectile_height / 2), (rand() % 360) / 360.0f, 250.f, 1.f, b2fixture_types::DEBRIS);
						// }

						// set projectile type to empty in ud_a
						ud_a->type = b2fixture_types::EMPTY;
					}
					else
					{
						// delete circle shape around projectile
						// world_tiles->delete_circle((int)(fixture_b->GetBody()->GetPosition().x + glsl_helper::projectile_width / 2), (int)(fixture_b->GetBody()->GetPosition().y + glsl_helper::projectile_height / 2), explosion_radius);
						game_engine::task_scheduler_pointer->add_task({&delete_circle_task, new delete_circle_params((int)(fixture_b->GetBody()->GetPosition().x + glsl_helper::projectile_width / 2), (int)(fixture_b->GetBody()->GetPosition().y + glsl_helper::projectile_height / 2), explosion_radius)});
						game_engine::task_scheduler_pointer->add_task({&create_debris_task, new create_debris_params(fixture_b->GetBody()->GetPosition().x + glsl_helper::projectile_width / 2.f, fixture_b->GetBody()->GetPosition().y + glsl_helper::projectile_height / 2.f, 40.f, 40.f, 0.5f, SNOW, SNOW, AIR)});

						// for (int i = 0; i < explosion_radius; i++)
						// {
						// 	entity e = game_engine::game_engine_pointer->create_entity();
						// 	proj_system->create_projectile(e, (fixture_a->GetBody()->GetPosition().x + glsl_helper::projectile_width / 2), (fixture_a->GetBody()->GetPosition().y + glsl_helper::projectile_height / 2), (rand() % 360) / 360.0f, 250.f, 1.f, b2fixture_types::DEBRIS);
						// }

						// set projectile type to empty in ud_b
						ud_b->type = b2fixture_types::EMPTY;
					}
					return;
				}
			}
			else if (game_engine::in_set(b2fixture_types::DEBRIS, ud_a->type, ud_b->type))
			{
				// debris has collided with something
				if (game_engine::in_set(ud_a->type, b2fixture_types::PLAYER) || game_engine::in_set(ud_b->type, b2fixture_types::PLAYER))
					return;

				if (game_engine::in_set(ud_a->type, b2fixture_types::TERRAIN) || game_engine::in_set(ud_b->type, b2fixture_types::TERRAIN))
				{
					if (ud_a->type == b2fixture_types::DEBRIS)
					{
						b2_user_data *proj_ud = (b2_user_data *)(fixture_a->GetUserData().pointer);

						if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - proj_ud->spawn_time).count() < proj_ud->lifetime)
							return;

						tile_type debri_tile_type = (proj_system->get_projectile(proj_ud->ent)).debri_tile_type;

						if (debri_tile_type != 0)
						{
							game_engine::task_scheduler_pointer->add_task({update_tile_task, new update_tile_params(fixture_a->GetBody()->GetPosition().x, fixture_a->GetBody()->GetPosition().y, debri_tile_type)});
						}
						ud_a->type = b2fixture_types::EMPTY;
					}
					else
					{
						b2_user_data *proj_ud = (b2_user_data *)(fixture_b->GetUserData().pointer);

						if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - proj_ud->spawn_time).count() < proj_ud->lifetime)
							return;

						tile_type debri_tile_type = (proj_system->get_projectile(proj_ud->ent)).debri_tile_type;

						if (debri_tile_type != 0)
						{
							game_engine::task_scheduler_pointer->add_task({update_tile_task, new update_tile_params(fixture_b->GetBody()->GetPosition().x, fixture_b->GetBody()->GetPosition().y, debri_tile_type)});
						}
						ud_b->type = b2fixture_types::EMPTY;
					}
				}
			}
		}
	};

	// misc

	std::vector<game_engine::vec2> load_normal_vectors();

} // namespace game
