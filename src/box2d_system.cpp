#include "box2d_system.hpp"
#include "tasks.hpp"

namespace game
{
	std::mutex b2d_mutex;
	float box2d_scale = 10.0f;

	box2d_system::box2d_system()
	{
		gravity = b2Vec2(0.0f, 19.81f);
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
			vertices[i].Set(mesh[i].first / box2d_scale, mesh[i].second / box2d_scale);
		}
		chain.CreateLoop(vertices, (int32)(mesh.size()));
		b2FixtureDef fixtureDef;
		fixtureDef.shape = &chain;
		fixtureDef.density = 0.0f;
		fixtureDef.friction = 0.173f;
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
		game::b2d_mutex.lock();
		b2Body *body = world->CreateBody(&bodyDef);
		game::b2d_mutex.unlock();
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
					vertices[k].Set(meshes->at(i)[j + k].first / box2d_scale, meshes->at(i)[j + k].second / box2d_scale);
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
				fixtureDef.friction = 0.173f;
				fixtureDef.restitution = 0.15f;

				fixtureDef.filter.categoryBits = b2fixture_types::TERRAIN;

				b2FixtureUserData fixtureUserData;
				// fixtureUserData.pointer = b2fixture_types::TERRAIN;
				fixtureUserData.pointer = (uintptr_t) new b2_user_data(ent, b2fixture_types::TERRAIN);
				fixtureDef.userData = fixtureUserData;
				game::b2d_mutex.lock();
				body->CreateFixture(&fixtureDef);
				game::b2d_mutex.unlock();
				delete[] vertices;
			}
		}
		static_bodies.add(ent, body);
	}

	void box2d_system::update_static_outlines(entity ent, std::vector<std::vector<std::pair<float, float>>> *meshes)
	{
		if (!static_bodies.contains(ent))
		{
			create_static_bodies(ent, meshes);
			return;
		}

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
			{
				continue;
			}
			// if (meshes[i].size() < 3)
			//     continue;
			for (int j = 0; j < meshes->at(i).size(); j += 3)
			{
				b2PolygonShape chain;
				b2Vec2 *vertices = new b2Vec2[3];
				for (int k = 0; k < 3; k++)
				{
					vertices[k].Set(meshes->at(i)[j + k].first / box2d_scale, meshes->at(i)[j + k].second / box2d_scale);
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
				fixtureDef.friction = 0.29f;
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
		game::b2d_mutex.lock();
		world->DestroyBody(static_bodies.get(ent));
		static_bodies.remove(ent);
		game::b2d_mutex.unlock();
	}

	/// @brief Creates a dynamic body with the provided entity and mesh.
	/// @param ent 
	/// @param mesh Mesh is in a squashed vector of triangle co-ordinates
	void box2d_system::create_dynamic_body(entity ent, std::vector<std::pair<float, float>> mesh)
	{
		// // do this ^^^ but get the shape from the mesh paramater
		b2BodyDef body_def;
		body_def.type = b2_dynamicBody;
		body_def.position.Set(90.0f / box2d_scale, 20.0f / box2d_scale);
		body_def.fixedRotation = true;
		b2Body *body = world->CreateBody(&body_def);
		// use mesh

		for (int i = 0; i < mesh.size(); i += 3)
		{
			b2PolygonShape dynamic_box;
			b2Vec2 *vertices = new b2Vec2[3];
			for (int j = 0; j < 3; j++)
			{
				vertices[j].Set(mesh[i + j].first / box2d_scale, mesh[i + j].second / box2d_scale);
			}
			// if straight line, skip (poor check, only works for axis aligned lines)
			if (vertices[0].x == vertices[1].x && vertices[0].x == vertices[2].x)
				continue;
			if (vertices[0].y == vertices[1].y && vertices[0].y == vertices[2].y)
				continue;
			dynamic_box.Set(vertices, 3);
			b2FixtureDef fixture_def;
			fixture_def.shape = &dynamic_box;
			fixture_def.density = 1.5f;
			fixture_def.friction = 0.25f;
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

	void box2d_system::add_mesh(entity ent,  std::vector<std::pair<float, float>> mesh, b2fixture_types mesh_type)
	{
		b2Body *body = dynamic_bodies.get(ent);
		for (int i = 0; i < mesh.size(); i += 3)
		{
			b2PolygonShape dynamic_box;
			b2Vec2 *vertices = new b2Vec2[3];
			for (int j = 0; j < 3; j++)
			{
				vertices[j].Set(mesh[i + j].first / box2d_scale, mesh[i + j].second / box2d_scale);
			}
			// if straight line, skip (poor check, only works for axis aligned lines)
			if (vertices[0].x == vertices[1].x && vertices[0].x == vertices[2].x)
				continue;
			if (vertices[0].y == vertices[1].y && vertices[0].y == vertices[2].y)
				continue;
			dynamic_box.Set(vertices, 3);
			b2FixtureDef fixture_def;
			fixture_def.shape = &dynamic_box;
			fixture_def.density = 1.5f;
			fixture_def.friction = 0.25f;
			fixture_def.restitution = .001f;
			fixture_def.filter.categoryBits = mesh_type;
			b2FixtureUserData fixtureUserData;
			// fixtureUserData.pointer = b2fixture_types::PLAYER;
			fixtureUserData.pointer = (uintptr_t) new b2_user_data(ent, mesh_type);
			fixture_def.userData = fixtureUserData;
			body->CreateFixture(&fixture_def);
			delete[] vertices;
		}
		
	}

	void box2d_system::add_dynamic_body(entity ent, b2Body *body)
	{
		dynamic_bodies.add(ent, body);
	}

	void box2d_system::remove_dynamic_body(entity ent)
	{
		// b2d_mutex.lock();
		world->DestroyBody(dynamic_bodies.get(ent));
		// b2d_mutex.unlock();
		dynamic_bodies.remove(ent);
	}

	void box2d_system::start_thread() 
	{
		running = 1;

		std::chrono::time_point<std::chrono::steady_clock> last_time = std::chrono::steady_clock::now();
		while(running)
		{	
			std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();
			b2d_mutex.lock();
			std::chrono::time_point<std::chrono::steady_clock> before_update = std::chrono::steady_clock::now();
			update(time_step_ms);
			b2d_mutex.unlock();
			std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();
			std::chrono::microseconds elapsed_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
			std::chrono::microseconds update_duration = std::chrono::duration_cast<std::chrono::microseconds>(end - before_update);
			add_time(elapsed_ms.count());
			// if(get_counter() % (1) == 0) printf("Elapsed time(microseconds): %ld\tUpdate duration: %ld\n", elapsed_ms.count(), update_duration.count());
			// elapsed_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - last_time);
			// if(get_counter() % (10) == 0)
			// 	printf("Elapsed time(microseconds): %ld\tUpdate duration: %ld\n", elapsed_ms.count(), update_duration.count());

			// last_time = end;
			if(elapsed_ms.count() < time_step_ms * 1000 * .8)
			{
				std::this_thread::sleep_for(std::chrono::microseconds((uint64_t)(time_step_ms * 1000 * .8 - elapsed_ms.count())));
			}
		}
	}

	void box2d_system::stop_thread()
	{
		running = 0;
	}

	void box2d_system::update(uint64_t time_to_step)
	{
		increment_counter();
		game_engine::box_system *bo_system_pointer = ((game_engine::box_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::box_system>()));

		game_engine::box_lerp player_box_0 = bo_system_pointer->get(game_engine::game_engine_pointer->player_entitiy);
		
		world->Step(time_to_step / 1000.f, 6, 2);
		// world->Step(1.0f / 144.0f, 6, 2);

		if(2 == 1 && get_counter() % 10 == 0)
		{
			// sum static body mesh vertices
			uint64_t static_vertex_count = 0;
			for (auto &ent : static_bodies.get_entities())
			{
				b2Body *body = static_bodies.get(ent);
				b2Fixture *fixtures = body->GetFixtureList();
				while (fixtures)
				{
					b2Shape::Type shapeType = fixtures->GetType();
					if (shapeType == b2Shape::e_polygon)
					{
						b2PolygonShape* polygonShape = (b2PolygonShape*)fixtures->GetShape();
						static_vertex_count += polygonShape->m_count;
					}
					// else if (shapeType == b2Shape::e_chain)
					// {
					// 	b2ChainShape* chainShape = (b2ChainShape*)fixtures->GetShape();
					// 	static_vertex_count += chainShape->m_count;
					// }
					fixtures = fixtures->GetNext();
				}
			}
			printf("Static body total vertex count: %llu\n", static_vertex_count);

			printf("Dynamic body count: %llu\n", dynamic_bodies.get_entities().size());

			// check all dynamic bodies to find if any have exceeded an amount of intersecionts/collisions
			for (auto &ent : dynamic_bodies.get_entities())
			{
				b2Body *body = dynamic_bodies.get(ent);
				int contact_count = 0;
				for (b2ContactEdge* ce = body->GetContactList(); ce; ce = ce->next)
				{
					if (ce->contact->IsTouching())
					{
						contact_count++;
					}
				}
				// if contact count > threshold, do something
				// printf("Entity %llu has %d contacts\n", ent.get_id(), contact_count);
				if (contact_count > 10)
				{
					printf("Entity %llu has exceeded contact threshold with %d contacts\n", ent, contact_count);
					// take some action, e.g., remove body or reset position
					// disable bodies collision for a while
					body->SetEnabled(false);
					// set position
					body->SetTransform(b2Vec2(90.0f / box2d_scale, 20.0f / box2d_scale), 0.0f);
					
					// insert task
					game_engine::task_scheduler_pointer -> add_backlog_task(game_engine::task{
						.function_pointer = game::set_b2d_body_enabled_task,
						.task_parameters = new game::set_b2d_body_enabled_params(ent, true),
						.run_time = 0,
						.retry_count = 0
					}, 8000); // re-enable after 8 seconds


				
				}
			}
		}

		b2Body *body = dynamic_bodies.get(game_engine::game_engine_pointer->player_entitiy);
		b2Vec2 position = body->GetPosition();

		// get player position
		// b2Body *body = dynamic_bodies.get(game_engine::game_engine_pointer->player_entitiy);
		// b2Vec2 position = body->GetPosition();
		// printf("x: %f, y: %f\n", position.x, position.y);
		// // get box position
		game_engine::box_lerp b = bo_system_pointer->get(game_engine::game_engine_pointer->player_entitiy);
		b.x2 = position.x * box2d_scale; // - glsl_helper::character_width / 2.0f;
		b.y2 = position.y * box2d_scale; // - glsl_helper::character_height / 2.0f;
		b.x = player_box_0.x2;
		b.y = player_box_0.y2;
		b.t0 = player_box_0.t1;
		b.t1 = std::chrono::time_point_cast<std::chrono::microseconds>(std::chrono::steady_clock::now()).time_since_epoch().count();

		bo_system_pointer->update_box(game_engine::game_engine_pointer->player_entitiy, b);
		game_engine::texture_vbo_system *tex_vbo_system_pointer = ((game_engine::texture_vbo_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::texture_vbo_system>()));
		// tex_vbo_system_pointer->update(game_engine::game_engine_pointer->player_entitiy, b.get_box());
	}

	b2Body *box2d_system::get_static_body(entity ent)
	{
		return static_bodies.get(ent);
	}

	b2Body *box2d_system::get_dynamic_body(entity ent)
	{
		return dynamic_bodies.get(ent);
	}

	bool box2d_system::contains_static_body(entity ent)
	{
		return static_bodies.contains(ent);
	}
	bool box2d_system::contains_dynamic_body(entity ent)
	{
		return dynamic_bodies.contains(ent);
	}

	std::vector<entity> box2d_system::get_static_body_entities()
	{
		return static_bodies.get_entities();
	}
}