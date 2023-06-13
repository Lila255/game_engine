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

#define M_PI 3.14159265358979323846	  /* pi */
#define radians(x) ((x)*M_PI / 180.0) // degrees to radians

#define PIXEL_SCALE 8

// typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
// typedef K::Point_2 point_2;
// typedef CGAL::Delaunay_triangulation_2<K>  Triangulation;
// typedef Triangulation::Edge_iterator  Edge_iterator;

namespace game
{
	const uint16_t NUM_CHUNKS = 9; // 3x3 chunks in world
	const uint16_t CHUNKS_WIDTH = 3;
	// const uint16_t CHUNK_SIZE = 128; // There are CHUNK_SIZE*CHUNK_SIZE tiles in chunk

	std::mutex b2d_mutex;

	// box2d system
	struct box2d_system : public game_engine::system
	{
	private:
		b2World *world;
		b2Vec2 gravity; //(0.0f, -9.8f);
		// std::vector<b2Body *> bodies;
		game_engine::sparse_component_set<b2Body *> static_bodies;
		game_engine::sparse_component_set<b2Body *> dynamic_bodies;

	public:
		// box2d_system() = default;
		box2d_system()
		{
			gravity = b2Vec2(0.0f, 51.8f);
			world = new b2World(gravity);
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
			chain.CreateLoop(vertices, mesh.size());
			b2FixtureDef fixtureDef;
			fixtureDef.shape = &chain;
			fixtureDef.density = 0.0f;
			fixtureDef.friction = 0.73f;
			body->CreateFixture(&fixtureDef);
			static_bodies.add(ent, body);
		}

		/// @brief Create a static body with multiple meshes
		/// @param ent The entity to associate the body with
		/// @param meshes The meshes to add to the body
		void create_static_bodies(entity ent, std::vector<std::vector<std::pair<float, float>>> meshes)
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
			for (int i = 0; i < meshes.size(); i++)
			{
				if (meshes[i].size() == 0 || meshes[i].size() % 3 != 0)
					continue;
				// if (meshes[i].size() < 3)
				//     continue;
				for (int j = 0; j < meshes[i].size(); j += 3)
				{
					b2PolygonShape chain;
					b2Vec2 *vertices = new b2Vec2[3];
					for (int k = 0; k < 3; k++)
					{
						vertices[k].Set(meshes[i][j + k].first, meshes[i][j + k].second);
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
					body->CreateFixture(&fixtureDef);
					delete[] vertices;
				}
			}
			static_bodies.add(ent, body);
		}

