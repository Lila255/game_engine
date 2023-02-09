#pragma once
#include <chrono>
#include <unordered_set>

#include <poly2tri/poly2tri.h>

#include "util.hpp"

// #include "game_core.hpp"
// #include "engine_comp.hpp"
#define entity uint32_t


namespace game
{

    const uint16_t CHUNK_SIZE = 128; // There are CHUNK_SIZE*CHUNK_SIZE tiles in chunk


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

        void create_chunk();

        void to_string();

        // void create_texture_from_chunk(GLuint &texture);
        std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE>* get_data();

        void set_tile(int x, int y, uint8_t value);

        bool isBoundaryTile(int x, int y);

        std::pair<int, int> getNextBoundaryTile2(int x, int y, std::pair<int, int> came_from);

        // float square_distance_between_points(p2t::Point *p1, p2t::Point *p2)
        // {
        //     float x = p1->x - p2->x;
        //     float y = p1->y - p2->y;
        //     return x * x + y * y;
        // }

        std::vector<std::vector<std::pair<float, float>>> create_outlines_centers();
    };
}