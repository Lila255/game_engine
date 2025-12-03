#include "projectile_system.hpp"
#include "box2d_system.hpp"
#include "world_tile_system.hpp"
#include "tasks.hpp"

namespace game
{
	std::mutex projectile_mutex;

	void projectile_system::update(uint64_t tick_count)
	{
		increment_counter();
		auto start = std::chrono::steady_clock::now();
		box2d_system *box2d_system_pointer = ((box2d_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<box2d_system>()));
		game_engine::box_system *bo_system_pointer = ((game_engine::box_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::box_system>()));
		game_engine::render_system *render_system_pointer = ((game_engine::render_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::render_system>()));
		world_tile_system *world_tiles = ((world_tile_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<world_tile_system>()));
		// retrieve all projectiles' locations from the box2d world, and update their positions in the game
		// for (auto &proj : projectiles)
		
		// printf("projectiles size: %d\n", entities.size());
		// 
		projectile_mutex.lock();
		std::vector<entity> entities = projectiles.get_entities();
		// if(entities.size() % 10 == 0)
		// {
		// 	printf("projectiles size: %d\n", entities.size());
		// }
		if (tick_count % 100 == 0)
		{
			projectiles.shrink_to_fit();
		}

		for (auto &proj_entity : entities)
		{
			projectile &projectile = projectiles.get(proj_entity);
			if (box2d_system_pointer->contains_dynamic_body(proj_entity) == false)
			{
				continue;
			}

			// printf("\tx: %f, y: %f\n", projectile.body->GetPosition().x, projectile.body->GetPosition().y);

			b2_user_data ud = *(b2_user_data *)(projectile.body->GetFixtureList()->GetUserData().pointer);

			if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - ud.spawn_time).count() > 10000)
			{
				// timed out
				ud.type = b2fixture_types::EMPTY;
				if(ud.type == DEBRIS && projectile.debri_tile_type != 0)
				{
					game_engine::task_scheduler_pointer->add_task({&update_tile_task, new update_tile_params(projectile.body->GetPosition().x * game::box2d_scale, projectile.body->GetPosition().y *  game::box2d_scale, projectile.debri_tile_type, projectile.tile_temperature, projectile.tile_misc_data)});
				}
			}

			if (ud.type == b2fixture_types::EMPTY)
			{
				// projectile_mutex.unlock();
				projectiles.remove(proj_entity);
				bo_system_pointer->remove(proj_entity);
				render_system_pointer->remove(proj_entity);
				// free entity
				game_engine::game_engine_pointer->remove_entity(proj_entity);
				box2d_system_pointer->remove_dynamic_body(proj_entity);
				// projectile_mutex.lock();
				continue;
			}
			b2Vec2 position = projectile.body->GetPosition();
			position.x *= box2d_scale;
			position.y *= box2d_scale;
			
			if (ud.type == b2fixture_types::PROJECTILE)
			{
				game_engine::box_lerp b = bo_system_pointer->get(proj_entity);
				b.x = position.x - glsl_helper::projectile_width / 2.0f;
				b.y = position.y - glsl_helper::projectile_height / 2.0f;
				b.x2 = position.x - glsl_helper::projectile_width / 2.0f;
				b.y2 = position.y - glsl_helper::projectile_height / 2.0f;

				bo_system_pointer->update_box(proj_entity, b);
				game_engine::texture_vbo_system *tex_vbo_system_pointer = ((game_engine::texture_vbo_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::texture_vbo_system>()));
				tex_vbo_system_pointer->update(proj_entity, b.get_box());
			}
			else if (ud.type == b2fixture_types::DEBRIS)
			{
				tile_type tile = (tile_type)world_tiles->get_write_tile_at(position.x, position.y);
				tile_simple_type simple_tile = get_simple_tile_type(tile);
				tile_simple_type debris_simple_tile = get_simple_tile_type(projectile.debri_tile_type);


				if (projectile.permanent_trail_tile_type != 0 && simple_tile < debris_simple_tile)
				{
					world_tiles->set_tile_at_with_lock(position.x, position.y, projectile.permanent_trail_tile_type);
					world_tiles->set_tile_temperature_at(position.x, position.y, projectile.tile_temperature);
				}
				else if(projectile.permanent_trail_tile_type == 0 && debris_simple_tile <= simple_tile && std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - ud.spawn_time).count() > 500)
				{
					ud.type = b2fixture_types::EMPTY;
				}
				else if (projectile.temporary_trail_tile_type != 0)//&& world_tiles->get_tile_at(position.x, position.y) <= AIR)
				{
					tile_type current_tile = (tile_type)world_tiles->get_tile_at(position.x, position.y);
					if (get_simple_tile_type(current_tile) >= get_simple_tile_type(projectile.temporary_trail_tile_type))
					{
						continue;
					}
					world_tiles->set_tile_copy_at(position.x, position.y, projectile.temporary_trail_tile_type);
					int16_t tile_temperature = world_tiles->get_tile_temperature_at(position.x, position.y);
					int16_t new_temperature = tile_temperature + (projectile.tile_temperature - tile_temperature) / 20;
					world_tiles->set_tile_temperature_at(position.x, position.y, new_temperature);
				}
				
