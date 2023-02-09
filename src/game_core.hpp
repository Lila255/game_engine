#pragma once
#include <array>
#include <chrono>
#include <cmath>
// #include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
// #include <CGAL/Delaunay_triangulation_2.h>
#include <utility>
#include <poly2tri/poly2tri.h>
// box2d
#include <box2d/box2d.h>

#include "engine_comp.hpp"
#include "chunk.hpp"

#define M_PI 3.14159265358979323846   /* pi */
#define radians(x) ((x)*M_PI / 180.0) // degrees to radians

// typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
// typedef K::Point_2 point_2;
// typedef CGAL::Delaunay_triangulation_2<K>  Triangulation;
// typedef Triangulation::Edge_iterator  Edge_iterator;

namespace game
{
    const uint16_t NUM_CHUNKS = 16; // 3x3 chunks in world
    const uint16_t CHUNKS_WIDTH = 4;
    // const uint16_t CHUNK_SIZE = 128; // There are CHUNK_SIZE*CHUNK_SIZE tiles in chunk

    // siv::PerlinNoise perlin_noise(0.0);

    struct world_tile_system : public game_engine::system
    {
    private:
        std::array<chunk *, NUM_CHUNKS> chunk_data{};
        // std::array<game_engine::entity *, NUM_CHUNKS> chunk_entities;

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

        void update() override
        {
            throw std::runtime_error("world_tile_system::update() not implemented yet");
        }

        void generate_world()
        {
            for (int chunk = 1; chunk < NUM_CHUNKS; chunk++)
            {
                chunk_data[chunk]->create_chunk();
            }
        }
        // std::array<GLuint, NUM_CHUNKS> chunk_textures;

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
            return chunk_data[x + y * CHUNKS_WIDTH]->create_outlines_centers();
        }
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
            gravity = b2Vec2(0.0f, 199.8f);
            world = new b2World(gravity);
        }

        /// @brief Create and add a static body to the b2d world
        /// @param ent The entity to associate the body with
        /// @param mesh The triangulated mesh to add to the body
        void create_static_body(entity ent, std::vector<std::pair<float, float>> mesh)
        {

            if(mesh.size() < 3) return;
            // static_bodies.add(ent, body);
            b2BodyDef bodyDef;
            bodyDef.type = b2_staticBody;
            b2Body *body = world -> CreateBody(&bodyDef);
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
            b2BodyDef body_def;
            body_def.type = b2_dynamicBody;
            body_def.position.Set(110.0f, 100.0f);
            b2Body *body = world->CreateBody(&body_def);
            b2PolygonShape dynamic_box;
            // dynamic_box.SetAsBox(2,2);
            dynamic_box.SetAsBox(glsl_helper::character_width/2.0f, glsl_helper::character_height/2.0f);
            b2FixtureDef fixture_def;
            fixture_def.shape = &dynamic_box;
            fixture_def.density = 5.4f;
            fixture_def.friction = 0.3f;
            body->CreateFixture(&fixture_def);
            dynamic_bodies.add(ent, body);
        }
        

        void remove_dynamic_body(entity ent)
        {
            world->DestroyBody(dynamic_bodies.get(ent));
            dynamic_bodies.remove(ent);
        }

        void update() override
        {
            
            world->Step(1.0f / 60.0f, 6, 2);
            
            b2Body *body = dynamic_bodies.get(game_engine::game_engine_pointer->player_entitiy);
            b2Vec2 position = body->GetPosition();

            // get player position
            // b2Body *body = dynamic_bodies.get(game_engine::game_engine_pointer->player_entitiy);
            // b2Vec2 position = body->GetPosition();
            // printf("x: %f, y: %f\n", position.x, position.y);
            // // get box position
            game_engine::box_system *bo_system_pointer = ((game_engine::box_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::box_system>()));
            game_engine::box b = bo_system_pointer->get(game_engine::game_engine_pointer->player_entitiy);
            b.x = position.x - glsl_helper::character_width/2.0f;
            b.y = position.y - glsl_helper::character_height/2.0f;
            bo_system_pointer->update_box(game_engine::game_engine_pointer->player_entitiy, b);
            game_engine::texture_vbo_system *tex_vbo_system_pointer = ((game_engine::texture_vbo_system *)game_engine::game_engine_pointer->get_system(game_engine::family::type<game_engine::texture_vbo_system>()));
            tex_vbo_system_pointer->update(game_engine::game_engine_pointer->player_entitiy);
        }

        b2Body * get_static_body(entity ent)
        {
            return static_bodies.get(ent);
        }
        b2Body * get_dynamic_body(entity ent)
        {
            return dynamic_bodies.get(ent);
        }
    };

} // namespace game
