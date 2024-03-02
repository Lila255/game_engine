#pragma once
#include <chrono>
#include <unordered_set>

#include <poly2tri/poly2tri.h>

#include "util.hpp"

#define entity uint32_t


namespace game
{
	const siv::BasicPerlinNoise<double>::seed_type perlin_noise_seed = 1;
	static siv::PerlinNoise perlin_noise_1(perlin_noise_seed);
	static siv::PerlinNoise perlin_noise_2(perlin_noise_seed + 1);
	static siv::PerlinNoise perlin_noise_3(perlin_noise_seed + 2);

	const uint16_t CHUNK_SIZE = 100; // There are CHUNK_SIZE*CHUNK_SIZE tiles in chunk
	
	// enum for tile types
	enum tile_type
	{
		// gas tiles
		AIR,	// 0
		SMOKE,	// 1
		STEAM,	// 2
		TEMPORARY_SMOKE,	// 3
		GAS_04,	// 4
		GAS_05,	// 5
		GAS_06,	// 6
		GAS_07,	// 7
		GAS_08,	// 8
		GAS_09,	// 9
		GAS_10,	// 10
		GAS_11,	// 11
		GAS_12,	// 12
		GAS_13,	// 13
		GAS_14,	// 14
		GAS_15,	// 15
		GAS_16,	// 16
		GAS_17,	// 17
		GAS_18,	// 18
		GAS_19,	// 19
		GAS_20,	// 20
		GAS_21,	// 21
		GAS_22,	// 22
		GAS_23,	// 23
		GAS_24,	// 24
		GAS_25,	// 25
		GAS_26,	// 26
		GAS_27,	// 27
		GAS_28,	// 28
		GAS_29,	// 29
		GAS_30,	// 30
		GAS_31,	// 31
		// fluid tiles
		WATER,	// 32
		LAVA,	// 33
		ACID,	// 34
		FLUID_03,	// 35
		FLUID_04,	// 36
		FLUID_05,	// 37
		FLUID_06,	// 38
		FLUID_07,	// 39
		FLUID_08,	// 40
		FLUID_09,	// 41
		FLUID_10,	// 42
		FLUID_11,	// 43
		FLUID_12,	// 44
		FLUID_13,	// 45
		FLUID_14,	// 46
		FLUID_15,	// 47
		FLUID_16,	// 48
		FLUID_17,	// 49
		FLUID_18,	// 50
		FLUID_19,	// 51
		FLUID_20,	// 52
		FLUID_21,	// 53
		FLUID_22,	// 54
		FLUID_23,	// 55
		FLUID_24,	// 56
		FLUID_25,	// 57
		FLUID_26,	// 58
		FLUID_27,	// 59
		FLUID_28,	// 60
		FLUID_29,	// 61
		FLUID_30,	// 62
		FLUID_31,	// 63
		// solid tiles
		GLASS,	// 64
		LEAF,	// 65
		STONE,	// 66
		DIRT,	// 67
		SAND,	// 68
		GRASS,	// 69
		WOOD,	// 70
		GOLD,	// 71
		TREE_SEED,	// 72
		ROOT,	// 73
		EMBER,	// 74
		SNOW,	// 75
		SOLID_12,	// 76
		SOLID_13,	// 77
		SOLID_14,	// 78
		SOLID_15,	// 79
		SOLID_16,	// 80
		SOLID_17,	// 81
		SOLID_18,	// 82
		SOLID_19,	// 83
		SOLID_20,	// 84
		SOLID_21,	// 85
		SOLID_22,	// 86
		SOLID_23,	// 87
		SOLID_24,	// 88
		SOLID_25,	// 89
		SOLID_26,	// 90
		SOLID_27,	// 91
		SOLID_28,	// 92
		SOLID_29,	// 93
		SOLID_30,	// 94
		SOLID_31,	// 95
		SOLID_32,	// 96
		SOLID_33,	// 97
		SOLID_34,	// 98
		SOLID_35,	// 99
		SOLID_36,	// 100
		SOLID_37,	// 101
		SOLID_38,	// 102
		SOLID_39,	// 103
		SOLID_40,	// 104
		SOLID_41,	// 105
		SOLID_42,	// 106
		SOLID_43,	// 107
		SOLID_44,	// 108
		SOLID_45,	// 109
		SOLID_46,	// 110
		SOLID_47,	// 111
		SOLID_48,	// 112
		SOLID_49,	// 113
		SOLID_50,	// 114
		SOLID_51,	// 115
		SOLID_52,	// 116
		SOLID_53,	// 117
		SOLID_54,	// 118
		SOLID_55,	// 119
		SOLID_56,	// 120
		SOLID_57,	// 121
		SOLID_58,	// 122
		SOLID_59,	// 123
		SOLID_60,	// 124
		SOLID_61,	// 125
		SOLID_62,	// 126
		SOLID_63,	// 127
		// background tiles
		BRICK_1,	// 128
		BRICK_2,	// 129
		BRICK_3,	// 130
		BRICK_4,	// 131
		MORTAR,	// 132

		// indestructible tiles
		BEDROCK,

	};
	const uint16_t SOLID_TILE_START_INDEX = tile_type::GLASS;
	extern std::array<uint8_t, 256> is_solid_tile;



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