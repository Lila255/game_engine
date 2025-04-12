#include "legged_creature_system.hpp"
#include "box2d_system.hpp"
// #include ""

namespace game
{
	void legged_creature_system::start_thread()
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

	void legged_creature_system::update(uint64_t step_num)
	{
		std::unordered_map<uint64_t, std::string> * task_names = new std::unordered_map<uint64_t, std::string>();
		
		// task_scheduler_pointer -> add_task({game_engine::task_scheduler::print_task_counter, 0});
		std::vector<entity> entities = legged_creatures.get_entities();

		// get player position
		b2Vec2 player_pos = b2d_system -> get_dynamic_body(game_engine::game_engine_pointer -> player_entitiy) -> GetPosition();

		double noise_1 = perlin_noise_spider.noise1D_01(step_num / 10.0);
		double noise_2 = perlin_noise_spider.noise1D_01((step_num + 10000) / 10.0);

		for (auto &ent : entities)
		{
			legged_creature &creature = legged_creatures.get(ent);
			b2Vec2 force = b2Vec2(0, 0);
			b2Body *body = b2d_system->get_dynamic_body(ent);
			b2Vec2 legged_creature_pos = body->GetPosition();
			
			switch(creature.get_state())
			{
				case legged_creature_state::WALKING:
					{
						uint64_t connected_legs = creature.connected_legs.size();
						if(!connected_legs)
						{
							creature.set_state(legged_creature_state::NOT_WALKING);
							continue;
						}

						// check if creature is close to player
						if(creature.head_entity == 0 && b2Distance(legged_creature_pos, player_pos) < 1)
						{
							creature.set_state(legged_creature_state::WATCHING);
							continue;
						}


						// check if feet should be unstuck
						for(int leg_i = 0; leg_i < creature.legs.size(); leg_i++)
						{
							if(creature.connected_legs.size() == 1)
								break;

							legged_creature_leg &l = creature.legs[leg_i];
							// move leg
							entity foot_ent = l.foot_entity;
							b2Body * foot_body = b2d_system -> get_dynamic_body(foot_ent);

							float ang_to_target = atan2(player_pos.y - legged_creature_pos.y, player_pos.x - legged_creature_pos.x);

							// move foot away from body, if not connected
							if(!(creature.connected_legs.count(leg_i)))
							{
								// apply impulse to foot towards the player
								b2Vec2 impulse = player_pos - foot_body -> GetPosition();
								impulse.Normalize();

								// make each leg a bit different



								impulse *= 0.16;
								foot_body -> ApplyLinearImpulseToCenter(impulse, true);

				
								continue;
							}

							b2Vec2 foot_pos = foot_body -> GetPosition();
							// calculate difference of angle between foot and body and target
							float ang_to_foot = atan2(foot_pos.y - legged_creature_pos.y, foot_pos.x - legged_creature_pos.x);

							// if angle is too high, disconnect leg
							if(abs(ang_to_foot - ang_to_target) > 1.25)
							{
								foot_body -> SetType(b2_dynamicBody);
								creature.connected_legs.erase(leg_i);
							}
						}

						// walking
						// move towards the player
						b2Vec2 legged_creature_pos = body -> GetPosition();

						b2Vec2 body_target;
						if (creature.head_entity)
						{
							body_target = b2d_system -> get_dynamic_body(creature.head_entity) -> GetPosition();
						}
						else
						{
							body_target = player_pos;
						}
						
						b2Vec2 target_vector = body_target - legged_creature_pos;
						target_vector.Normalize();

						// apply force to body
						force = target_vector;
						force *= .12 * connected_legs;
						body -> ApplyLinearImpulseToCenter(force, true);
						break;
					}
				case legged_creature_state::NOT_WALKING:
					{
						uint64_t connected_legs = creature.connected_legs.size();
						if(connected_legs)
						{
							creature.set_state(legged_creature_state::WALKING);
							continue;
						}

						// falling
						for(int leg_i = 0; leg_i < creature.legs.size(); leg_i++)
						{
							legged_creature_leg &l = creature.legs[leg_i];
							// move leg
							entity foot_ent = l.foot_entity;
							b2Body * foot_body = b2d_system -> get_dynamic_body(foot_ent);

							// move foot away from body, if not connected
							if(!(creature.connected_legs.count(leg_i)))
							{
								// apply impulse
								float ang = leg_i * 2 * M_PI / creature.leg_count + 1.1111109090990;
								b2Vec2 leg_vector = b2Vec2(cos(ang), sin(ang));
								leg_vector *= 0.1;
								foot_body -> ApplyLinearImpulseToCenter(leg_vector, true);

							}
						}

						break;
					}
				default:
					// check if creature is close to player
					if(b2Distance(legged_creature_pos, player_pos) > 4)
					{
						creature.set_state(legged_creature_state::NOT_WALKING);
						continue;
					}
					break;
			}
			// for(legged_creature_leg &l : creature.legs)


		}
	}
	