		void update_static_outlines(entity ent, std::vector<std::vector<std::pair<float, float>>> meshes)
		{
			if (!static_bodies.contains(ent))
				return;

			printf("Here, locking now\n");
			game::b2d_mutex.lock();
			printf("Locked\n");
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
			for (int i = 0; i < meshes.size(); i++)
			{
				if (meshes[i].size() == 0 || meshes[i].size() % 3 != 0)
					continue;
				// if (meshes[i].size() < 3)
				//     continue;
				for (int j = 0; j < meshes[i].size(); j += 3)
				{
					b2PolygonShape chain;
					b2Vec2 *vertices = new b2Vec2[3];
					for (int k = 0; k < 3; k++)
					{
						vertices[k].Set(meshes[i][j + k].first, meshes[i][j + k].second);
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
					fixtureDef.friction = 0.9f;
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
			body_def.position.Set(90.0f, -20.0f);
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
				fixture_def.density = 1.f;
				fixture_def.friction = 0.5f;
				fixture_def.restitution = .20f;
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
			world->DestroyBody(dynamic_bodies.get(ent));
			dynamic_bodies.remove(ent);
		}
		void update() {}
		void update(uint64_t time_to_step)
		{

			// world->Step((double)time_to_step / 1000.0, 6, 2);
			world->Step(1.0f / 144.0f, 6, 2);

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
			if(chunk_x < 0 || chunk_x >= CHUNKS_WIDTH || chunk_y < 0 || chunk_y >= CHUNKS_WIDTH)
				return (tile_type)0;
			if(tile_x < 0 || tile_x >= CHUNK_SIZE || tile_y < 0 || tile_y >= CHUNK_SIZE)
				return (tile_type)0;
			return (chunk_data[chunk]) -> get_tile(tile_x, tile_y);
		}

		void set_tile_at(int x, int y, uint8_t tile_type)
		{
			int chunk_x = x / CHUNK_SIZE;
			int chunk_y = y / CHUNK_SIZE;
			int chunk = chunk_x + chunk_y * CHUNKS_WIDTH;
			int tile_x = x % CHUNK_SIZE;
			int tile_y = y % CHUNK_SIZE;
			if(chunk_x < 0 || chunk_x >= CHUNKS_WIDTH || chunk_y < 0 || chunk_y >= CHUNKS_WIDTH)
				return;
			
			(chunk_data[chunk]) -> set_tile(tile_x, tile_y, tile_type);
		}
		
		std::array<entity, NUM_CHUNKS> get_chunk_entities()
		{
			return chunk_entities;
		}

		void update() {}

		void update(uint64_t tick_count)
		{
			for (int y = CHUNKS_WIDTH * CHUNK_SIZE - 1; y >= 0 ; y--)
			{
				for (int x = 0; x < CHUNKS_WIDTH * CHUNK_SIZE; x++)
				{
					int chunk_x = x / CHUNK_SIZE;
					int chunk_y = y / CHUNK_SIZE;
					int chunk = chunk_x + chunk_y * CHUNKS_WIDTH;
					int tile_x = x % CHUNK_SIZE;
					int tile_y = y % CHUNK_SIZE;

					uint8_t tile_type = get_tile_at(x, y);
					switch (tile_type)
					{
					case GRASS:
						if (rand() % 20 == 0)	// 1% chance to try growing grass
						{
							int dx[] = {0, 1, 1, 1, 0, -1, -1, -1};
							int dy[] = {-1, -1, 0, 1, 1, 1, 0, -1};
							int r = rand() % 8;
							for(int i = 0; i < 8; i++){
								int xx = x + dx[(r + i) % 8];
								int yy = y + dy[(r + i) % 8];
								if(get_tile_at(xx, yy) == DIRT && get_tile_at(xx, yy - 1) == AIR){
									set_tile_at(xx, yy, GRASS);
									break;
								}
							}
						}
						if(get_tile_at(x, y - 1) == AIR && rand() % 100 == 0){
							set_tile_at(x, y, DIRT);
						}
						break;

					case SAND:
						if(get_tile_at(x, y + 1) == AIR){
							set_tile_at(x, y + 1, SAND);
							set_tile_at(x, y, AIR);
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
				int chunk_y	= chunk / CHUNKS_WIDTH;
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

		std::vector<std::vector<std::pair<float, float>>> create_outlines(int x, int y)
		{
			return chunk_data[x + y * CHUNKS_WIDTH]->create_outlines();
		}

		void delete_circle(int x, int y, int radius, std::vector<std::vector<std::vector<std::pair<float, float>>>> *chunk_outlines)
		{
			// get texture system
			auto texture_system = (game_engine::render_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::render_system>());
			// get b2d system
			auto b2d_system = (box2d_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<box2d_system>());

			for (int i = 0; i < NUM_CHUNKS; i++)
			{
				bool modified = chunk_data[i]->delete_circle(x, y, radius);
				if (modified)
				{
					std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE> *tile_data = chunk_data[i]->get_data();
					entity ent = chunk_entities[i];
					// texture_system->update_texture(ent, (uint8_t *)tile_data->data(), CHUNK_SIZE, CHUNK_SIZE, game_engine::shader_programs[0]);
					int chunkx = i % CHUNKS_WIDTH;
					int chunky = i / CHUNKS_WIDTH;
					texture_system->update_texture_section(all_chunk_ent, (uint8_t *)tile_data->data(), chunkx * CHUNK_SIZE, chunky * CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE);
					std::vector<std::vector<std::pair<float, float>>> outlines = chunk_data[i]->create_outlines();
					// (*chunk_outlines)[i] = outlines;
					// chunk_outlines->at(i) = outlines;

					b2d_system->update_static_outlines(ent, outlines);
				}
			}
			//
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
