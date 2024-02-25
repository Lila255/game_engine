#include "projectile_system.hpp"
#include "box2d_system.hpp"
#include "world_tile_system.hpp"


namespace game
{
	void projectile_system::update(uint64_t time_to_step)
	{
		std::vector<entity> entities = projectiles.get_entities();
		// box2d_system *box2d_system_pointer = ((box2d_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<box2d_system>()));
		game_engine::box_system *bo_system_pointer = ((game_engine::box_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::box_system>()));
		game_engine::render_system *render_system_pointer = ((game_engine::render_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::render_system>()));
		world_tile_system *world_tiles = ((world_tile_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<world_tile_system>()));
		// retrieve all projectiles' locations from the box2d world, and update their positions in the game
		// for (auto &proj : projectiles)
		for (auto &proj_entity : entities)
		{
			projectile & projectile = projectiles.get(proj_entity);

			b2_user_data *ud = (b2_user_data *)(projectile.body->GetFixtureList()->GetUserData().pointer);
			if (ud && ud->type == b2fixture_types::EMPTY)
			{
				remove_projectile(proj_entity);
				bo_system_pointer->remove(proj_entity);
				render_system_pointer->remove(proj_entity);
				// free entity
				game_engine::game_engine_pointer->remove_entity(proj_entity);

				continue;
			}
			b2Vec2 position = projectile.body->GetPosition();

			if(ud->type == b2fixture_types::PROJECTILE)
			{
				game_engine::box b = bo_system_pointer->get(proj_entity);
				b.x = position.x - glsl_helper::projectile_width / 2.0f;
				b.y = position.y - glsl_helper::projectile_height / 2.0f;
				bo_system_pointer->update_box(proj_entity, b);
				game_engine::texture_vbo_system *tex_vbo_system_pointer = ((game_engine::texture_vbo_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::texture_vbo_system>()));
				tex_vbo_system_pointer->update(proj_entity);
			} else if(ud->type == b2fixture_types::DEBRIS)
			{
				if(projectile.trail_tile_type != 0 && world_tiles->get_tile_at(position.x, position.y) == 0)
				{
					world_tiles->set_tile_at_with_lock(position.x, position.y, projectile.trail_tile_type);
				}
			}
		}
	}
	projectile &projectile_system::create_projectile(entity ent, float x, float y, float ang, float vel, float radius, b2fixture_types projectile_type)
	{
		b2d_mutex.lock();
		// create small circle projectile
		b2BodyDef body_def;
		body_def.type = b2_dynamicBody;
		body_def.position.Set(x, y);
		// body_def.angle = ang;
		b2Body *body = ((box2d_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<box2d_system>()))->world->CreateBody(&body_def);
		b2CircleShape circle;
		circle.m_radius = radius;
		b2FixtureDef fixture_def;
		fixture_def.shape = &circle;
		fixture_def.density = 0.2f;
		fixture_def.friction = 0.3f;
		fixture_def.restitution = 0.89f;
		fixture_def.filter.categoryBits = projectile_type;

		if(projectile_type == b2fixture_types::DEBRIS)
		{
			fixture_def.filter.maskBits = b2fixture_types::TERRAIN | b2fixture_types::PLAYER;
		} else {
			fixture_def.filter.maskBits = b2fixture_types::TERRAIN | b2fixture_types::PLAYER;
		}

		b2FixtureUserData fixtureUserData;
		// fixtureUserData.pointer = b2fixture_types::PROJECTILE;
		fixtureUserData.pointer = (uintptr_t) new b2_user_data(ent, projectile_type);
		fixture_def.userData = fixtureUserData;

		body->CreateFixture(&fixture_def);
		body->SetLinearVelocity(b2Vec2(vel * cos(ang), -vel * sin(ang)));
		b2d_mutex.unlock();
		projectile proj(body);
		projectiles.add(ent, proj);
		return projectiles.get(ent);
	}

	void projectile_system::add_projectile(entity ent, projectile proj)
	{
		projectiles.add(ent, proj);
	}

	/// @brief Remove the projectile from the system, and destroy the box2d body
	/// @param ent
	void projectile_system::remove_projectile(entity ent)
	{
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
	}

	projectile &projectile_system::get_projectile(entity ent)
	{
		return projectiles.get(ent);
	}
}