	void legged_creature_system::update_rendering(uint64_t time_to_step)
	{
		// for(auto creature: legged_creature_system::legged_creatures)
		// {
		// 	// update creature
		// 	// if creature has no target, find target
		// }
		game_engine::task t;
		while(task_scheduler_pointer->pop_task(t))
		{
			t.run();
		}


		std::vector<entity> entities = legged_creatures.get_entities();
		for (auto &ent : entities)
		{
			legged_creature &creature = legged_creatures.get(ent);
			// printf("creature type: %d\n", creature.type);
			b2Body *body = b2d_system->get_dynamic_body(ent);

			// b2_user_data *ud = (b2_user_data *)(body->GetFixtureList()->GetUserData().pointer);

			b2Vec2 position = body->GetPosition();

			position.x *= box2d_scale;
			position.y *= box2d_scale;
			position.x -= legged_creature_scale / 2.0f;
			position.y -= legged_creature_scale / 2.0f;

			game_engine::box_lerp b = box_sys->get(ent);
			b.x = position.x;
			b.y = position.y;
			b.x2 = position.x;
			b.y2 = position.y;
			
			box_sys->update_box(ent, b);
			texture_vbo_sys->update(ent, b.get_box());

			// update legs, now using rectangles and textures for bones
			for (int l = 0; l < creature.leg_count; l++)
			{
				legged_creature_leg &leg = creature.legs[l];

				for (int b = 0; b < leg.bones; b++)
				{
					entity leg_ent = leg.bone_entities[b];
					
					//get vertex coordinates from box2d fixtures

					b2Body * leg_body = b2d_system -> get_dynamic_body(leg_ent);
					//fixtures
					b2Fixture * fixtures = leg_body -> GetFixtureList();
					// get vertices from rectangle fixture
					b2PolygonShape * shape = (b2PolygonShape *) fixtures -> GetShape();
					std::vector<float> vertices;

					game_engine::box_lerp &leg_b = box_sys->get(leg_ent);
					for(int i = 0; i < shape -> m_count; i++)
					{
						b2Vec2 vertex = leg_body->GetWorldPoint(shape -> m_vertices[i]);
						vertex *= box2d_scale;
						vertices.push_back(vertex.x);
						vertices.push_back(vertex.y);
						vertices.push_back(leg_b.z);
					}
					texture_vbo_sys->update(leg_ent, {}, vertices, {});

				}
				entity foot_ent = leg.foot_entity;
				b2Body * foot_body = b2d_system -> get_dynamic_body(foot_ent);

				game_engine::box_lerp b = box_sys -> get(foot_ent);
				b2Vec2 foot_pos = foot_body -> GetPosition();
				foot_pos *= box2d_scale;
				foot_pos.x -= b.w / 2.0f;
				foot_pos.y -= b.h / 2.0f;
				b.x = foot_pos.x;
				b.y = foot_pos.y;
				b.x2 = foot_pos.x;
				b.y2 = foot_pos.y;
				box_sys -> update_box(foot_ent, b);
				texture_vbo_sys -> update(foot_ent, b.get_box());

				// b2Vec2 prev_anchor = leg.joints[0]->GetAnchorA();
				// prev_anchor *= box2d_scale;
				
				// for (int b = 1; b < leg.bones; b++)
				// {
				// 	entity leg_ent = leg.bone_entities[b];
				// 	if(!(leg.joints[b]))
				// 		continue;
					
				// 	b2Vec2 anchor_a = leg.joints[b]->GetAnchorA();
				// 	b2Vec2 anchor_b = leg.joints[b]->GetAnchorB();
				// 	anchor_a *= box2d_scale;
				// 	anchor_b *= box2d_scale;


				// 	game_engine::box_lerp &leg_b = box_sys->get(leg_ent);


				// 	// leg bone displaced vertice positions
				// 	std::vector<float> vertices = {
				// 		prev_anchor.x, prev_anchor.y, leg_b.z,
				// 		anchor_a.x, anchor_a.y, leg_b.z,
				// 		anchor_a.x, anchor_a.y + 2, leg_b.z,
				// 		prev_anchor.x, prev_anchor.y + 2, leg_b.z
				// 	};
				// 	prev_anchor = anchor_b;

				// 	texture_vbo_sys->update(leg_ent, {}, vertices, {});
				// }
				// // update last bone
				// entity leg_ent = leg.bone_entities[leg.bones - 1];
				// if(!(leg.joints[leg.bones - 1]))
				// 	continue;

				// b2Vec2 anchor_a = leg.joints[leg.bones - 1]->GetAnchorA();
				// anchor_a *= box2d_scale;

				// game_engine::box_lerp &leg_b = box_sys->get(leg_ent);

				// // leg bone displaced vertice positions
				// std::vector<float> vertices = {
				// 	prev_anchor.x, prev_anchor.y, leg_b.z,
				// 	anchor_a.x, anchor_a.y, leg_b.z,
				// 	anchor_a.x, anchor_a.y + 2, leg_b.z,
				// 	prev_anchor.x, prev_anchor.y + 2, leg_b.z
				// };
			}
		}
	}

	
	void legged_creature_system::add(entity ent, legged_creature &creature)
	{
		legged_creature_system::legged_creatures.add(ent, creature);
	}

