#include "flying_creature_system.hpp"
// #include ""

namespace game
{
	void flying_creature_system::start_thread()
	{
		printf("Starting flying creature system thread\n");
		running = 1;
		uint64_t step_count = 0;

		while(running)
		{	
			std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();
			b2d_mutex.lock();
			
			update(step_count++);
			b2d_mutex.unlock();
			std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();
			std::chrono::microseconds elapsed_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
			// printf("Elapsed time: %ld\n", elapsed_ms.count());
			if(elapsed_ms.count() < time_step_ms * 1000)
			{
				std::this_thread::sleep_for(std::chrono::microseconds((time_step_ms * 1000 - elapsed_ms.count())));
			}
		}
	}

	void flying_creature_system::update(uint64_t step_num)
	{
		std::unordered_map<uint64_t, std::string> * task_names = new std::unordered_map<uint64_t, std::string>();
		
		task_scheduler_pointer -> add_task({game_engine::task_scheduler::print_task_counter, 0});
		std::vector<entity> entities = flying_creatures.get_entities();


		double noise_1 = perlin_noise__bees.noise1D_01(step_num / 10.0);
		double noise_2 = perlin_noise__bees.noise1D_01((step_num + 10000) / 10.0);

		for (auto &ent : entities)
		{
			flying_creature &creature = flying_creatures.get(ent);
			b2Vec2 force = b2Vec2(0, 0);
			b2Body *body = b2d_system->get_dynamic_body(ent);
			
			switch(creature.get_state())
			{
				case flying_creature_state::IDLE:
					// creature is idle, find job
					if(creature.get_collected_mass() > 10)
					{
						creature.set_state(flying_creature_state::RETRIEVING);
						break;
					}
					// find food
					
					creature.set_state(flying_creature_state::EATING);
					break;

				case flying_creature_state::EATING:
					// creature is eating
					
					
					force = b2Vec2(10 * (noise_1 - 0.5), 10 * (noise_2 - 0.5));
					body->ApplyForceToCenter(force, true);

					if(creature.get_collected_mass() > 10)
					{
						creature.set_state(flying_creature_state::RETRIEVING);
					}
					break;
				
				case flying_creature_state::RETRIEVING:
				
					if(creature.get_collected_mass() == 0)
					{
						creature.set_state(flying_creature_state::IDLE);
						break;						
					}
					// creature is retrieving
					// if creature has collided with wax, deposit collected mass
					b2Vec2 position = body->GetPosition();
					std::pair<int, int> tile_pos = {position.x * box2d_scale, position.y * box2d_scale};


					if(creature.target_home.first > tile_pos.first)
					{
						force.x = 1;
					}
					else if(creature.target_home.first < tile_pos.first)
					{
						force.x = -1;
					}

					if(creature.target_home.second > tile_pos.second)
					{
						force.y = 1;
					}
					else if(creature.target_home.second < tile_pos.second)
					{
						force.y = -1;
					}
					body->ApplyForceToCenter(force, true);
					break;
			}
		}
	}

	void flying_creature_system::update_rendering(uint64_t time_to_step)
	{
		// for(auto creature: flying_creature_system::flying_creatures)
		// {
		// 	// update creature
		// 	// if creature has no target, find target
		// }
		game_engine::task t;
		while(task_scheduler_pointer->pop_task(t))
		{
			t.run();
		}


		std::vector<entity> entities = flying_creatures.get_entities();
		for (auto &ent : entities)
		{
			flying_creature &creature = flying_creatures.get(ent);
			// printf("creature type: %d\n", creature.type);
			b2Body *body = b2d_system->get_dynamic_body(ent);

			// b2_user_data *ud = (b2_user_data *)(body->GetFixtureList()->GetUserData().pointer);

			b2Vec2 position = body->GetPosition();
			position.x *= box2d_scale;
			position.y *= box2d_scale;

			game_engine::box_lerp b = box_sys->get(ent);
			b.x = position.x - 2 / 2.0f;
			b.y = position.y - 2 / 2.0f;
			b.x2 = position.x - 2 / 2.0f;
			b.y2 = position.y - 2 / 2.0f;
			
			box_sys->update_box(ent, b);
			texture_vbo_sys->update(ent, b.get_box());

			if(creature.is_flying)
			{
				// set velocity
				// body->SetLinearVelocity(b2Vec2((rand() % 100) / 100.f - 0.5f, (rand() % 100) / 100.f - 0.5f));
				
				//get defs
				// b2Fixture *fista1 = body->GetFixtureList();
				body->ApplyForceToCenter(body->GetMass() * -0.38 * (b2d_system->world->GetGravity()) , true);
				// sta2def.gravityScale = 0.0f;
			}
		}
	}

	
	void flying_creature_system::add(entity ent, flying_creature &creature)
	{
		flying_creature_system::flying_creatures.add(ent, creature);
	}

	flying_creature& flying_creature_system::get(entity ent)
	{
		return flying_creature_system::flying_creatures.get(ent);
	}

	
	void flying_creature_system::create_flying_creature(entity ent, float x, float y, flying_creature_type type)
	{
		// flying_creature_system::b2d_system = flying_creature_system::b2d_system ? flying_creature_system::b2d_system : (box2d_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<box2d_system>());
		
		b2d_mutex.lock();
		// create small circle projectile
		b2BodyDef body_def;
		body_def.type = b2_dynamicBody;
		body_def.position.Set(x / box2d_scale, y / box2d_scale);

		b2Body *body = flying_creature_system::b2d_system->world->CreateBody(&body_def);
		flying_creature_system::b2d_system->add_dynamic_body(ent, body);
		// flying_creature_system::b2d_system->add_mesh(ent, {{0, -1}, {1, 0}, {0, 1}, {0, -1}, {0, 1}, {-1, 0}}, b2fixture_types::FLYING_CREATURE);
		b2CircleShape circle;
		circle.m_radius = 2 / box2d_scale;
		b2FixtureDef fixture_def;
		fixture_def.shape = &circle;
		fixture_def.density = 0.2f;
		fixture_def.friction = 0.3f;
		fixture_def.restitution = 0.89f;
		fixture_def.filter.categoryBits = b2fixture_types::FLYING_CREATURE;

		b2_user_data *ud = new b2_user_data(ent, b2fixture_types::FLYING_CREATURE);
		
		b2FixtureUserData fixtureUserData;
		fixtureUserData.pointer = (uintptr_t) ud;
		fixture_def.userData = fixtureUserData;

		body->CreateFixture(&fixture_def);
		body->SetLinearVelocity(b2Vec2(0.f, -1.f));
		
		GLuint creature_texture = glsl_helper::texture_map["bee"];
		// GLuint creature_texture = glsl_helper::texture_map["bee"];

		game_engine::sprite sprt(game_engine::shader_programs[0]);
		sprt.add_texture({creature_texture, 0, GL_R8, 2, 2});

		render_sys->add(ent, sprt);

		box_sys->add(ent, {0, 0, -4.6f, 4, 4});
		texture_vbo_sys->add(ent);

		flying_creature creature = {type};
		flying_creatures.add(ent, creature);

		b2d_mutex.unlock();

		// flying_creature_system::b2d_system -> create_dynamic_body(ent, {{0, -1}, {1, 0}, {0, 1}, {0, -1}, {0, 1}, {-1, 0}});
		// game_engine::box2d_body = 
	}

	void flying_creature_system::add_task(game_engine::task t)
	{
		printf("%d\n", (uint64_t)task_scheduler_pointer);
		task_scheduler_pointer->add_task(t);
	}

}