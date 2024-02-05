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
#include <box2d/box2d.h>

#include "engine_comp.hpp"
#include "chunk.hpp"

#define M_PI 3.14159265358979323846		/* pi */
#define radians(x) ((x) * M_PI / 180.0) // degrees to radians
// #define raise(x) (1 << x)

#define PIXEL_SCALE 7

// typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
// typedef K::Point_2 point_2;
// typedef CGAL::Delaunay_triangulation_2<K>  Triangulation;
// typedef Triangulation::Edge_iterator  Edge_iterator;

template <typename t, typename... args>
bool in_set(t val, args... set)
{
	return ((val == set) || ...);
}

namespace game
{
	const uint16_t NUM_CHUNKS = 9; // 3x3 chunks in world
	const uint16_t CHUNKS_WIDTH = 3;
	// const uint16_t CHUNK_SIZE = 128; // There are CHUNK_SIZE*CHUNK_SIZE tiles in chunk

	std::mutex b2d_mutex;

	// pre-declare world_tile_system, with a delete_circle function
	// struct b2_contact_listener : public b2ContactListener;
	// void world_tile_system::delete_circle(int x, int y, int radius){}


	enum b2fixture_types
	{
		EMPTY = 0x00,	// set to this to delete fixture
		PLAYER = 0x01,
		TERRAIN = 0x02,
		PROJECTILE = 0x04,
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

	// projectile
	struct projectile : public game_engine::component
	{
		b2Body *body;
		projectile() = default;
		projectile(b2Body *b) : body(b) {}
	};

	// projectile system
	struct projectile_system : public game_engine::system
	{
	private:
		game_engine::sparse_component_set<projectile> projectiles;

	public:
		projectile_system() = default;

		void update() {}

		void update(uint64_t time_to_step) // updating projectiles requires stepping time in box2d first
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

		b2Body *create_projectile(entity ent, float x, float y, float ang, float vel)
		{
			// create small circle projectile
			b2BodyDef body_def;
			body_def.type = b2_dynamicBody;
			body_def.position.Set(x, y);
			// body_def.angle = ang;
			b2Body *body = ((box2d_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<box2d_system>()))->world->CreateBody(&body_def);
			b2CircleShape circle;
			circle.m_radius = glsl_helper::projectile_width / 2.0f;
			b2FixtureDef fixture_def;
			fixture_def.shape = &circle;
			fixture_def.density = 0.2f;
			fixture_def.friction = 0.3f;
			fixture_def.restitution = 0.89f;
			
			b2FixtureUserData fixtureUserData;
			fixtureUserData.pointer = b2fixture_types::PROJECTILE;
			fixtureUserData.pointer = (uintptr_t)new b2_user_data(ent, b2fixture_types::PROJECTILE);
			fixture_def.userData = fixtureUserData;


			b2d_mutex.lock();
			body->CreateFixture(&fixture_def);
			body->SetLinearVelocity(b2Vec2(vel * cos(ang), -vel * sin(ang)));
			b2d_mutex.unlock();
			projectile proj(body);
			projectiles.add(ent, proj);
			return body;
		}

		void add_projectile(entity ent, projectile proj)
		{
			projectiles.add(ent, proj);
		}

		/// @brief Remove the projectile from the system, and destroy the box2d body
		/// @param ent 
		void remove_projectile(entity ent)
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

		projectile &get_projectile(entity ent)
		{
			return projectiles.get(ent);
		}
	};



	struct world_tile_system : public game_engine::system
	{
	private:
		std::array<chunk *, NUM_CHUNKS> chunk_data{};
		std::array<entity, NUM_CHUNKS> chunk_entities;

	public:
		entity all_chunk_ent;

		world_tile_system()
		{
			// chunk_data = new uint8_t[NUM_CHUNKS];

			for (int i = 0; i < NUM_CHUNKS; i++)
			{
				int chunk_x = i % CHUNKS_WIDTH;
				int chunk_y = i / CHUNKS_WIDTH;
				chunk_data[i] = new chunk(chunk_x, chunk_y);
				// chunk_data[i] = new std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE>{};
			}
		}

		~world_tile_system()
		{
			for (int i = 0; i < NUM_CHUNKS; i++)
			{
				delete chunk_data[i];
			}
		}

		void set_all_chunk_ent(entity ent)
		{
			all_chunk_ent = ent;
		}

