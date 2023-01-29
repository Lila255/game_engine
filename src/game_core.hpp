#pragma once
#include <array>

#include "engine_comp.hpp"

namespace game
{
    const uint16_t NUM_CHUNKS = 9; // 3x3 chunks in world
    const uint16_t CHUNKS_WIDTH = 3;
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
        int dx[8] = { 0, 1, 1, 1, 0, -1, -1, -1 };
        int dy[8] = { -1, -1, 0, 1, 1, 1, 0, -1 };


        chunk() = default;
        chunk(uint16_t x, uint16_t y) : chunk_x(x), chunk_y(y)
        {
            data = std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE>{};
        }

        void create_chunk()
        {
            for (int y = 0; y < CHUNK_SIZE; y++)
            {
                for (int x = 0; x < CHUNK_SIZE; x++)
                {
                    double noise = perlin_noise.noise2D_01((x + chunk_x * game::CHUNK_SIZE) / 50.0, (y + chunk_y * game::CHUNK_SIZE) / 50.0);
                    if (noise > 0.5)
                    {
                        data[y][x] = 1;
                    }
                    else
                    {
                        data[y][x] = 0;
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
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, CHUNK_SIZE, CHUNK_SIZE, 0, GL_RED, GL_UNSIGNED_BYTE, data.data());
            // set texture parameters
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
            // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
            glBindTexture(GL_TEXTURE_2D, 0);
        }


        // // Helper function to check if a tile is a boundary tile
        // bool isBoundaryTile(int x, int y) {
        //     int rows = data.size();
        //     int cols = data[0].size();
        //     for (int i = -1; i <= 1; i++) {
        //         for (int j = -1; j <= 1; j++) {
        //             int newX = x + i;
        //             int newY = y + j;
        //             if (newX >= 0 && newX < rows && newY >= 0 && newY < cols && data[newX][newY] == 0) {
        //                 return true;
        //             }
        //         }
        //     }
        //     return false;
        // }
        bool isBoundaryTile(int x, int y) {
            int rows = data.size();
            int cols = data[0].size();
            for (int i = -1; i <= 1; i++) {
                for (int j = -1; j <= 1; j++) {
                    int newX = x + i;
                    int newY = y + j;
                    if (newX < 0 || newX >= rows || newY < 0 || newY >= cols || data[newX][newY] == 0) {
                        return true;
                    }
                }
            }
            return false;
        }

        // Helper function to get the next boundary tile
        std::pair<int, int> getNextBoundaryTile(int x, int y, int direction) {
            int rows = data.size();
            int cols = data[0].size();
            int newX = x + dx[direction];
            int newY = y + dy[direction];
            if (newX >= 0 && newX < rows && newY >= 0 && newY < cols && data[newX][newY] > 0) {
                return std::make_pair(newX, newY);
            }
            int newDirection = (direction + 1) % 8;
            newX = x + dx[newDirection];
            newY = y + dy[newDirection];
            if (newX >= 0 && newX < rows && newY >= 0 && newY < cols && data[newX][newY] > 0) {
                return std::make_pair(newX, newY);
            }
            return std::make_pair(-1, -1);
        }

        std::vector<std::vector<std::pair<int, int>>> create_outlines()
        {
            std::vector<std::vector<std::pair<int, int>>> outlines;
            std::unordered_set<std::pair<int, int>, game_engine::pair_hash> visited;
            for (int y = 0; y < CHUNK_SIZE; y++)
            {
                for (int x = 0; x < CHUNK_SIZE; x++)
                {
                    std::pair<int, int> tile = std::make_pair(x, y);
                    if (data[x][y] > 0 && !visited.count(tile))
                    {
                        if (isBoundaryTile(x, y))
                        {
                            int start_x = x;
                            int start_y = y;
                            int direction = 0;

                            outlines.push_back({std::make_pair(start_x + chunk_x * CHUNK_SIZE, start_y + chunk_y * CHUNK_SIZE)});
                            visited.insert(tile);
                            while (true) 
                            {

                                std::pair<int, int> next_tile = getNextBoundaryTile(start_x, start_y, direction);
                                int next_x = std::get<0>(next_tile);
                                int next_y = std::get<1>(next_tile);
                                visited.insert(next_tile);
                                if (next_x == -1 && next_y == -1 || (next_x == x && next_y == y)) {
                                    break;
                                }
                                outlines.back().push_back(std::make_pair(next_x + chunk_x * CHUNK_SIZE, next_y + chunk_y * CHUNK_SIZE));
                                //.push_back(std::make_pair(next_x, next_y));
                                start_x = next_x;
                                start_y = next_y;
                                direction = (direction + 7) % 8;

                            }

                        }
                    }
                }
            }
            return outlines;
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

        std::vector<std::vector<std::pair<int, int>>> create_outlines(int x, int y)
        {
            return chunk_data[x + y * CHUNKS_WIDTH]->create_outlines();
        }
    };
} // namespace game
