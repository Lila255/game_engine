#include "box2d_system.hpp"

namespace game
{
	std::mutex b2d_mutex;

	box2d_system::box2d_system()
	{
		gravity = b2Vec2(0.0f, 91.8f);
		world = new b2World(gravity);
		// contact_listener = new b2_contact_listener();
	}

	box2d_system::~box2d_system()
	{
		// delete user data from fixtures
		for (auto &ent : static_bodies.get_entities())
		{
			b2Body *body = static_bodies.get(ent);
			b2Fixture *fixtures = body->GetFixtureList();
			while (fixtures)
			{
				b2Fixture *next = fixtures->GetNext();
				if (fixtures->GetUserData().pointer != 0)
					delete (b2_user_data *)(fixtures->GetUserData().pointer);
				fixtures = next;
			}
		}
		for (auto &ent : dynamic_bodies.get_entities())
		{
			b2Body *body = dynamic_bodies.get(ent);
			b2Fixture *fixtures = body->GetFixtureList();
			while (fixtures)
			{
				b2Fixture *next = fixtures->GetNext();
				if (fixtures->GetUserData().pointer != 0)
					delete (b2_user_data *)(fixtures->GetUserData().pointer);
				fixtures = next;
			}
		}

		delete world;
	}

	void box2d_system::create_static_body(entity ent, std::vector<std::pair<float, float>> mesh)
	{

		if (mesh.size() < 3)
			return;
		// static_bodies.add(ent, body);
		b2BodyDef bodyDef;
		bodyDef.type = b2_staticBody;
		b2Body *body = world->CreateBody(&bodyDef);
		// add mesh
		b2ChainShape chain;
		b2Vec2 *vertices = new b2Vec2[mesh.size()];
		for (int i = 0; i < mesh.size(); i++)
		{
			vertices[i].Set(mesh[i].first, mesh[i].second);
		}
		chain.CreateLoop(vertices, (int32)(mesh.size()));
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &chain;
		fixtureDef.density = 0.0f;
		fixtureDef.friction = 0.73f;
		fixtureDef.restitution = 0.15f;
		fixtureDef.filter.categoryBits = b2fixture_types::TERRAIN;
		b2FixtureUserData fixtureUserData;
		fixtureUserData.pointer = (uintptr_t) new b2_user_data(ent, b2fixture_types::TERRAIN);
		fixtureDef.userData = fixtureUserData;
		body->CreateFixture(&fixtureDef);
		static_bodies.add(ent, body);
	}

	void box2d_system::create_static_bodies(entity ent, std::vector<std::vector<std::pair<float, float>>> *meshes)
	{
		b2BodyDef bodyDef;
		bodyDef.type = b2_staticBody;
		b2Body *body = world->CreateBody(&bodyDef);
		// add mesh
		for (int i = 0; i < meshes->size(); i++)
		{
			if (meshes->at(i).size() == 0 || meshes->at(i).size() % 3 != 0)
				continue;
			// if (meshes[i].size() < 3)
			//     continue;
			for (int j = 0; j < meshes->at(i).size(); j += 3)
			{
				b2PolygonShape chain;
				b2Vec2 *vertices = new b2Vec2[3];
				for (int k = 0; k < 3; k++)
				{
					vertices[k].Set(meshes->at(i)[j + k].first, meshes->at(i)[j + k].second);
				}
				// if straight line, skip (poor check, only works for axis aligned lines)
				if (vertices[0].x == vertices[1].x && vertices[0].x == vertices[2].x)
					continue;
				if (vertices[0].y == vertices[1].y && vertices[0].y == vertices[2].y)
					continue;

				chain.Set(vertices, 3);
				b2FixtureDef fixtureDef;
				fixtureDef.shape = &chain;
				fixtureDef.density = 0.0f;
				fixtureDef.friction = 0.73f;
				fixtureDef.restitution = 0.15f;

				fixtureDef.filter.categoryBits = b2fixture_types::TERRAIN;

				b2FixtureUserData fixtureUserData;
				// fixtureUserData.pointer = b2fixture_types::TERRAIN;
				fixtureUserData.pointer = (uintptr_t) new b2_user_data(ent, b2fixture_types::TERRAIN);
				fixtureDef.userData = fixtureUserData;
				body->CreateFixture(&fixtureDef);
				delete[] vertices;
			}
		}
		static_bodies.add(ent, body);
	}

