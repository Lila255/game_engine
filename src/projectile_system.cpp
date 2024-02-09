#include "projectile_system.hpp"
#include "game_core.hpp"

namespace game
{
	void projectile_system::update(uint64_t time_to_step)
	{
		std::vector<entity> entities = projectiles.get_entities();
		// box2d_system *box2d_system_pointer = ((box2d_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<box2d_system>()));
		game_engine::box_system *bo_system_pointer = ((game_engine::box_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::box_system>()));
		game_engine::render_system *render_system_pointer = ((game_engine::render_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::render_system>()));

		// retrieve all projectiles' locations from the box2d world, and update their positions in the game
		// for (auto &proj : projectiles)
		for (auto &proj : entities)
		{
			b2_user_data *ud = (b2_user_data *)(projectiles.get(proj).body->GetFixtureList()->GetUserData().pointer);
			if (ud && ud->type == b2fixture_types::EMPTY)
			{

				remove_projectile(proj);
				bo_system_pointer->remove(proj);
				render_system_pointer->remove(proj);
				// free entity
				game_engine::game_engine_pointer->remove_entity(proj);

				continue;
			}
			b2Vec2 position = projectiles.get(proj).body->GetPosition();

			game_engine::box b = bo_system_pointer->get(proj);
			b.x = position.x - glsl_helper::projectile_width / 2.0f;
			b.y = position.y - glsl_helper::projectile_height / 2.0f;
			bo_system_pointer->update_box(proj, b);
			game_engine::texture_vbo_system *tex_vbo_system_pointer = ((game_engine::texture_vbo_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::texture_vbo_system>()));
			tex_vbo_system_pointer->update(proj);
		}
	}
	b2Body *projectile_system::create_projectile(entity ent, float x, float y, float ang, float vel, float radius)
	{
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

		b2FixtureUserData fixtureUserData;
		// fixtureUserData.pointer = b2fixture_types::PROJECTILE;
		fixtureUserData.pointer = (uintptr_t) new b2_user_data(ent, b2fixture_types::PROJECTILE);
		fixture_def.userData = fixtureUserData;

		b2d_mutex.lock();
		body->CreateFixture(&fixture_def);
		body->SetLinearVelocity(b2Vec2(vel * cos(ang), -vel * sin(ang)));
		b2d_mutex.unlock();
		projectile proj(body);
		projectiles.add(ent, proj);
		return body;
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