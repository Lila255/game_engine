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

#define M_PI 3.14159265358979323846		/* pi */
#define radians(x) ((x) * M_PI / 180.0) // degrees to radians
// #define raise(x) (1 << x)

#define PIXEL_SCALE 6

// typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
// typedef K::Point_2 point_2;
// typedef CGAL::Delaunay_triangulation_2<K>  Triangulation;
// typedef Triangulation::Edge_iterator  Edge_iterator;


namespace game
{

	// const uint16_t CHUNK_SIZE = 128; // There are CHUNK_SIZE*CHUNK_SIZE tiles in chunk

	extern std::mutex b2d_mutex;

	// pre-declare world_tile_system, with a delete_circle function
	// struct b2_contact_listener : public b2ContactListener;
	// void world_tile_system::delete_circle(int x, int y, int radius){}


	enum b2fixture_types
	{
		EMPTY,	// set to this to delete fixture
		PLAYER,
		TERRAIN,
		PROJECTILE,
		DEBRIS
	};

	struct b2_user_data
	{
		entity ent;
		b2fixture_types type;
		b2_user_data() = default;
		b2_user_data(entity e, b2fixture_types t) : ent(e), type(t) {}
	};

	// b2ContactListener * contact_listener;

	// box2d system
	struct box2d_system : public game_engine::system
	{
	private:
		b2Vec2 gravity; //(0.0f, -9.8f);
		// std::vector<b2Body *> bodies;
		game_engine::sparse_component_set<b2Body *> static_bodies;
		game_engine::sparse_component_set<b2Body *> dynamic_bodies;

	public:
		b2World *world;
		// box2d_system() = default;
		box2d_system()
		{
			gravity = b2Vec2(0.0f, 91.8f);
			world = new b2World(gravity);
			// contact_listener = new b2_contact_listener();
		}

		~box2d_system()
		{
			// delete user data from fixtures
			for (auto &ent : static_bodies.get_entities())
			{
				b2Body *body = static_bodies.get(ent);
				b2Fixture *fixtures = body->GetFixtureList();
				while (fixtures)
				{
					b2Fixture *next = fixtures->GetNext();
					if(fixtures->GetUserData().pointer != 0)
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
					if(fixtures->GetUserData().pointer != 0)
						delete (b2_user_data *)(fixtures->GetUserData().pointer);
					fixtures = next;
				}
			}

			delete world;
		}

		/// @brief Create and add a static body to the b2d world
		/// @param ent The entity to associate the body with
		/// @param mesh The triangulated mesh to add to the body
		void create_static_body(entity ent, std::vector<std::pair<float, float>> mesh)
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
			b2FixtureUserData fixtureUserData;
			fixtureUserData.pointer = (uintptr_t)new b2_user_data(ent, b2fixture_types::TERRAIN);
			fixtureDef.userData = fixtureUserData;
			body->CreateFixture(&fixtureDef);
			static_bodies.add(ent, body);
		}