	void box2d_system::update_static_outlines(entity ent, std::vector<std::vector<std::pair<float, float>>> *meshes)
	{
		if (!static_bodies.contains(ent))
			return;

		// printf("Here, locking now\n");
		game::b2d_mutex.lock();
		// printf("Locked\n");
		b2Body *body = static_bodies.get(ent);
		b2Fixture *fixtures = body->GetFixtureList();
		while (fixtures)
		{
			b2Fixture *next = fixtures->GetNext();
			delete (b2_user_data *)(fixtures->GetUserData().pointer);
			body->DestroyFixture(fixtures);
			// fixtures = body->GetFixtureList();
			fixtures = next;
		}
		// add mesh
		for (int i = 0; i < meshes->size(); i++)
		{
			if (meshes->at(i).size() == 0 || meshes->at(i).size() % 3 != 0)
				continue;
			// if (meshes[i].size() < 3)
			//     continue;
			for (int j = 0; j < meshes->at(i).size(); j += 3)
			{
				b2PolygonShape chain;
				b2Vec2 *vertices = new b2Vec2[3];
				for (int k = 0; k < 3; k++)
				{
					vertices[k].Set(meshes->at(i)[j + k].first, meshes->at(i)[j + k].second);
				}
				// if straight line, skip (poor check, only works for axis aligned lines)
				if (vertices[0].x == vertices[1].x && vertices[0].x == vertices[2].x)
					continue;
				if (vertices[0].y == vertices[1].y && vertices[0].y == vertices[2].y)
					continue;

				chain.Set(vertices, 3);
				b2FixtureDef fixtureDef;
				fixtureDef.shape = &chain;
				fixtureDef.density = 1.0f;
				fixtureDef.friction = 0.9f;
				fixtureDef.restitution = 0.15f;
				fixtureDef.filter.categoryBits = b2fixture_types::TERRAIN;

				b2FixtureUserData fixtureUserData;
				// fixtureUserData.pointer = b2fixture_types::TERRAIN;
				fixtureUserData.pointer = (uintptr_t) new b2_user_data(ent, b2fixture_types::TERRAIN);
				fixtureDef.userData = fixtureUserData;
				body->CreateFixture(&fixtureDef);
				delete[] vertices;
			}
		}
		game::b2d_mutex.unlock();
	}

	void box2d_system::remove_static_body(entity ent)
	{
		world->DestroyBody(static_bodies.get(ent));
		static_bodies.remove(ent);
	}

	void box2d_system::create_dynamic_body(entity ent, std::vector<std::pair<float, float>> mesh)
	{
		// // do this ^^^ but get the shape from the mesh paramater
		b2BodyDef body_def;
		body_def.type = b2_dynamicBody;
		body_def.position.Set(90.0f, 20.0f);
		body_def.fixedRotation = true;
		b2Body *body = world->CreateBody(&body_def);
		// use mesh

		for (int i = 0; i < mesh.size(); i += 3)
		{
			b2PolygonShape dynamic_box;
			b2Vec2 *vertices = new b2Vec2[3];
			for (int j = 0; j < 3; j++)
			{
				vertices[j].Set(mesh[i + j].first, mesh[i + j].second);
			}
			// if straight line, skip (poor check, only works for axis aligned lines)
			if (vertices[0].x == vertices[1].x && vertices[0].x == vertices[2].x)
				continue;
			if (vertices[0].y == vertices[1].y && vertices[0].y == vertices[2].y)
				continue;
			dynamic_box.Set(vertices, 3);
			b2FixtureDef fixture_def;
			fixture_def.shape = &dynamic_box;
			fixture_def.density = 0.5f;
			fixture_def.friction = 0.5f;
			fixture_def.restitution = .001f;
			fixture_def.filter.categoryBits = b2fixture_types::PLAYER;
			b2FixtureUserData fixtureUserData;
			// fixtureUserData.pointer = b2fixture_types::PLAYER;
			fixtureUserData.pointer = (uintptr_t) new b2_user_data(ent, b2fixture_types::PLAYER);
			fixture_def.userData = fixtureUserData;
			body->CreateFixture(&fixture_def);
			delete[] vertices;
		}
		dynamic_bodies.add(ent, body);
	}

	void box2d_system::remove_dynamic_body(entity ent)
	{
		b2d_mutex.lock();
		world->DestroyBody(dynamic_bodies.get(ent));
		b2d_mutex.unlock();
		dynamic_bodies.remove(ent);
	}

	void box2d_system::update(uint64_t time_to_step)
	{

		world->Step(time_to_step / 1000000.f, 6, 2);
		// world->Step(1.0f / 144.0f, 6, 2);

		b2Body *body = dynamic_bodies.get(game_engine::game_engine_pointer->player_entitiy);
		b2Vec2 position = body->GetPosition();

		// get player position
		// b2Body *body = dynamic_bodies.get(game_engine::game_engine_pointer->player_entitiy);
		// b2Vec2 position = body->GetPosition();
		// printf("x: %f, y: %f\n", position.x, position.y);
		// // get box position
		game_engine::box_system *bo_system_pointer = ((game_engine::box_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::box_system>()));
		game_engine::box b = bo_system_pointer->get(game_engine::game_engine_pointer->player_entitiy);
		b.x = position.x; // - glsl_helper::character_width / 2.0f;
		b.y = position.y; // - glsl_helper::character_height / 2.0f;
		bo_system_pointer->update_box(game_engine::game_engine_pointer->player_entitiy, b);
		game_engine::texture_vbo_system *tex_vbo_system_pointer = ((game_engine::texture_vbo_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::texture_vbo_system>()));
		tex_vbo_system_pointer->update(game_engine::game_engine_pointer->player_entitiy);
	}

	b2Body *box2d_system::get_static_body(entity ent)
	{
		return static_bodies.get(ent);
	}

	b2Body *box2d_system::get_dynamic_body(entity ent)
	{
		return dynamic_bodies.get(ent);
	}
}