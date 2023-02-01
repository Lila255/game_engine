#pragma once
#include <array>
#include <chrono>
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
        entity *background_entity;
        entity *foreground_entity;
        entity *light_entity;
        // int dx[8] = {0, 1, 1, 1, 0, -1, -1, -1};
        // int dy[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
        const int8_t dx[4] = {0, 1, 0, -1};
        const int8_t dy[4] = {-1, 0, 1, 0};
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
            for (int x = 0; x < CHUNK_SIZE; x++)
            {
                for (int y = 0; y < CHUNK_SIZE; y++)
                {
                    printf("%d ", data[x][y]);
                }
                printf("\n");
            }
            printf("\n");
        }

        void create_texture_from_chunk(GLuint &texture)
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
        // bool isBoundaryTile(int x, int y)
        // {
        //     int rows = data.size();
        //     int cols = data[0].size();
        //     for (int i = -1; i <= 1; i++)
        //     {
        //         for (int j = -1; j <= 1; j++)
        //         {
        //             int newX = x + i;
        //             int newY = y + j;
        //             if (newX < 0 || newX >= rows || newY < 0 || newY >= cols || data[newX][newY] == 0)
        //             {
        //                 return true;
        //             }
        //         }
        //     }
        //     return false;
        // }
        // // Helper function to get the next boundary tile
        // std::pair<int, int> getNextBoundaryTile(int x, int y, int direction)
        // {
        //     int rows = data.size();
        //     int cols = data[0].size();
        //     int newX = x + dx[direction];
        //     int newY = y + dy[direction];
        //     if (newX >= 0 && newX < rows && newY >= 0 && newY < cols && data[newX][newY] > 0)
        //     {
        //         return std::make_pair(newX, newY);
        //     }
        //     int newDirection = (direction + 1) % 8;
        //     newX = x + dx[newDirection];
        //     newY = y + dy[newDirection];
        //     if (newX >= 0 && newX < rows && newY >= 0 && newY < cols && data[newX][newY] > 0)
        //     {
        //         return std::make_pair(newX, newY);
        //     }
        //     return std::make_pair(-1, -1);
        // }
        //     std::vector<std::vector<std::pair<int, int>>> create_outlines()
        //     {
        //         // get time in nano seconds
        //         auto start = std::chrono::high_resolution_clock::now();
        //         std::vector<std::vector<std::pair<int, int>>> outlines;
        //         std::unordered_set<std::pair<int, int>, game_engine::pair_hash> visited;
        //         for (int x = 0; x < CHUNK_SIZE; x++)
        //         {
        //             for (int y = 0; y < CHUNK_SIZE; y++)
        //             {
        //                 std::pair<int, int> tile = std::make_pair(x, y);
        //                 if (data[x][y] > 0 && !visited.count(tile))
        //                 {
        //                     if (isBoundaryTile(x, y))
        //                     {
        //                         int start_x = x;
        //                         int start_y = y;
        //                         int direction = 0;
        //                         outlines.push_back({std::make_pair(start_x + chunk_x * CHUNK_SIZE, start_y + chunk_y * CHUNK_SIZE)});
        //                         visited.insert(tile);
        //                         while (true)
        //                         {
        //                             std::pair<int, int> next_tile = getNextBoundaryTile(start_x, start_y, direction);
        //                             int next_x = std::get<0>(next_tile);
        //                             int next_y = std::get<1>(next_tile);
        //                             visited.insert(next_tile);
        //                             if (next_x == -1 && next_y == -1 || (next_x == x && next_y == y)) {
        //                                 break;
        //                             }
        //                             outlines.back().push_back(std::make_pair(next_x + chunk_x * CHUNK_SIZE, next_y + chunk_y * CHUNK_SIZE));
        //                             //.push_back(std::make_pair(next_x, next_y));
        //                             start_x = next_x;
        //                             start_y = next_y;
        //                             direction = (direction + 7) % 8;
        //                         }
        //                     }
        //                 }
        //             }
        //         }
        //         // decimate the meshes
        //         for (int i = 0; i < outlines.size(); i++)
        //         {
        //             std::vector<std::pair<int, int>> outline = outlines[i];
        //             std::vector<std::pair<int, int>> new_outline;
        //             new_outline.push_back(outline[0]);
        //             for (int j = 1; j < outline.size(); j++)
        //             {
        //                 std::pair<int, int> p1 = outline[j - 1];
        //                 std::pair<int, int> p2 = outline[j];
        //                 std::pair<int, int> p3 = outline[(j + 1) % outline.size()];
        //                 if (p1.first != p2.first || p1.second != p2.second)
        //                 {
        //                     if (p2.first != p3.first || p2.second != p3.second)
        //                     {
        //                         new_outline.push_back(p2);
        //                     }
        //                 }
        //             }
        //             outlines[i] = new_outline;
        //         }
        //         int decimation = 8;
        //         // remove points to keep only one every decimation value
        //         for (int i = 0; i < outlines.size(); i++)
        //         {
        //             std::vector<std::pair<int, int>> outline = outlines[i];
        //             std::vector<std::pair<int, int>> new_outline;
        //             for (int j = 0; j < outline.size(); j++)
        //             {
        //                 if (j % decimation == 0)
        //                 {
        //                     new_outline.push_back(outline[j]);
        //                 }
        //             }
        //             outlines[i] = new_outline;
        //         }
        //         auto end = std::chrono::high_resolution_clock::now();
        //         auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
        //         printf("Time taken by function: %d\n", duration.count());
        //         // print the number of lines and the number of points in each line
        //         printf("Number of lines: %d\n", outlines.size());
        //         for (int i = 0; i < outlines.size(); i++)
        //         {
        //             printf("Number of points in line %d: %d\n", i, outlines[i].size());
        //         }
        //         printf("\n");
        //         return outlines;
        //     }
        // };

        bool isBoundaryTile(int x, int y)
        {
            int rows = data.size();
            int cols = data[0].size();
            for (int i = -1; i <= 1; i++)
            {
                for (int j = -1; j <= 1; j++)
                {
                    int newX = x + i;
                    int newY = y + j;
                    if (newX < 0 || newX >= rows || newY < 0 || newY >= cols || data[newX][newY] == 0)
                    {
                        return true;
                    }
                }
            }
            return false;
        }

        std::pair<int, int> getNextBoundaryTile(int x, int y, int direction)
        {
            int rows = data.size();
            int cols = data[0].size();
            int newX = x + dx[direction];
            int newY = y + dy[direction];
            if (newX >= 0 && newX < rows && newY >= 0 && newY < cols && data[newX][newY] > 0)
            {
                return std::make_pair(newX, newY);
            }
            // int newDirection = (direction + 1) % 8;
            // newX = x + dx[newDirection];
            // newY = y + dy[newDirection];
            // if (newX >= 0 && newX < rows && newY >= 0 && newY < cols && data[newX][newY] > 0) {
            //     return std::make_pair(newX, newY);
            // }
            return std::make_pair(-1, -1);
            // int rows = data.size();
            // int cols = data[0].size();
            // int newX;
            // int newY;

            // for (int i = 0; i < 8; i++)
            // {

            //     int newDirection = (direction + i) % 8;
            //     newX = x + dx[newDirection];
            //     newY = y + dy[newDirection];
            //     if (newX >= 0 && newX < rows && newY >= 0 && newY < cols && data[newX][newY] > 0)
            //     {
            //         return std::make_pair(newX, newY);
            //     }
            // }
            // return std::make_pair(-1, -1);
        }
        std::pair<int, int> getNextBoundaryTile2(int x, int y, std::pair<int, int> came_from)
        {
            // check N, NE, E, SE, S, SW, W, NW
            // The first direction that is a boundary tile is the next boundary tile
            int rows = data.size();
            int cols = data[0].size();
            int newX;
            int newY;
            for (int i = 0; i < sizeof(dx); i++)
            {
                int newDirection = i;
                newX = x + dx[newDirection];
                newY = y + dy[newDirection];
                if (newX == came_from.first && newY == came_from.second)
                {
                    continue;
                }
                if (newX >= 0 && newX < rows && newY >= 0 && newY < cols && data[newX][newY] > 0 && isBoundaryTile(newX, newY))
                {
                    return std::make_pair(newX, newY);
                }
            }
            return std::make_pair(-1, -1);
        }

        std::vector<std::vector<std::pair<float, float>>> create_outlines_centers()
        {
            auto start = std::chrono::high_resolution_clock::now();
            std::vector<std::vector<std::pair<float, float>>> outlines;
            std::unordered_set<std::pair<int, int>, game_engine::pair_hash> visited;
            int rows = data.size();
            int cols = data[0].size();
            // Iterate over all tiles
            for (int x = 0; x < CHUNK_SIZE; x++)
            {
                for (int y = 0; y < CHUNK_SIZE; y++)
                {
                    if (data[x][y] > 0)
                    {
                        std::pair<int, int> tile = std::make_pair(x, y);
                        // If the tile is solid and not visited, start outlining
                        if (!visited.count(tile))
                        {
                            if(chunk_x == 1) printf("Current start tile: (%d, %d)\n", x, y);
                            if (isBoundaryTile(x, y))
                            {
                                uint32_t current_x = x;
                                uint32_t current_y = y;
                                uint32_t previous_x = x;
                                uint32_t previous_y = y;
                                uint32_t direction = 0;
                                std::vector<std::pair<float, float>> outline;
                                printf("Current tile(init): (%d, %d)\n", current_x, current_y);
                                std::pair<int, int> next_tile = getNextBoundaryTile2(current_x, current_y, {current_x, current_y});
                                if(next_tile.first == -1) continue;
                                current_x = next_tile.first;
                                current_y = next_tile.second;
                                // printf("Next tile: (%d, %d)\n", current_x, current_y);

                                while (current_x != x || current_y != y)
                                {
                                    printf("Current tile: (%d, %d)\n", current_x, current_y);
                                    // outline.push_back(std::make_pair(current_y + chunk_y * game::CHUNK_SIZE + 0.5, current_x + 0.5 + chunk_x * game::CHUNK_SIZE));
                                    // std::pair<float, float> point = std::make_pair(current_y + 0.5 + chunk_x * game::CHUNK_SIZE, current_x + 0.5 + chunk_y * game::CHUNK_SIZE);
                                    // outline.push_back(point);
                                    // outline.push_back(std::make_pair(current_y + chunk_x * game::CHUNK_SIZE + 0.5, current_x + chunk_y * game::CHUNK_SIZE + 0.5));
                                    std::pair<float, float> p = std::make_pair(current_y + chunk_x * game::CHUNK_SIZE + 0.5, current_x + chunk_y * game::CHUNK_SIZE + 0.5);
                                    outline.push_back(p);

                                    // outline.push_back(std::make_pair(current_y + 0.5 + chunk_x * game::CHUNK_SIZE, current_x + chunk_y * game::CHUNK_SIZE + 0.5));

                                    // outline.push_back(std::make_pair(current_x + 0.5 + chunk_x * game::CHUNK_SIZE, current_y + chunk_y * game::CHUNK_SIZE + 0.5));
                                    visited.insert(std::make_pair(current_x, current_y));
                                    // next_tile = getNextBoundaryTile2(current_x, current_y,  {previous_x, previous_y});
                                    // Do this ^^^ inline
                                
                                
                                    int newX;
                                    int newY;
                                    for (int i = 0; i < 4; i++)
                                    {
                                        newX = current_x + dx[i];
                                        newY = current_y + dy[i];
                                        if (newX == previous_x && newY == previous_y)
                                        {
                                            continue;
                                        }
                                        // if (newX >= 0 && newX < rows && newY >= 0 && newY < cols && data[newX][newY] > 0)
                                        // {

                                        if (newX >= 0 && newX < rows && newY >= 0 && newY < cols && data[newX][newY] > 0 && isBoundaryTile(newX, newY))
                                        {
                                            next_tile = std::make_pair(newX, newY);
                                            if (visited.count(next_tile))
                                            {
                                                continue;
                                            }
                                            break;
                                        }
                                        // if ended
                                        // if(i == 3) {
                                        //     // didnt find a next tile
                                        //     // printf("No next tile found\n");
                                        //     // printf("Current tile: (%d, %d)\n", current_x, current_y);
                                        //     current_x = x;
                                        //     current_y = y;
                                        // }
                                    }

                                    previous_x = current_x;
                                    previous_y = current_y;
                                    current_x = next_tile.first;
                                    current_y = next_tile.second;
                                    // printf("Next tile(looping): (%d, %d)\n", current_x, current_y);
                                }
                                outlines.push_back(outline);
                            }
                        }
                    }
                }
            }
            for (int i = 0; i < outlines.size(); i++)
            {
                std::vector<std::pair<float, float>> outline = outlines[i];
                std::vector<std::pair<float, float>> new_outline;
                new_outline.push_back(outline[0]);
                for (int j = 1; j < outline.size(); j++)
                {
                    std::pair<float, float> p1 = outline[j - 1];
                    std::pair<float, float> p2 = outline[j];
                    std::pair<float, float> p3 = outline[(j + 1) % outline.size()];
                    if (p1.first != p2.first || p1.second != p2.second)
                    {
                        if (p2.first != p3.first || p2.second != p3.second)
                        {
                            new_outline.push_back(p2);
                        }
                    }
                }
                outlines[i] = new_outline;
            }

            int decimation = 4;
            // remove points to keep only one every decimation value
            for (int i = 0; i < outlines.size(); i++)
            {
                std::vector<std::pair<float, float>> outline = outlines[i];
                std::vector<std::pair<float, float>> new_outline;
                for (int j = 0; j < outline.size(); j++)
                {
                    if (j % decimation == 0)
                    {
                        new_outline.push_back(outline[j]);
                    }
                }
                // push first point to end to close the outline
                new_outline.push_back(new_outline[0]);
                outlines[i] = new_outline;
            }
            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            printf("Time taken by function: %d\n", duration.count());
            return outlines;
        }

        std::vector<std::vector<std::pair<int, int>>> create_outlines()
        {
            // get time in nano seconds
            auto start = std::chrono::high_resolution_clock::now();
            std::vector<std::vector<std::pair<int, int>>> outlines;
            std::unordered_set<std::pair<int, int>, game_engine::pair_hash> visited;

            // Iterate over all tiles
            for (int x = 0; x < CHUNK_SIZE; x++)
            {
                for (int y = 0; y < CHUNK_SIZE; y++)
                {
                    std::pair<int, int> tile = std::make_pair(x, y);
                    // If the tile is solid and not visited, start outlining
                    if (data[x][y] > 0 && !visited.count(tile))
                    {
                        // Check if the current tile is a boundary tile
                        if (isBoundaryTile(x, y))
                        {
                            // Start outlining from this tile
                            int start_x = x;
                            int start_y = y;
                            int direction = 0;

                            // Create a new outline
                            std::vector<std::pair<int, int>> outline;
                            // std::unordered_set<std::pair<int, int>, game_engine::pair_hash> outline_set;
                            outline.push_back({std::make_pair(start_x + chunk_x * CHUNK_SIZE, start_y + chunk_y * CHUNK_SIZE)});
                            // outlines.push_back({std::make_pair(start_x + chunk_x * CHUNK_SIZE, start_y + chunk_y * CHUNK_SIZE)});
                            // outlines.push_back(outline);
                            visited.insert(tile);

                            uint32_t count = 1;
                            // Keep outlining until we reach the starting tile again
                            while (true)
                            {
                                // printf("count: %d\n", count);
                                std::pair<int, int> next_tile = getNextBoundaryTile(start_x, start_y, direction);
                                // std::pair<int, int> next_tile;
                                // if(visited.count(next_tile))
                                // {
                                //     break;
                                // }

                                int next_x = next_tile.first;
                                int next_y = next_tile.second;
                                // If there is no next tile, stop outlining
                                // if (next_x == -1 && next_y == -1)
                                // {
                                //     break;
                                // }
                                // If we reached the starting tile again, stop outlining
                                if (next_x == x && next_y == y)
                                {
                                    break;
                                }
                                // printf("count: %d\n", count);
                                count++;
                                // Add the next tile to the outline
                                outline.push_back(std::make_pair(next_x + chunk_x * CHUNK_SIZE, next_y + chunk_y * CHUNK_SIZE));
                                visited.insert(next_tile);
                                // Update the current tile
                                start_x = next_x;
                                start_y = next_y;
                                direction = (direction + 7) % 8;
                            }
                            outlines.push_back(outline);
                        }
                    }
                }
            }
            printf("Number of 1 lines: %d\n", outlines.size());
            // decimate the meshes
            for (int i = 0; i < outlines.size(); i++)
            {
                std::vector<std::pair<int, int>> outline = outlines[i];
                std::vector<std::pair<int, int>> new_outline;
                new_outline.push_back(outline[0]);
                for (int j = 1; j < outline.size(); j++)
                {
                    std::pair<int, int> p1 = outline[j - 1];
                    std::pair<int, int> p2 = outline[j];
                    std::pair<int, int> p3 = outline[(j + 1) % outline.size()];
                    if (p1.first != p2.first || p1.second != p2.second)
                    {
                        if (p2.first != p3.first || p2.second != p3.second)
                        {
                            new_outline.push_back(p2);
                        }
                    }
                }
                outlines[i] = new_outline;
            }
            printf("Number of 2 lines: %d\n", outlines.size());

            int decimation = 2;
            // remove points to keep only one every decimation value
            for (int i = 0; i < outlines.size(); i++)
            {
                std::vector<std::pair<int, int>> outline = outlines[i];
                std::vector<std::pair<int, int>> new_outline;
                for (int j = 0; j < outline.size(); j++)
                {
                    if (j % decimation == 0)
                    {
                        new_outline.push_back(outline[j]);
                    }
                }
                outlines[i] = new_outline;
            }

            auto end = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
            printf("Time taken by function: %d\n", duration.count());

            // print the number of lines and the number of points in each line
            printf("Number of 3 lines: %d\n", outlines.size());
            // for (int i = 0; i < outlines.size(); i++)
            // {
            //     printf("Number of points in line %d: %d\n", i, outlines[i].size());
            // }
            printf("\n");

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

        std::vector<std::vector<std::pair<float, float>>> create_outlines(int x, int y)
        {
            return chunk_data[x + y * CHUNKS_WIDTH]->create_outlines_centers();
        }
    };
} // namespace game