		uint8_t get_tile_at(int x, int y)
		{
			int chunk_x = x / CHUNK_SIZE;
			int chunk_y = y / CHUNK_SIZE;
			int chunk = chunk_x + chunk_y * CHUNKS_WIDTH;
			int tile_x = x % CHUNK_SIZE;
			int tile_y = y % CHUNK_SIZE;
			if (chunk_x < 0 || chunk_x >= CHUNKS_WIDTH || chunk_y < 0 || chunk_y >= CHUNKS_WIDTH)
				return (tile_type)0;
			if (tile_x < 0 || tile_x >= CHUNK_SIZE || tile_y < 0 || tile_y >= CHUNK_SIZE)
				return (tile_type)0;
			return (chunk_data[chunk])->get_tile(tile_x, tile_y);
		}

		void set_tile_at(int x, int y, uint8_t tile_type)
		{
			int chunk_x = x / CHUNK_SIZE;
			int chunk_y = y / CHUNK_SIZE;
			int chunk = chunk_x + chunk_y * CHUNKS_WIDTH;
			int tile_x = x % CHUNK_SIZE;
			int tile_y = y % CHUNK_SIZE;
			if (chunk_x < 0 || chunk_x >= CHUNKS_WIDTH || chunk_y < 0 || chunk_y >= CHUNKS_WIDTH)
				return;

			(chunk_data[chunk])->set_tile(tile_x, tile_y, tile_type);
		}

		std::array<entity, NUM_CHUNKS> get_chunk_entities()
		{
			return chunk_entities;
		}

		void update() {}

