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

#define PIXEL_SCALE 6

// typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
// typedef K::Point_2 point_2;
// typedef CGAL::Delaunay_triangulation_2<K>  Triangulation;
// typedef Triangulation::Edge_iterator  Edge_iterator;

namespace game
{
	const uint16_t NUM_CHUNKS = 9; // 3x3 chunks in world
	const uint16_t CHUNKS_WIDTH = 3;
	// const uint16_t CHUNK_SIZE = 128; // There are CHUNK_SIZE*CHUNK_SIZE tiles in chunk

	// enum for tile types
	enum tile_type
	{
		AIR,	// 0
		SMOKE,	// 1
		WATER,	// 2
		GLASS,	// 3
		SAND,	// 4
		DIRT,	// 5
		STONE,	// 6
		WOOD,	// 7
		LEAF,	// 8
		GRASS,	// 9
		
		BRICK_1,
		BRICK_2,
		MORTAR,

	};

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
		entity all_chunk_ent;

	public:
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

		void update() override
		{
			throw std::runtime_error("world_tile_system::update() not implemented yet");
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
					texture_system->update_texture_section(all_chunk_ent, (uint8_t *)tile_data->data(), chunkx * CHUNK_SIZE, chunky * CHUNK_SIZE, CHUNK_SIZE, CHUNK_SIZE, game_engine::shader_programs[0]);
					std::vector<std::vector<std::pair<float, float>>> outlines = chunk_data[i]->create_outlines();
					(*chunk_outlines)[i] = outlines;
					// chunk_outlines->at(i) = outlines;

					b2d_system->update_static_outlines(ent, outlines);
				}
			}
			//
		}
	};

	// misc

	// 256 vec2 values for each tile permutations normal
	std::vector<game_engine::vec2> noramal_vectors = {
		{0.0f, 0.0f},	//	0	00000000
		{1.0f, -1.0f},	//	1	00000001
		{1.0f, 0.0f},	//	2	00000010
		{0.8f, -1.0f},	//	3	00000011
		{-1.0f, -1.0f},	//	4	00000100
		{0.0f, -1.0f},	//	5	00000101
		{-0.8f, -1.0f},	//	6	00000110
		{0.0f, -1.0f},	//	7	00000111
		{0.0f, 1.0f},	//	8	00001000
		{-0.8f, 1.0f},	//	9	00001001
		{-1.0f, -1.0f},	//	10	00001010
		{-1.0f, -1.0f},	//	11	00001011
		{-0.2f, -1.0f},	//	12	00001100
		{-0.2f, -1.0f},	//	13	00001101
		{-0.2f, -1.0f},	//	14	00001110
		{-0.2f, -1.0f},	//	15	00001111
		{0.0f, 1.0f},	//	16	00010000
		{0.2f, -1.0f},	//	17	00010001
		{1.0f, -1.0f},	//	18	00010010
		{1.0f, -0.2f},	//	19	00010011
		{0.2f, 1.0f},	//	20	00010100
		{0.2f, 1.0f},	//	21	00010101
		{1.0f, -1.0f},	//	22	00010110
		{0.2f, -1.0f},	//	23	00010111
		{0.0f, 1.0f},	//	24	00011000
		{0.0f, 1.0f},	//	25	00011001
		{0.0f, 1.0f},	//	26	00011010
		{0.0f, 1.0f},	//	27	00011011
		{0.0f, -1.0f},	//	28	00011100
		{0.0f, -1.0f},	//	29	00011101
		{0.0f, -1.0f},	//	30	00011110
		{0.0f, 1.0f},	//	31	00011111
		{1.0f, 1.0f},	//	32	00100000
		{-1.0f, 0.0f},	//	33	00100001
		{-1.0f, -0.2f},	//	34	00100010
		{-1.0f, -0.2f},	//	35	00100011
		{1.0f, 1.0f},	//	36	00100100
		{-1.0f, -1.0f},	//	37	00100101
		{-1.0f, -1.0f},	//	38	00100110
		{-1.0f, -1.0f},	//	39	00100111
		{-0.2f, -1.0f},	//	40	00101000
		{-1.0f, 0.0f},	//	41	00101001
		{-0.2f, -1.0f},	//	42	00101010
		{-0.2f, -1.0f},	//	43	00101011
		{0.3f, 1.0f},	//	44	00101100
		{-1.0f, -1.0f},	//	45	00101101
		{-1.0f, -1.0f},	//	46	00101110
		{-1.0f, -1.0f},	//	47	00101111
		{0.3f, 1.0f},	//	48	00110000
		{0.0f, 1.0f},	//	49	00110001
		{1.0f, 1.0f},	//	50	00110010
		{-0.5f, -1.0f},	//	51	00110011
		{0.3f, 1.0f},	//	52	00110100
		{-0.4f, -1.0f},	//	53	00110101
		{1.0f, 1.0f},	//	54	00110110
		{-0.5f, -1.0f},	//	55	00110111
		{-0.3f, -1.0f},	//	56	00111000
		{0.0f, 1.0f},	//	57	00111001
		{-0.4f, -1.0f},	//	58	00111010
		{-0.4f, -1.0f},	//	59	00111011
		{1.0f, 1.0f},	//	60	00111100
		{-0.4f, -1.0f},	//	61	00111101
		{-0.4f, -1.0f},	//	62	00111110
		{-0.4f, -1.0f},	//	63	00111111
		{1.0f, 0.0f},	//	64	01000000
		{-1.0f, 0.5f},	//	65	01000001
		{1.0f, 0.0f},	//	66	01000010
		{1.0f, 0.0f},	//	67	01000011
		{1.0f, 0.5f},	//	68	01000100
		{1.0f, 0.0f},	//	69	01000101
		{1.0f, 0.0f},	//	70	01000110
		{0.0f, 1.0f},	//	71	01000111
		{-1.0f, 1.0f},	//	72	01001000
		{-1.0f, 0.5f},	//	73	01001001
		{1.0f, 0.0f},	//	74	01001010
		{1.0f, 0.0f},	//	75	01001011
		{1.0f, 1.0f},	//	76	01001100
		{-1.0f, -1.0f},	//	77	01001101
		{-1.0f, -1.0f},	//	78	01001110
		{-1.0f, -1.0f},	//	79	01001111
		{1.0f, 1.0f},	//	80	01010000
		{-1.0f, 1.0f},	//	81	01010001
		{1.0f, 0.0f},	//	82	01010010
		{1.0f, -1.0f},	//	83	01010011
		{1.0f, 0.5f},	//	84	01010100
		{1.0f, -0.6f},	//	85	01010101
		{1.0f, 0.0f},	//	86	01010110
		{1.0f, -1.0f},	//	87	01010111
		{0.0f, 1.0f},	//	88	01011000
		{-0.2f, 1.0f},	//	89	01011001
		{1.0f, 1.0f},	//	90	01011010
		{1.0f, -1.0f},	//	91	01011011
		{0.3f, 1.0f},	//	92	01011100
		{0.0f, 1.0f},	//	93	01011101
		{1.0f, 1.0f},	//	94	01011110
		{0.0f, 1.0f},	//	95	01011111
		{1.0f, 0.2f},	//	96	01100000
		{-1.0f, 0.5f},	//	97	01100001
		{1.0f, 0.0f},	//	98	01100010
		{1.0f, 0.0f},	//	99	01100011
		{1.0f, 1.0f},	//	100	01100100
		{1.0f, 1.0f},	//	101	01100101
		{1.0f, 1.0f},	//	102	01100110
		{1.0f, 0.0f},	//	103	01100111
		{-1.0f, 1.0f},	//	104	01101000
		{-1.0f, 0.5f},	//	105	01101001
		{1.0f, 0.0f},	//	106	01101010
		{1.0f, 0.0f},	//	107	01101011
		{1.0f, 1.0f},	//	108	01101100
		{-1.0f, -1.0f},	//	109	01101101
		{1.0f, 1.0f},	//	110	01101110
		{1.0f, 1.0f},	//	111	01101111
		{0.5f, 1.0f},	//	112	01110000
		{1.0f, -1.0f},	//	113	01110001
		{1.0f, 1.0f},	//	114	01110010
		{1.0f, 0.0f},	//	115	01110011
		{1.0f, 1.0f},	//	116	01110100
		{1.0f, 1.0f},	//	117	01110101
		{1.0f, 1.0f},	//	118	01110110
		{1.0f, 1.0f},	//	119	01110111
		{0.0f, 1.0f},	//	120	01111000
		{-1.0f, 1.0f},	//	121	01111001
		{-1.0f, 1.0f},	//	122	01111010
		{1.0f, 0.0f},	//	123	01111011
		{1.0f, 1.0f},	//	124	01111100
		{-0.8f, -1.0f},	//	125	01111101
		{1.0f, 1.0f},	//	126	01111110
		{1.0f, 1.0f},	//	127	01111111
		{1.0f, -1.0f},	//	128 10000000
		{1.0f, -1.0f},	//	129	10000001
		{1.0f, -0.5f},	//	130	10000010
		{1.0f, -1.0f},	//	131	10000011
		{1.0f, 0.0f},	//	132	10000100
		{1.0f, -1.0f},	//	133	10000101
		{1.0f, -0.5f},	//	134	10000110
		{1.0f, -1.0f},	//	135	10000111
		{-0.5f, 1.0f},	//	136	10001000
		{-1.0f, 1.0f},	//	137	10001001
		{1.0f, -1.0f},	//	138	10001010
		{1.0f, -1.0f},	//	139	10001011
		{0.0f, 1.0f},	//	140	10001100
		{1.0f, -1.0f},	//	141	10001101
		{1.0f, -1.0f},	//	142	10001110
		{1.0f, -1.0f},	//	143	10001111
		{-0.3f, 1.0f},	//	144	10010000
		{1.0f, -1.0f},	//	145	10010001
		{1.0f, -0.5f},	//	146	10010010
		{1.0f, -1.0f},	//	147	10010011
		{1.0f, 0.0f},	//	148	10010100
		{1.0f, -1.0f},	//	149	10010101
		{1.0f, -0.5f},	//	150	10010110
		{1.0f, -1.0f},	//	151	10010111
		{0.0f, 1.0f},	//	152	10011000
		{1.0f, -1.0f},	//	153	10011001
		{1.0f, -1.0f},	//	154	10011010
		{1.0f, -1.0f},	//	155	10011011
		{0.0f, 1.0f},	//	156 10011100
		{0.0f, 1.0f},	//	157	10011101
		{0.0f, 1.0f},	//	158	10011110
		{1.0f, -1.0f},	//	159	10011111
		{0.0f, 1.0f},	//	160	10100000
		{-1.0f, 1.0f},	//	161	10100001
		{1.0f, 0.0f},	//	162	10100010
		{-1.0f, 1.0f},	//	163	10100011
		{1.0f, 1.0f},	//	164	10100100
		{1.0f, 0.0f},	//	165	10100101
		{1.0f, 1.0f},	//	166	10100110
		{1.0f, 0.0f},	//	167	10100111
		{-0.5f, 1.0f},	//	168	10101000
		{-1.0f, 1.0f},	//	169	10101001
		{-1.0f, 1.0f},	//	170	10101010
		{-1.0f, 1.0f},	//	171	10101011
		{0.0f, 1.0f},	//	172	10101100
		{0.0f, 1.0f},	//	173	10101101
		{1.0f, 1.0f},	//	174	10101110
		{1.0f, 1.0f},	//	175	10101111
		{0.5f, 1.0f},	//	176	10110000
		{-1.0f, 1.0f},	//	177 10110001
		{0.8f, 1.0f},	//	178	10110010
		{-1.0f, 1.0f},	//	179	10110011
		{1.0f, 1.0f},	//	180	10110100
		{0.0f, 1.0f},	//	181	10110101
		{1.0f, 1.0f},	//	182	10110110
		{1.0f, -1.0f},	//	183	10110111
		{0.0f, 1.0f},	//	184	10111000
		{-1.0f, 1.0f},	//	185	10111001
		{0.0f, 1.0f},	//	186	10111010
		{0.3f, -1.0f},	//	187	10111011
		{1.0f, 1.0f},	//	188	10111100
		{0.0f, 1.0f},	//	189	10111101
		{1.0f, 1.0f},	//	190	10111110
		{0.0f, 1.0f},	//	191	10111111
		{-1.0f, 0.2f},	//	192	11000000
		{-1.0f, 1.0f},	//	193	11000001
		{1.0f, 0.0f},	//	194	11000010
		{1.0f, -1.0f},	//	195	11000011
		{1.0f, 0.5f},	//	196	11000100
		{1.0f, -1.0f},	//	197	11000101
		{1.0f, 0.0f},	//	198	11000110
		{1.0f, -1.0f},	//	199	11000111
		{-0.5, 1.0f},	//	200	11001000
		{-1.0f, 1.0f},	//	201	11001001
		{-1.0f, 1.0f},	//	202	11001010
		{-1.0f, 1.0f},	//	203	11001011
		{1.0f, 1.0f},	//	204	11001100
		{1.0f, -1.0f},	//	205	11001101
		{1.0f, 0.0f},	//	206	11001110
		{1.0f, -1.0f},	//	207	11001111
		{1.0f, 1.0f},	//	208	11010000
		{1.0f, -1.0f},	//	209	11010001
		{1.0f, 0.0f},	//	210	11010010
		{1.0f, -1.0f},	//	211	11010011
		{1.0f, 0.5f},	//	212	11010100
		{1.0f, -1.0f},	//	213	11010101
		{1.0f, 0.0f},	//	214	11010110
		{1.0f, -1.0f},	//	215	11010111
		{0.0f, 1.0f},	//	216	11011000
		{-1.0f, 1.0f},	//	217	11011001
		{1.0f, -1.0f},	//	218	11011010
		{1.0f, -1.0f},	//	219	11011011
		{1.0f, 1.0f},	//	220	11011100
		{0.0f, 1.0f},	//	221	11011101
		{1.0f, 0.0f},	//	222	11011110
		{1.0f, -1.0f},	//	223	11011111
		{0.0f, 1.0f},	//	224	11100000
		{-1.0f, 1.0f},	//	225	11100001
		{1.0f, 0.0f},	//	226	11100010
		{1.0f, 0.0f},	//	227	11100011
		{1.0f, 1.0f},	//	228	11100100
		{1.0f, 0.0f},	//	229	11100101
		{1.0f, 1.0f},	//	230	11100110
		{1.0f, 0.0f},	//	231	11100111
		{-0.5f, 1.0f},	//	232	11101000
		{-1.0f, 1.0f},	//	233	11101001
		{-1.0f, 1.0f},	//	234	11101010
		{-1.0f, 1.0f},	//	235	11101011
		{1.0f, 1.0f},	//	236	11101100
		{-1.0f, 1.0f},	//	237	11101101
		{1.0f, 0.0f},	//	238	11101110
		{1.0f, 0.0f},	//	239	11101111
		{0.5f, 1.0f},	//	240	11110000
		{-0.5f, 1.0f},	//	241	11110001
		{0.8f, 1.0f},	//	242	11110010
		{1.0f, 0.0f},	//	243	11110011
		{1.0f, 1.0f},	//	244	11110100
		{1.0f, 1.0f},	//	245	11110101
		{1.0f, 1.0f},	//	246	11110110
		{1.0f, 0.0f},	//	247	11110111
		{0.0f, 1.0f},	//	248	11111000
		{-0.5f, 1.0f},	//	249	11111001
		{0.0f, 1.0f},	//	250	11111010
		{-1.0f, 1.0f},	//	251	11111011
		{0.5f, 1.0f},	//	252	11111100
		{0.0f, 1.0f},	//	253	11111101
		{1.0f, 1.0f},	//	254	11111110
		{0.0f, 0.0f} 	//	255	11111111
	};

	std::vector<game_engine::vec2> load_normal_vectors()
	{
		std::string path = "normal_vectors.txt";
		// stored in csv format: float, float,\n
		std::vector<game_engine::vec2> normal_vectors;

		std::fstream newfile;
		newfile.open(path, std::ios::in); //open a file to perform read operation using file object
		if (newfile.is_open()) { //checking whether the file is open
			std::string tp;
			while (getline(newfile, tp)) { //read data from file object and put it into string.
				std::stringstream ss(tp);
				std::string token;
				std::vector<std::string> tokens;
				while (getline(ss, token, ',')) {
					tokens.push_back(token);
				}
				normal_vectors.push_back(game_engine::vec2(std::stof(tokens[0]), std::stof(tokens[1])));
			}
			newfile.close(); //close the file object.
		}
		return normal_vectors;
	}
} // namespace game
