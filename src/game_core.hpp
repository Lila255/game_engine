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
#include "flying_creature_system.hpp"
#include "legged_creature_system.hpp"
#include "chunk_frame_system.hpp"
#include "tile_arcing_system.hpp"
#include "tile_pathfinding_system.hpp"
#include "tile_conveyor_system.hpp"
#include "building_component_system.hpp"
#include "inventory_system.hpp"

#include "tasks.hpp"

// #define M_PI 3.14159265358979323846		/* pi */
// #define radians(x) ((x) * M_PI / 180.0) // degrees to radians
// #define raise(x) (1 << x)

#define PIXEL_SCALE 5

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
		flying_creature_system *flying_creature_sys;

		b2_contact_listener()
		{
			world_tiles = (world_tile_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<world_tile_system>());
			proj_system = (projectile_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<projectile_system>());
			flying_creature_sys = (flying_creature_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<flying_creature_system>());
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
					uint16_t explosion_radius = 4;
					// printf("Terrain hit\n");

					tile_type t = LAVA;
					int16_t temperature = 2100;
					if (ud_a->type == b2fixture_types::PROJECTILE)
					{
						// delete circle shape around projectile
						// world_tiles->delete_circle((int)(fixture_a->GetBody()->GetPosition().x + glsl_helper::projectile_width / 2), (int)(fixture_a->GetBody()->GetPosition().y + glsl_helper::projectile_height / 2), explosion_radius, {});
						game_engine::task_scheduler_pointer->add_task({&delete_circle_task, new delete_circle_params((int)(fixture_a->GetBody()->GetPosition().x * game::box2d_scale + glsl_helper::projectile_width / 2), (int)(fixture_a->GetBody()->GetPosition().y * game::box2d_scale + glsl_helper::projectile_height / 2), explosion_radius)});
						// game_engine::task_scheduler_pointer->add_task({(&create_debris_task), new create_debris_params(fixture_a->GetBody()->GetPosition().x * game::box2d_scale + glsl_helper::projectile_width / 2.f, fixture_a->GetBody()->GetPosition().y * game::box2d_scale + glsl_helper::projectile_height / 2.f, 4.f, 4.f, 0.5f, t, t, AIR, 500, temperature)});
						// for (int i = 0; i < explosion_radius; i++)
						// {
						// 	entity e = game_engine::game_engine_pointer->create_entity();
						// 	proj_system->create_projectile(e, (fixture_a->GetBody()->GetPosition().x + glsl_helper::projectile_width / 2), (fixture_a->GetBody()->GetPosition().y + glsl_helper::projectile_height / 2), (rand() % 360) / 360.0f, 250.f, 1.f, b2fixture_types::DEBRIS);
						// }
						

						// create flying_creature_nest
						// game_engine::task_scheduler_pointer->add_task({&delete_circle_task, new delete_circle_params((int)(fixture_a->GetBody()->GetPosition().x * game::box2d_scale + glsl_helper::projectile_width / 2), (int)(fixture_a->GetBody()->GetPosition().y * game::box2d_scale + glsl_helper::projectile_height / 2), explosion_radius)});
						flying_creature_sys->add_task({&create_flying_creature_nest_task, new create_flying_creature_nest_params(fixture_a->GetBody()->GetPosition().x * game::box2d_scale + glsl_helper::projectile_width * 2, fixture_a->GetBody()->GetPosition().y * game::box2d_scale + glsl_helper::projectile_height * 2)});
						// game_engine::task_scheduler_pointer->add_task({&update_tile_task, new update_tile_params((int)(fixture_b->GetBody()->GetPosition().x * game::box2d_scale), (int)(fixture_b->GetBody()->GetPosition().y * game::box2d_scale), WAX)});

						// set projectile type to empty in ud_a
						ud_a->type = b2fixture_types::EMPTY;
					}
					else
					{
						// delete circle shape around projectile
						// world_tiles->delete_circle((int)(fixture_b->GetBody()->GetPosition().x + glsl_helper::projectile_width / 2), (int)(fixture_b->GetBody()->GetPosition().y + glsl_helper::projectile_height / 2), explosion_radius, {});
						game_engine::task_scheduler_pointer->add_task({&delete_circle_task, new delete_circle_params((int)(fixture_b->GetBody()->GetPosition().x * game::box2d_scale + glsl_helper::projectile_width / 2), (int)(fixture_b->GetBody()->GetPosition().y * game::box2d_scale + glsl_helper::projectile_height / 2), explosion_radius)});
						// game_engine::task_scheduler_pointer->add_task({&create_debris_task, new create_debris_params(fixture_b->GetBody()->GetPosition().x * game::box2d_scale + glsl_helper::projectile_width / 2.f, fixture_b->GetBody()->GetPosition().y * game::box2d_scale + glsl_helper::projectile_height / 2.f, 4.f, 4.f, 0.5f, t, t, AIR, 500, temperature)});
	
						// // create flying_creature_nest
						// game_engine::task_scheduler_pointer->add_task({&delete_circle_task, new delete_circle_params((int)(fixture_b->GetBody()->GetPosition().x * game::box2d_scale + glsl_helper::projectile_width / 2), (int)(fixture_b->GetBody()->GetPosition().y * game::box2d_scale + glsl_helper::projectile_height / 2), explosion_radius)});
						// // game_engine::task_scheduler_pointer->add_task({&create_flying_creature_nest_task, new create_flying_creature_nest_params(fixture_b->GetBody()->GetPosition().x * game::box2d_scale + glsl_helper::projectile_width * 2, fixture_b->GetBody()->GetPosition().y * game::box2d_scale + glsl_helper::projectile_height * 2)});
						flying_creature_sys->add_task({&create_flying_creature_nest_task, new create_flying_creature_nest_params((int)(fixture_b->GetBody()->GetPosition().x * game::box2d_scale), (int)(fixture_b->GetBody()->GetPosition().y * game::box2d_scale))});
						// game_engine::task_scheduler_pointer->add_task({&update_tile_task, new update_tile_params((int)(fixture_b->GetBody()->GetPosition().x * game::box2d_scale), (int)(fixture_b->GetBody()->GetPosition().y * game::box2d_scale), WAX)});
						
						
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

				// entity projectile_entity = (ud_a->type == b2fixture_types::DEBRIS) ? ud_a->ent : ud_b->ent;

				// // if projectile system does not contain the projectile_entity, projectile has already been removed
				// if(proj_system->contains(projectile_entity) == false)
				// 	return;
				
				// projectile &proj = proj_system->get_projectile(projectile_entity);
				// if (proj.last_collided_time + 100 > std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count())
				// 	return;

				// proj.last_collided_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

				if (game_engine::in_set(ud_a->type, b2fixture_types::TERRAIN) || game_engine::in_set(ud_b->type, b2fixture_types::TERRAIN))
				{
					if (ud_a->type == b2fixture_types::DEBRIS)
					{
						b2_user_data *proj_ud = (b2_user_data *)(fixture_a->GetUserData().pointer);

						if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - proj_ud->spawn_time).count() < proj_ud->lifetime)
							return;

						if (proj_system->contains(proj_ud->ent) == false)
							return;
						tile_type debri_tile_type = (proj_system->get_projectile(proj_ud->ent)).debri_tile_type;

						if (debri_tile_type != 0)
						{
							projectile &proj = proj_system->get_projectile(proj_ud->ent);
							game_engine::task_scheduler_pointer->add_task({update_tile_flush_task, new update_tile_params(fixture_a->GetBody()->GetPosition().x * game::box2d_scale, fixture_a->GetBody()->GetPosition().y * game::box2d_scale, debri_tile_type, proj.tile_temperature, proj.tile_misc_data)});
							proj_system->remove_projectile(proj_ud->ent);
						}
						ud_a->type = b2fixture_types::EMPTY;
					}
					else
					{
						b2_user_data *proj_ud = (b2_user_data *)(fixture_b->GetUserData().pointer);

						if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - proj_ud->spawn_time).count() < proj_ud->lifetime)
							return;

						if (proj_system->contains(proj_ud->ent) == false)
							return;

						tile_type debri_tile_type = (proj_system->get_projectile(proj_ud->ent)).debri_tile_type;

						if (debri_tile_type != 0)
						{
							projectile &proj = proj_system->get_projectile(proj_ud->ent);
							game_engine::task_scheduler_pointer->add_task({update_tile_flush_task, new update_tile_params(fixture_b->GetBody()->GetPosition().x * game::box2d_scale, fixture_b->GetBody()->GetPosition().y * game::box2d_scale, debri_tile_type, proj.tile_temperature, proj.tile_misc_data)});
							proj_system->remove_projectile(proj_ud->ent);
						}
						ud_b->type = b2fixture_types::EMPTY;
					}
				}
			}
			else if (game_engine::in_set(b2fixture_types::FLYING_CREATURE, ud_a->type, ud_b->type))
			{
				if (game_engine::in_set(ud_a->type, b2fixture_types::PLAYER) || game_engine::in_set(ud_b->type, b2fixture_types::PLAYER))
					return;

				if (game_engine::in_set(ud_a->type, b2fixture_types::TERRAIN) || game_engine::in_set(ud_b->type, b2fixture_types::TERRAIN))
				{
					b2Vec2 pos;
					entity ent;
					if (ud_a->type == b2fixture_types::FLYING_CREATURE)
					{
						pos = fixture_a->GetBody()->GetPosition();
						ent = ud_a->ent;
					}
					else
					{
						pos = fixture_b->GetBody()->GetPosition();
						ent = ud_b->ent;
					}

					flying_creature &creature = flying_creature_sys->get(ent);

					std::pair<int, int> tile_pos;
					switch(creature.get_state())
					{
						case flying_creature_state::TRAVELING:
							// // if creature has collided with wax, deposit collected mass
							
							// if(world_tiles -> find_tile_in_rect(tile_pos, pos.x * game::box2d_scale - 2, pos.y * game::box2d_scale - 2, 8, 8, {WAX}))
							// {
							// 	game_engine::task_scheduler_pointer->add_task({&flying_creature_deposit_task, new flying_creature_deposit_params(tile_pos.first, tile_pos.second, ent)});
							// }
							// break;
						case flying_creature_state::EATING:
							if(creature.last_eat_time + 1000 > std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count())
								return;
							game_engine::task_scheduler_pointer->add_task({&flying_creature_eat_task, new flying_creature_eat_params(pos.x * game::box2d_scale, pos.y * game::box2d_scale, ent)});
							creature.last_eat_time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
							break;
					}
				}
			}
			else if (game_engine::in_set(b2fixture_types::FEET, ud_a->type, ud_b->type))
			{
				if (!game_engine::in_set(ud_a->type, b2fixture_types::TERRAIN) && !game_engine::in_set(ud_b->type, b2fixture_types::TERRAIN))
					return;
				
				b2_user_data * foot_user_data;
				if (ud_a->type == b2fixture_types::FEET)
				{
					foot_user_data = ud_a;
				}
				else
				{
					foot_user_data = ud_b;
				}
				entity legged_creature_ent = foot_user_data->ent_2;
				entity foot_ent = foot_user_data->ent;
				uint64_t foot_index = foot_user_data->index;

				game_engine::task_scheduler_pointer->add_task({&legged_creature_step_task, new legged_creature_step_params(legged_creature_ent, foot_ent, foot_index)});

			}
		}
	};

	// misc

	std::vector<game_engine::vec2> load_normal_vectors();

} // namespace game
