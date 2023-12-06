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
	const double perlin_noise_seed = 31.0;
	static siv::PerlinNoise perlin_noise_1(perlin_noise_seed);
	static siv::PerlinNoise perlin_noise_2(perlin_noise_seed + 1);
	static siv::PerlinNoise perlin_noise_3(perlin_noise_seed + 2);

	const uint16_t CHUNK_SIZE = 100; // There are CHUNK_SIZE*CHUNK_SIZE tiles in chunk
	
	const uint16_t SOLID_TILE_START_INDEX = 3;
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
		GOLD,	// 10

		BRICK_1,	// background tiles
		BRICK_2,
		MORTAR,

		// indestructible tiles
		BEDROCK,

	};

	struct tile_linef
	{
		float x1, y1, x2, y2;
	};
	struct linef_hash
	{
		std::size_t operator()(const tile_linef &l) const
		{
			return std::hash<float>()(l.x1) ^ std::hash<float>()(l.y1) ^ std::hash<float>()(l.x2) ^ std::hash<float>()(l.y2);
		}
	};
	struct tile_line
	{
		int x1, y1, x2, y2;
		tile_line(int x1, int y1, int x2, int y2) : x1(x1), y1(y1), x2(x2), y2(y2) {}

		// define != operator
		bool operator!=(const tile_line &other) const
		{
			// return x1 != other.x1 || y1 != other.y1 || x2 != other.x2 || y2 != other.y2;
			return (x1 + y1 * CHUNK_SIZE) != (other.x1 + other.y1 * CHUNK_SIZE) || (x2 + y2 * CHUNK_SIZE) != (other.x2 + other.y2 * CHUNK_SIZE);
		}
		// define == operator
		bool operator==(const tile_line &other) const
		{
			// return x1 == other.x1 && y1 == other.y1 && x2 == other.x2 && y2 == other.y2;
			// return (x1 == other.x1 && y1 == other.y1 && x2 == other.x2 && y2 == other.y2) || (x1 == other.x2 && y1 == other.y2 && x2 == other.x1 && y2 == other.y1);
			// return (x1 + y1 * CHUNK_SIZE) == (other.x1 + other.y1 * CHUNK_SIZE) && (x2 + y2 * CHUNK_SIZE) == (other.x2 + other.y2 * CHUNK_SIZE);
			if(x1 == other.x1 && y1 == other.y1 && x2 == other.x2 && y2 == other.y2) {
				return true;
			} else if(x1 == other.x2 && y1 == other.y2 && x2 == other.x1 && y2 == other.y1) {
				return true;
			} else {
				return false;
			}
		}
	};

	struct tile_line_hash
	{
		std::size_t operator()(const tile_line &l) const
		{
			// return std::hash<int>()(l.x1) ^ std::hash<int>()(l.y1) ^ std::hash<int>()(l.x2) ^ std::hash<int>()(l.y2);
			return (l.x1 * 1 + l.y1 * CHUNK_SIZE) ^ (l.x2 * 1 + l.y2 * CHUNK_SIZE);
		}
	};

	struct pair_hash {
		template <class T1, class T2>
		std::size_t operator() (const std::pair<T1, T2> &p) const {
			return std::hash<T1>()(p.first) ^ std::hash<T2>()(p.second * CHUNK_SIZE);
		}
	};
	struct chunk_coord_pair_hash {
		template <class T1, class T2>
		std::size_t operator() (const std::pair<T1, T2> &p) const {
			return std::hash<T1>()(p.first) + std::hash<T2>()(p.second) * CHUNK_SIZE;
		}
	};
	

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
		uint8_t get_tile(int x, int y);

		bool isBoundaryTile(int x, int y);

		std::pair<int, int> getNextBoundaryTile2(int x, int y, std::pair<int, int> came_from);

		// float square_distance_between_points(p2t::Point *p1, p2t::Point *p2)
		// {
		//     float x = p1->x - p2->x;
		//     float y = p1->y - p2->y;
		//     return x * x + y * y;
		// }
		
		void create_outlines(std::vector<std::vector<std::pair<float, float>>> * chunk_outline);
		bool is_outline(tile_line l);
		bool is_edge(int, int);
		std::vector<std::pair<int, int>> trace_outline(tile_line start_line, std::unordered_set<tile_line, tile_line_hash> &visited_lines);
		std::vector<std::vector<std::pair<float, float>>> create_outlines_centers();


		bool delete_circle(int x, int y, int radius);
	private:
		uint16_t get_tile_edginess(int x, int y);
		

	};
}