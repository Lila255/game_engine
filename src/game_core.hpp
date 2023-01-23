#pragma once
#include <array>

#include "engine_comp.hpp"

namespace game
{
    const uint16_t NUM_CHUNKS = 16; // 3x3 chunks in world
    const uint16_t CHUNKS_WIDTH = 4;
    const uint16_t CHUNK_SIZE = 256; // There are CHUNK_SIZE*CHUNK_SIZE tiles in chunk

    siv::PerlinNoise perlin_noise(0.0);

    struct chunk
    {
    public:
        std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE> data;
        uint16_t chunk_x;
        uint16_t chunk_y;
        entity * background_entity;
        entity * foreground_entity;
        entity * light_entity;

        chunk() = default;
        chunk(uint16_t x, uint16_t y) : chunk_x(x), chunk_y(y)
        {
            data = std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE>{};
        }

        void create_chunk()
        {
            for (int x = 0; x < CHUNK_SIZE; x++)
            {
                for (int y = 0; y < CHUNK_SIZE; y++)
                {
                    double noise = perlin_noise.noise2D_01(x / 50.0, y / 50.0);
                    if (noise > 0.5)
                    {
                        data[x][y] = 1;
                    }
                    else
                    {
                        data[x][y] = 0;
                    }
                }
            }
        }

        void to_string()
        {
            for (int x = 0; x < CHUNK_SIZE; x++){
                for (int y = 0; y < CHUNK_SIZE; y++){
                    printf("%d ", data[x][y]);
                }
                printf("\n");
            }
            printf("\n");
        }

        void create_texture_from_chunk(GLuint & texture)
        {
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            // set data and size
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, CHUNK_SIZE, CHUNK_SIZE, 0, GL_RED, GL_UNSIGNED_BYTE, data.data());
            // set texture parameters
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    };

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
            for (int chunk = 0; chunk < NUM_CHUNKS; chunk++)
            {
                chunk_data[chunk]->create_chunk();
            }
        }
        // std::array<GLuint, NUM_CHUNKS> chunk_textures;

        std::array<GLuint, NUM_CHUNKS> create_chunk_textures()
        {
            std::array<GLuint, NUM_CHUNKS> textures;
            for (int chunk = 0; chunk < NUM_CHUNKS; chunk++)
            {
                chunk_data[chunk]->create_texture_from_chunk(textures[chunk]);
            }
            return textures;
        }
    };
} // namespace game