				// check if body still exists
				// if(box2d_system_pointer->world->GetBodyList().find(projectile.body) == nullptr)
				// {
				// 	remove_projectile(proj_entity);
				// 	bo_system_pointer->remove(proj_entity);
				// 	render_system_pointer->remove(proj_entity);
				// 	// free entity
				// 	game_engine::game_engine_pointer->remove_entity(proj_entity);
				// 	continue;
				// }

				// make debris fall slower if fall_speed_multiplier is set
				if (projectile.fall_speed_multiplier != 1.0f)
				{
					b2Vec2 vel = projectile.body->GetLinearVelocity();
					float target_y_vel = projectile.fall_speed_multiplier * projectile.fall_speed_multiplier;
					vel.y = vel.y + (target_y_vel - vel.y) / 32.0f;
					vel.x = vel.x * 0.98f;
					projectile.body->SetLinearVelocity(vel);
				}
			}
		}

		projectile_mutex.unlock();

		auto end = std::chrono::steady_clock::now();
		auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();
		add_time(elapsed);
	}
	projectile &projectile_system::create_projectile(entity ent, float x, float y, float ang, float vel, float radius, uint16_t millis_lifetime, b2fixture_types projectile_type)
	{
		b2d_mutex.lock();
		// create small circle projectile
		b2BodyDef body_def;
		body_def.type = b2_dynamicBody;
		body_def.position.Set(x / box2d_scale, y / box2d_scale);
		// body_def.angle = ang;
		box2d_system *box2d_system_pointer = (box2d_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<box2d_system>());
		b2Body *body = box2d_system_pointer->world->CreateBody(&body_def);
		box2d_system_pointer -> add_dynamic_body(ent, body);
		b2CircleShape circle;
		circle.m_radius = radius / box2d_scale;
		b2FixtureDef fixture_def;
		fixture_def.shape = &circle;
		fixture_def.density = 0.2f;
		fixture_def.friction = 0.3f;
		fixture_def.restitution = 0.89f;
		fixture_def.filter.categoryBits = projectile_type;

		if (projectile_type == b2fixture_types::DEBRIS)
		{
			fixture_def.filter.maskBits = b2fixture_types::TERRAIN | b2fixture_types::PLAYER | b2fixture_types::FLYING_CREATURE;
		}
		else
		{
			fixture_def.filter.maskBits = b2fixture_types::TERRAIN | b2fixture_types::PLAYER | b2fixture_types::FLYING_CREATURE;
		}

		b2FixtureUserData fixtureUserData;
		// fixtureUserData.pointer = b2fixture_types::PROJECTILE;
		b2_user_data *ud = new b2_user_data(ent, projectile_type);
		ud->set_lifetime(millis_lifetime);
		fixtureUserData.pointer = (uintptr_t) ud;
		fixture_def.userData = fixtureUserData;

		body->CreateFixture(&fixture_def);
		body->SetLinearVelocity(b2Vec2(vel * cos(ang), -vel * sin(ang)));
		b2d_mutex.unlock();
		
		projectile proj(body);
		projectile_mutex.lock();
		projectiles.add(ent, proj);
		projectile_mutex.unlock();
		return projectiles.get(ent);
	}

	void projectile_system::add_projectile(entity ent, projectile proj)
	{
		projectile_mutex.lock();
		projectiles.add(ent, proj);
		projectile_mutex.unlock();
	}

	/// @brief Remove the projectile from the system, and destroy the box2d body
	/// @param ent
	void projectile_system::remove_projectile(entity ent)
	{
		projectile_mutex.lock();
		// if userdata is not null, delete it
		if ((projectiles.get(ent).body)->GetUserData().pointer != 0)
		{
			delete (b2_user_data *)(projectiles.get(ent).body->GetUserData().pointer);
		}

		// destroy box2d body
		// b2d_mutex.lock();
		((box2d_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<box2d_system>()))->world->DestroyBody(projectiles.get(ent).body);
		// b2d_mutex.unlock();
		projectiles.remove(ent);
		projectile_mutex.unlock();
	}

	projectile &projectile_system::get_projectile(entity ent)
	{
		return projectiles.get(ent);
	}
}