	legged_creature& legged_creature_system::get(entity ent)
	{
		return legged_creature_system::legged_creatures.get(ent);
	}

	// const float legged_creature_scale = 8.0f;

	void legged_creature_system::create_legged_creature(entity ent, float x, float y, legged_creature_type type, entity head_ent, entity tail_ent)
	{
		// legged_creature_system::b2d_system = legged_creature_system::b2d_system ? legged_creature_system::b2d_system : (box2d_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<box2d_system>());
		
		b2d_mutex.lock();
		// create small circle projectile
		b2BodyDef body_def;
		body_def.type = b2_dynamicBody;
		body_def.position.Set(x / box2d_scale, y / box2d_scale);

		world_tile_sys -> delete_circle(x, y, 32, {});

		b2Body *body = legged_creature_system::b2d_system->world->CreateBody(&body_def);
		legged_creature_system::b2d_system->add_dynamic_body(ent, body);
		// legged_creature_system::b2d_system->add_mesh(ent, {{0, -1}, {1, 0}, {0, 1}, {0, -1}, {0, 1}, {-1, 0}}, b2fixture_types::LEGGED_CREATURE);
		b2CircleShape circle;
		circle.m_radius = (legged_creature_scale / 2.0) / box2d_scale;
		b2FixtureDef fixture_def;
		fixture_def.shape = &circle;
		fixture_def.density = 0.2f;
		fixture_def.friction = 0.3f;
		fixture_def.restitution = 0.89f;
		fixture_def.filter.categoryBits = b2fixture_types::LEGGED_CREATURE;
		fixture_def.filter.maskBits = b2fixture_types::TERRAIN | b2fixture_types::PLAYER | b2fixture_types::LEGGED_CREATURE;


		b2_user_data *ud = new b2_user_data(ent, b2fixture_types::LEGGED_CREATURE);
		
		b2FixtureUserData fixtureUserData;
		fixtureUserData.pointer = (uintptr_t) ud;
		fixture_def.userData = fixtureUserData;

		body->CreateFixture(&fixture_def);
		body->SetLinearVelocity(b2Vec2(0.f, -1.f));
		
		GLuint creature_texture = glsl_helper::texture_map["spider"];

		game_engine::sprite sprt(game_engine::shader_programs[0]);
		sprt.add_texture({creature_texture, 0, GL_R8, 4, 4});

		render_sys->add(ent, sprt);

		box_sys->add(ent, {0, 0, -4.6f, legged_creature_scale, legged_creature_scale});
		texture_vbo_sys->add(ent);

		legged_creature creature = {type};
		creature.set_params(8, 3, x, y);
		if(head_ent)
		{
			creature.head_entity = head_ent;
		}
		if(tail_ent)
		{
			creature.tail_entity = tail_ent;
		}

		// for (legged_creature_leg &leg : creature.legs)
		for (int l = 0; l < creature.leg_count; l++)
		{
			legged_creature_leg &leg = creature.legs[l];

			for (int b = 0; b < leg.bones; b++)
			{
				entity leg_ent = game_engine::game_engine_pointer -> create_entity();
				leg.bone_entities.push_back(leg_ent);
				// game_engine::sprite leg_sprt(game_engine::shader_programs[1]);
				// leg_sprt.type = game_engine::sprite_type::STATIC_LINE;
				game_engine::sprite leg_sprt(game_engine::shader_programs[0]);
				leg_sprt.add_texture({creature_texture, 0, GL_R8, 4, 4});

				render_sys->add(leg_ent, leg_sprt);


				float leg_x = leg.end_points[b].first;
				float leg_y = leg.end_points[b].second;
				float leg_w = 2;
				float leg_h = leg.end_points[b + 1].second - leg_y;

				box_sys->add(leg_ent, {leg_x, leg_y, -5.6f, leg_w, leg_h});

				texture_vbo_sys->add(leg_ent);

				b2BodyDef bodyDef;
				bodyDef.type = b2_dynamicBody;
				bodyDef.position.Set(leg_x / box2d_scale, leg_y / box2d_scale);
				b2Body *body = b2d_system->world->CreateBody(&bodyDef);
				b2d_system -> add_dynamic_body(leg_ent, body);

				b2PolygonShape shape;
				shape.SetAsBox(abs(leg_w / box2d_scale) / 2.0, abs(leg_h / box2d_scale) / 2.0);
				// shape.SetAsBox(abs(w / box2d_scale) / 2.0, abs(h / box2d_scale) / 2.0);
				b2FixtureDef fixtureDef;
				fixtureDef.shape = &shape;
				fixtureDef.density = 0.0515f;
				fixtureDef.friction = 0.3f;
				fixtureDef.filter.categoryBits = b2fixture_types::LEGS;
				fixtureDef.filter.maskBits = b2fixture_types::TERRAIN | b2fixture_types::PLAYER;

				body->CreateFixture(&fixtureDef);
			}
			// add a small circle to the end of the leg
			entity foot_ent = game_engine::game_engine_pointer -> create_entity();
			leg.foot_entity = foot_ent;
			game_engine::sprite leg_sprt(game_engine::shader_programs[1]);
			leg_sprt.add_texture({creature_texture, 0, GL_R8, 2, 2});

			render_sys->add(foot_ent, leg_sprt);
			box_sys->add(foot_ent, {(float)(leg.end_points[leg.bones].first) - 1.f, (float)(leg.end_points[leg.bones].second) - 1, -5.6f, 2, 2});

			texture_vbo_sys->add(foot_ent);

			b2BodyDef bodyDef;
			bodyDef.type = b2_dynamicBody;
			bodyDef.position.Set(leg.end_points[leg.bones].first / box2d_scale, leg.end_points[leg.bones].second / box2d_scale);
			
			b2Body *body = b2d_system->world->CreateBody(&bodyDef);
			b2d_system -> add_dynamic_body(foot_ent, body);

			b2CircleShape circle;
			circle.m_radius = 1.0 / box2d_scale;
			b2FixtureDef fixtureDef;
			fixtureDef.shape = &circle;
			fixtureDef.density = 0.032f;
			fixtureDef.friction = 0.63f;
			fixtureDef.restitution = 0.19f;
			fixtureDef.filter.categoryBits = b2fixture_types::FEET;

			b2_user_data *ud = new b2_user_data(foot_ent, b2fixture_types::FEET);
			ud -> ent = foot_ent;
			ud -> ent_2 = ent;
			ud -> index = l;

			b2FixtureUserData fixtureUserData;
			fixtureUserData.pointer = (uintptr_t) ud;
			fixtureDef.userData = fixtureUserData;

			body->CreateFixture(&fixtureDef);

			for (int b = 0; b < leg.bones + 1; b++)
			{
				entity leg_ent = b == leg.bones ? leg.foot_entity : leg.bone_entities[b];
				if(leg_ent == ent)
				{
					leg.joints.push_back(0);
					continue;
				}
				entity prev_leg_ent = b > 0 ? leg.bone_entities[b - 1] : ent;
				b2Body *prev_body = b2d_system->get_dynamic_body(prev_leg_ent);
				b2Body *body = b2d_system->get_dynamic_body(leg_ent);

				b2RevoluteJointDef jointDef;
				jointDef.bodyA = prev_body;
				jointDef.bodyB = body;
				if(b == 0)
				{
					jointDef.localAnchorA = b2Vec2(0, 0);
					// jointDef.localAnchorB = b2Vec2(0, 0);
				}
				else
				{
					// b2PolygonShape * shape = (b2PolygonShape *) prev_body->GetFixtureList()->GetShape();
					jointDef.localAnchorA = b2Vec2(0, ((b2PolygonShape *)(prev_body->GetFixtureList()->GetShape()))->m_vertices[0].y);
				}
				if(b == leg.bones)
				{
					jointDef.localAnchorB = b2Vec2(0, 0);
				}
				else
				{
					jointDef.localAnchorB = b2Vec2(0, ((b2PolygonShape *)(body->GetFixtureList()->GetShape()))->m_vertices[2].y);
				}

				jointDef.collideConnected = false;
				b2RevoluteJoint *joint = (b2RevoluteJoint *)b2d_system->world->CreateJoint(&jointDef);
				leg.joints.push_back(joint);
			}

		}
		
		// if(head_ent)
		// {
		// 	creature.head_entity = head_ent;
		// }
		// if(tail_ent)
		// {
		// 	creature.tail_entity = tail_ent;
		// }
		if(head_ent && legged_creatures.contains(head_ent))
		{
			// create joint between head and creature
			b2Body *head_body = b2d_system -> get_dynamic_body(head_ent);
			b2Body *body = b2d_system -> get_dynamic_body(ent);

			b2RevoluteJointDef jointDef;
			jointDef.bodyA = head_body;
			jointDef.bodyB = body;
			
			// create anchor at the edge/radius of the head and body
			// get radius of head
			b2CircleShape * shape = (b2CircleShape *) head_body -> GetFixtureList() -> GetShape();
			float radius = shape -> m_radius;
			
			jointDef.localAnchorA = b2Vec2(0, -radius);

			radius = circle.m_radius;
			jointDef.localAnchorB = b2Vec2(0, radius);

			jointDef.collideConnected = true;
			b2d_system -> world -> CreateJoint(&jointDef);
		}
		if(tail_ent && legged_creatures.contains(tail_ent))
		{
			// create joint between tail and creature
			b2Body *tail_body = b2d_system -> get_dynamic_body(tail_ent);
			b2Body *body = b2d_system -> get_dynamic_body(ent);

			b2RevoluteJointDef jointDef;
			jointDef.bodyA = tail_body;
			jointDef.bodyB = body;
			
			// create anchor at the edge/radius of the head and body
			// get radius of head
			b2CircleShape * shape = (b2CircleShape *) tail_body -> GetFixtureList() -> GetShape();
			float radius = shape -> m_radius;
			
			jointDef.localAnchorA = b2Vec2(0, -radius);

			radius = circle.m_radius;
			jointDef.localAnchorB = b2Vec2(0, radius);

			jointDef.collideConnected = true;
			b2d_system -> world -> CreateJoint(&jointDef);
		}

		legged_creatures.add(ent, creature);



		b2d_mutex.unlock();

		// legged_creature_system::b2d_system -> create_dynamic_body(ent, {{0, -1}, {1, 0}, {0, 1}, {0, -1}, {0, 1}, {-1, 0}});
		// game_engine::box2d_body = 
	}

	void legged_creature_system::add_task(game_engine::task t)
	{
		printf("%d\n", (uint64_t)task_scheduler_pointer);
		task_scheduler_pointer->add_task(t);
	}

}