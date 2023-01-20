#pragma once
#include <array>

#include "engine_comp.hpp"

namespace game
{
    const uint16_t NUM_CHUNKS = 9; // 3x3 chunks in world
    const uint16_t CHUNKS_WIDTH = 3;
    const uint16_t CHUNK_SIZE = 512; // 4x4 tiles in chunk

    siv::PerlinNoise perlin_noise(0.0);

    struct chunk
    {
    public:
        std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE> data;
        uint16_t chunk_x;
        uint16_t chunk_y;

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
                    double noise = perlin_noise.noise2D_01(x, y);
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
    };

    struct world_tile_system : public game_engine::system
    {
    private:
        std::array<chunk *, NUM_CHUNKS> chunk_data;
        // std::
        // std::array<std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE> *, NUM_CHUNKS> chunk_data;

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
    };
} // namespace game