		/// @brief Create a static body with multiple meshes
		/// @param ent The entity to associate the body with
		/// @param meshes The meshes to add to the body
		void create_static_bodies(entity ent, std::vector<std::vector<std::pair<float, float>>> *meshes)
		{
			// b2BodyDef bodyDef;
			// bodyDef.type = b2_staticBody;
			// b2Body *body = world->CreateBody(&bodyDef);

			// // add mesh
			// for (int i = 0; i < meshes.size(); i++)
			// {
			//     if (meshes[i].size() < 3)
			//         continue;

			//     b2PolygonShape chain;

			//     b2Vec2 *vertices = new b2Vec2[meshes[i].size()];
			//     for (int j = 0; j < meshes[i].size(); j++)
			//     {
			//         vertices[j].Set(meshes[i][j].first, meshes[i][j].second);
			//     }
			//     printf("Verticy count: %d\n", meshes[i].size());
			//     chain.Set(vertices, meshes[i].size());
			//     b2FixtureDef fixtureDef;
			//     fixtureDef.shape = &chain;
			//     fixtureDef.density = 0.0f;
			//     fixtureDef.friction = 0.73f;
			//     body->CreateFixture(&fixtureDef);
			//     delete[] vertices;
			// }
			// static_bodies.add(ent, body);
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
					fixtureDef.isSensor = true;
					b2FixtureUserData fixtureUserData;
					// fixtureUserData.pointer = b2fixture_types::TERRAIN;
					fixtureUserData.pointer = (uintptr_t)new b2_user_data(ent, b2fixture_types::TERRAIN);
					fixtureDef.userData = fixtureUserData;
					body->CreateFixture(&fixtureDef);
					delete[] vertices;
				}
			}
			static_bodies.add(ent, body);
		}

		void update_static_outlines(entity ent, std::vector<std::vector<std::pair<float, float>>> *meshes)
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
					b2FixtureUserData fixtureUserData;
						// fixtureUserData.pointer = b2fixture_types::TERRAIN;
					fixtureUserData.pointer = (uintptr_t)new b2_user_data(ent, b2fixture_types::TERRAIN);
					fixtureDef.userData = fixtureUserData;
					body->CreateFixture(&fixtureDef);
					delete[] vertices;
				}
			}
			game::b2d_mutex.unlock();
		}

		void remove_static_body(entity ent)
		{
			world->DestroyBody(static_bodies.get(ent));
			static_bodies.remove(ent);
		}

		void create_dynamic_body(entity ent, std::vector<std::pair<float, float>> mesh)
		{
			// static_bodies.add(ent, body);
			// b2BodyDef bodyDef;
			// bodyDef.type = b2_dynamicBody;
			// // set position
			// bodyDef.position.Set(110.0f, 100.0f);
			// b2Body *body = world -> CreateBody(&bodyDef);
			// // add mesh
			// b2ChainShape chain;
			// b2Vec2 *vertices = new b2Vec2[mesh.size()];
			// for (int i = 0; i < mesh.size(); i++)
			// {
			//     vertices[i].Set(mesh[i].first, mesh[i].second);
			// }
			// chain.CreateLoop(vertices, mesh.size());
			// b2FixtureDef fixtureDef;
			// fixtureDef.shape = &chain;
			// fixtureDef.density = 10.0f;
			// fixtureDef.friction = 0.3f;
			// body->CreateFixture(&fixtureDef);
			// dynamic_bodies.add(ent, body);

			// create rectangle shape and add
			// b2BodyDef body_def;
			// body_def.type = b2_dynamicBody;
			// body_def.position.Set(110.0f, 100.0f);
			// b2Body *body = world->CreateBody(&body_def);
			// b2PolygonShape dynamic_box;
			// // dynamic_box.SetAsBox(2,2);
			// dynamic_box.SetAsBox(glsl_helper::character_width / 2.0f, glsl_helper::character_height / 2.0f);
			// b2FixtureDef fixture_def;
			// fixture_def.shape = &dynamic_box;
			// fixture_def.density = 5.4f;
			// fixture_def.friction = 0.3f;
			// body->CreateFixture(&fixture_def);
			// dynamic_bodies.add(ent, body);

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
				b2FixtureUserData fixtureUserData;
				// fixtureUserData.pointer = b2fixture_types::PLAYER;
				fixtureUserData.pointer = (uintptr_t)new b2_user_data(ent, b2fixture_types::PLAYER);
				fixture_def.userData = fixtureUserData;
				body->CreateFixture(&fixture_def);
				delete[] vertices;
			}
			dynamic_bodies.add(ent, body);

			// do this ^^^ but get the shape from the mesh paramater
			// b2BodyDef body_def;
			// body_def.type = b2_dynamicBody;
			// body_def.position.Set(60.0f, -20.0f);
			// b2Body *body = world->CreateBody(&body_def);
			// b2FixtureDef fixture_def;

			// for (int i = 0; i < mesh.size(); i += 3)
			// {
			// 	b2PolygonShape triangle_shape;
			// 	// triangle_shape.Set((const b2Vec2 *)&mesh[i], 3);
			// 	b2Vec2 *vertices = new b2Vec2[3];
			// 	for (int j = 0; j < 3; j++)
			// 	{
			// 		vertices[j].Set(mesh[i + j].first, mesh[i + j].second);
			// 	}
			// 	// if straight line, skip (poor check, only works for axis aligned lines)
			// 	if (vertices[0].x == vertices[1].x && vertices[0].x == vertices[2].x || vertices[0].y == vertices[1].y && vertices[0].y == vertices[2].y)
			// 		continue;
			// 	triangle_shape.Set(vertices, 3);
			// 	fixture_def.shape = &triangle_shape;
			// 	body->CreateFixture(&fixture_def);
			// }
			// fixture_def.density = 1.f;
			// fixture_def.friction = 0.5f;
			// fixture_def.restitution = 0.10f;

			// dynamic_bodies.add(ent, body);

			// Do this ^^ but do not use triangles
			// b2BodyDef body_def;
			// body_def.type = b2_dynamicBody;
			// body_def.position.Set(60.0f, -20.0f);
			// b2Body *body = world->CreateBody(&body_def);
			// b2PolygonShape dynamic_box;
			// b2Vec2 * verts = new b2Vec2[mesh.size()];
			// for (int i = 0; i < mesh.size(); i++)
			// {
			// 	verts[i].Set(mesh[i].first, mesh[i].second);
			// }
			// dynamic_box.Set(verts, mesh.size());
			// b2FixtureDef fixture_def;
			// fixture_def.shape = &dynamic_box;
			// fixture_def.density = 1.f;
			// fixture_def.friction = 0.25f;
			// fixture_def.restitution = 0.10f;
			// body->CreateFixture(&fixture_def);
			// delete[] verts;
			// dynamic_bodies.add(ent, body);
		}

		void remove_dynamic_body(entity ent)
		{
			b2d_mutex.lock();
			world->DestroyBody(dynamic_bodies.get(ent));
			b2d_mutex.unlock();
			dynamic_bodies.remove(ent);
		}
		void update() {}
		void update(uint64_t time_to_step)
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

		b2Body *get_static_body(entity ent)
		{
			return static_bodies.get(ent);
		}
		b2Body *get_dynamic_body(entity ent)
		{
			return dynamic_bodies.get(ent);
		}
	};

	

	// struct world_tile_system : public game_engine::system
	// {
	// private:
	// 	std::array<chunk *, NUM_CHUNKS> chunk_data{};
	// 	std::array<entity, NUM_CHUNKS> chunk_entities;


	// public:
	// 	std::array<std::array<uint8_t, game::CHUNKS_WIDTH>, game::CHUNKS_WIDTH> modified_chunks;
	
	// 	entity all_chunk_ent;

	// 	world_tile_system();

	// 	~world_tile_system();

	// 	void set_all_chunk_ent(entity ent);

	// 	uint8_t get_tile_at(int x, int y);

	// 	void set_tile_at(int x, int y, uint8_t tile_type);

	// 	std::array<entity, NUM_CHUNKS> get_chunk_entities();

	// 	// void update() {}

	// 	void update(uint64_t tick_count, std::array<std::array<uint8_t, CHUNKS_WIDTH>, CHUNKS_WIDTH> * modified_chunks);

	// 	void generate_world();

	// 	entity get_chunk_entity(int x, int y);

	// 	entity get_chunk_entity(int chunk);

	// 	std::array<chunk *, NUM_CHUNKS> *get_chunks();

	// 	std::array<std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE> *, NUM_CHUNKS> get_chunks_data();

	// 	std::vector<std::vector<std::pair<float, float>>> *create_outlines(int x, int y);

	// 	void delete_circle(int x, int y, int radius);
	// };

	struct b2_contact_listener : public b2ContactListener
	{
		void BeginContact(b2Contact *contact) override
		{
			b2Fixture *fixture_a = contact->GetFixtureA();
			b2Fixture *fixture_b = contact->GetFixtureB();
			b2_user_data *ud_a = (b2_user_data *)fixture_a->GetUserData().pointer;
			b2_user_data *ud_b = (b2_user_data *)fixture_b->GetUserData().pointer;

			if(ud_a == nullptr || ud_b == nullptr)
				return;
			
			if (ud_a->type == b2fixture_types::PROJECTILE || ud_b->type == b2fixture_types::PROJECTILE)
			{
				// if (fixture_a->IsSensor() || fixture_b->IsSensor())
				// 	return;
				
				if(ud_a->type == b2fixture_types::PLAYER || ud_b->type == b2fixture_types::PLAYER)
				{
					// printf("Player hit\n");
					return;
				}
				if(ud_a->type == b2fixture_types::TERRAIN || ud_b->type == b2fixture_types::TERRAIN)
				{
					uint16_t explosion_radius = 8;
					// printf("Terrain hit\n");
					if(ud_a->type == b2fixture_types::PROJECTILE)
					{
						// delete circle shape around projectile
						((world_tile_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<world_tile_system>()))->delete_circle((int)(fixture_a->GetBody()->GetPosition().x + glsl_helper::projectile_width / 2), (int)(fixture_a->GetBody()->GetPosition().y + glsl_helper::projectile_height / 2), explosion_radius);

						// set projectile type to empty in ud_a
						ud_a->type = b2fixture_types::EMPTY;
						
					}
					else
					{
						// delete circle shape around projectile
						((world_tile_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<world_tile_system>()))->delete_circle((int)(fixture_b->GetBody()->GetPosition().x + glsl_helper::projectile_width / 2), (int)(fixture_b->GetBody()->GetPosition().y + glsl_helper::projectile_height / 2), explosion_radius);
						// void delete_circle(int x, int y, int radius, std::vector<std::vector<std::vector<std::pair<float, float>>>> *chunk_outlines)

						// set projectile type to empty in ud_b
						ud_b->type = b2fixture_types::EMPTY;

					}
					return;
				}
			}
		}
	};

	// misc

	std::vector<game_engine::vec2> load_normal_vectors();

} // namespace game