		void update(uint64_t tick_count)
		{
			uint8_t direction = tick_count % 2;
			for (int y = CHUNKS_WIDTH * CHUNK_SIZE - 1; y >= 0; y--)
			{
				for (int x = direction ? 0 : CHUNKS_WIDTH * CHUNK_SIZE - 1; x < CHUNKS_WIDTH * CHUNK_SIZE && x >= 0; x += direction ? 1 : -1)
				{
					int chunk_x = x / CHUNK_SIZE;
					int chunk_y = y / CHUNK_SIZE;
					int chunk = chunk_x + chunk_y * CHUNKS_WIDTH;
					int tile_x = x % CHUNK_SIZE;
					int tile_y = y % CHUNK_SIZE;

					uint8_t tile_type = get_tile_at(x, y);
					switch (tile_type)
					{
					// case SMOKE:
					// 	if (get_tile_at(x, y-1) == AIR)
					// 	{
					// 		set_tile_at(x, y-1, SMOKE);
					// 		set_tile_at(x, y, AIR);
					// 		break;IN_SET
					// 	}
					// 	if (get_tile_at(x-1, y-1) == AIR)
					// 	{
					// 		set_tile_at(x-1, y-1, SMOKE);
					// 		set_tile_at(x, y, AIR);
					// 		break;
					// 	}
					// 	if (get_tile_at(x+1, y-1) == AIR)
					// 	{
					// 		set_tile_at(x+1, y-1, SMOKE);
					// 		set_tile_at(x, y, AIR);
					// 		break;
					// 	}
					// 	break;
					case WATER:
						if (in_set(get_tile_at(x, y + 1), AIR, SMOKE))
						{
							set_tile_at(x, y, get_tile_at(x, y + 1));
							set_tile_at(x, y + 1, WATER);
							break;
						}
						if (in_set(get_tile_at(x - 1, y + 1), AIR, SMOKE))
						{
							set_tile_at(x, y, get_tile_at(x - 1, y + 1));
							set_tile_at(x - 1, y + 1, WATER);
							break;
						}
						if (in_set(get_tile_at(x + 1, y + 1), AIR, SMOKE))
						{
							set_tile_at(x, y, get_tile_at(x + 1, y + 1));
							set_tile_at(x + 1, y + 1, WATER);
							break;
						}
						if (in_set(get_tile_at(x - 1, y), AIR, SMOKE))
						{
							set_tile_at(x - 1, y, WATER);
							set_tile_at(x, y, AIR);
							break;
						}
						if (in_set(get_tile_at(x + 1, y), AIR, SMOKE))
						{
							set_tile_at(x + 1, y, WATER);
							set_tile_at(x, y, AIR);
							break;
						}
					case GRASS:
						if (rand() % 20 == 0)
						{
							int dx[] = {0, 1, 1, 1, 0, -1, -1, -1};
							int dy[] = {-1, -1, 0, 1, 1, 1, 0, -1};
							int r = rand() % 8;
							for (int i = 0; i < 8; i++)
							{
								int xx = x + dx[(r + i) % 8];
								int yy = y + dy[(r + i) % 8];
								if (get_tile_at(xx, yy) == DIRT && get_tile_at(xx, yy - 1) == AIR)
								{
									set_tile_at(xx, yy, GRASS);
									break;
								}
							}
						}
						if (get_tile_at(x, y - 1) == AIR && rand() % 100 == 0)
						{
							set_tile_at(x, y, DIRT);
						}
						break;

					case SAND:
						if (in_set(get_tile_at(x, y + 1), AIR, SMOKE, WATER))
						{
							set_tile_at(x, y, get_tile_at(x, y + 1));
							set_tile_at(x, y + 1, SAND);
						}
						else if (in_set(get_tile_at(x - 1, y + 1), AIR, SMOKE, WATER))
						{
							set_tile_at(x, y, get_tile_at(x - 1, y + 1));
							set_tile_at(x - 1, y + 1, SAND);
						}
						else if (in_set(get_tile_at(x + 1, y + 1), AIR, SMOKE, WATER))
						{
							set_tile_at(x, y, get_tile_at(x + 1, y + 1));
							set_tile_at(x + 1, y + 1, SAND);
						}
						break;
					}
				}
			}

			for (int y = 0; y < CHUNKS_WIDTH * CHUNK_SIZE; y++)
			{
				for (int x = direction ? 0 : CHUNKS_WIDTH * CHUNK_SIZE - 1; x < CHUNKS_WIDTH * CHUNK_SIZE && x >= 0; x += direction ? 1 : -1)
				{
					int chunk_x = x / CHUNK_SIZE;
					int chunk_y = y / CHUNK_SIZE;
					int chunk = chunk_x + chunk_y * CHUNKS_WIDTH;
					int tile_x = x % CHUNK_SIZE;
					int tile_y = y % CHUNK_SIZE;

					uint8_t tile_type = get_tile_at(x, y);
					switch (tile_type)
					{
					case SMOKE:
						if (in_set(get_tile_at(x, y - 1), AIR, WATER))
						{
							set_tile_at(x, y, get_tile_at(x, y - 1));
							set_tile_at(x, y - 1, SMOKE);
						}
						else if (in_set(get_tile_at(x - 1, y - 1), AIR, WATER))
						{
							set_tile_at(x, y, get_tile_at(x - 1, y - 1));
							set_tile_at(x - 1, y - 1, SMOKE);
						}
						else if (in_set(get_tile_at(x + 1, y - 1), AIR, WATER))
						{
							set_tile_at(x, y, get_tile_at(x + 1, y - 1));
							set_tile_at(x + 1, y - 1, SMOKE);
						}
						else if (in_set(get_tile_at(x - 1, y), AIR, WATER))
						{
							set_tile_at(x, y, get_tile_at(x - 1, y));
							set_tile_at(x - 1, y, SMOKE);
						}
						else if (in_set(get_tile_at(x + 1, y), AIR, WATER))
						{
							set_tile_at(x, y, get_tile_at(x + 1, y));
							set_tile_at(x + 1, y, SMOKE);
						}
						break;
					}
				}
			}

			// update textures
			for (int chunk = 0; chunk < NUM_CHUNKS; chunk++)
			{
				entity chunk_ent = chunk_entities[chunk];
				int chunk_x = chunk % CHUNKS_WIDTH;
				int chunk_y = chunk / CHUNKS_WIDTH;
				game_engine::render_system *render_system_pointer = ((game_engine::render_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::render_system>()));
				render_system_pointer->update_texture_section(all_chunk_ent, (uint8_t *)(chunk_data[chunk]->get_data()), chunk_x * CHUNK_SIZE, chunk_y * CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
			}
		}

		void generate_world()
		{
			for (int chunk = 0; chunk < NUM_CHUNKS; chunk++)
			{
				chunk_data[chunk]->create_chunk();
				chunk_entities[chunk] = game_engine::game_engine_pointer->create_entity();
			}
			// create solid border around world
			for (int x = 0; x < CHUNKS_WIDTH * CHUNK_SIZE; x++)
			{
				set_tile_at(x, 0, BEDROCK);
				set_tile_at(x, CHUNKS_WIDTH * CHUNK_SIZE - 1, BEDROCK);
			}
			for (int y = 0; y < CHUNKS_WIDTH * CHUNK_SIZE; y++)
			{
				set_tile_at(0, y, BEDROCK);
				set_tile_at(CHUNKS_WIDTH * CHUNK_SIZE - 1, y, BEDROCK);
			}
		}
		entity get_chunk_entity(int x, int y)
		{
			return chunk_entities[x + y * CHUNKS_WIDTH];
		}
		entity get_chunk_entity(int chunk)
		{
			return chunk_entities[chunk];
		}
		std::array<chunk *, NUM_CHUNKS> *get_chunks()
		{
			return &chunk_data;
		}
		std::array<std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE> *, NUM_CHUNKS> get_chunks_data()
		{
			std::array<std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE> *, NUM_CHUNKS> chunks_data;
			for (int chunk = 0; chunk < NUM_CHUNKS; chunk++)
			{
				// chunk_data[chunk]->create_texture_from_chunk(textures[chunk]);
				chunks_data[chunk] = chunk_data[chunk]->get_data();
			}
			return chunks_data;
		}

		std::vector<std::vector<std::pair<float, float>>> *create_outlines(int x, int y)
		{
			std::vector<std::vector<std::pair<float, float>>> *outlines = new std::vector<std::vector<std::pair<float, float>>>;
			chunk_data[x + y * CHUNKS_WIDTH]->create_outlines(outlines);
			return outlines;
		}

		void delete_circle(int x, int y, int radius)
		{

			// get texture system
			// auto texture_system = (game_engine::render_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::render_system>());
			// get b2d system
			// auto b2d_system = (box2d_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<box2d_system>());

			for (int i = 0; i < NUM_CHUNKS; i++)
			{
				bool modified = chunk_data[i]->delete_circle(x, y, radius);
				// if (modified)
				// {
					// std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE> *tile_data = chunk_data[i]->get_data();
					// entity ent = chunk_entities[i];
					// texture_system->update_texture(ent, (uint8_t *)tile_data->data(), CHUNK_SIZE, CHUNK_SIZE, game_engine::shader_programs[0]);
					// int chunkx = i % CHUNKS_WIDTH;
					// int chunky = i / CHUNKS_WIDTH;
					// texture_system->update_texture_section(all_chunk_ent, (uint8_t *)tile_data->data(), chunkx * CHUNK_SIZE, chunky * CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
					// std::vector<std::vector<std::pair<float, float>>> * outlines = new std::vector<std::vector<std::pair<float, float>>>;
					// chunk_data[i]->create_outlines(outlines);
					// (*chunk_outlines)[i] = outlines;
					// chunk_outlines->at(i) = outlines;

					// b2d_system->update_static_outlines(ent, outlines);
					// delete outlines;
				// }
			}
			//
		}
	};

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
					uint16_t explosion_radius = 10;
					// printf("Terrain hit\n");
					if(ud_a->type == b2fixture_types::PROJECTILE)
					{
						// delete circle shape around projectile
						((world_tile_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<world_tile_system>()))->delete_circle((int)(fixture_a->GetBody()->GetPosition().x + glsl_helper::projectile_width / 2), (int)(fixture_a->GetBody()->GetPosition().y + glsl_helper::projectile_height / 2), explosion_radius);

						// set projectile type to empty in ud_a
						ud_a->type = b2fixture_types::EMPTY;
						// (*ud_a).type = b2fixture_types::EMPTY;


						// void delete_circle(int x, int y, int radius, std::vector<std::vector<std::vector<std::pair<float, float>>>> *chunk_outlines)

						// ((projectile_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<projectile_system>()))->remove_projectile(ud_a->ent);
						// remove from box2d world

					}
					else
					{
						// delete circle shape around projectile
						((world_tile_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<world_tile_system>()))->delete_circle((int)(fixture_b->GetBody()->GetPosition().x + glsl_helper::projectile_width / 2), (int)(fixture_b->GetBody()->GetPosition().y + glsl_helper::projectile_height / 2), explosion_radius);
						// void delete_circle(int x, int y, int radius, std::vector<std::vector<std::vector<std::pair<float, float>>>> *chunk_outlines)

						// set projectile type to empty in ud_b
						ud_b->type = b2fixture_types::EMPTY;

						// ((projectile_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<projectile_system>()))->remove_projectile(ud_b->ent);
						// remove from box2d world
						// b2d_mutex.lock();
						// ((box2d_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<box2d_system>()))->world->DestroyBody(fixture_b->GetBody());
						// b2d_mutex.unlock();
					}
					return;
				}
			}
		}
	};

	// misc

	std::vector<game_engine::vec2> load_normal_vectors()
	{
		std::string path = "normal_vectors.txt";
		// stored in csv format: float, float,\n
		std::vector<game_engine::vec2> normal_vectors;

		std::fstream newfile;
		newfile.open(path, std::ios::in); // open a file to perform read operation using file object
		if (newfile.is_open())
		{ // checking whether the file is open
			std::string tp;
			while (getline(newfile, tp))
			{ // read data from file object and put it into string.
				std::stringstream ss(tp);
				std::string token;
				std::vector<std::string> tokens;
				while (getline(ss, token, ','))
				{
					tokens.push_back(token);
				}
				normal_vectors.push_back(game_engine::vec2(std::stof(tokens[0]), std::stof(tokens[1])));
			}
			newfile.close(); // close the file object.
		}
		return normal_vectors;
	}
} // namespace game
