#pragma once
#include <chrono>
#include <unordered_set>
#include <shared_mutex>
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
	const uint8_t CHUNK_FRAMES = 16; // Number of frames the current chunk has
	
	const uint16_t NUM_CHUNKS = 16; 
	const uint16_t CHUNKS_WIDTH = 4;
	
	// enum for tile types
	enum tile_type
	{
		// gas tiles
		VACCUUM, // 0
		POLLUTION,	// 1
		AIR,	// 2
		SMOKE,	// 3
		STEAM,	// 4
		TEMPORARY_SMOKE,	// 5
		GAS_05,	// 6
		GAS_06,	// 7
		GAS_07,	// 8
		GAS_08,	// 9
		GAS_09,	// 10
		GAS_10,	// 11
		GAS_11,	// 12
		GAS_12,	// 13
		GAS_13,	// 14
		GAS_14,	// 15
		GAS_15,	// 16
		GAS_16,	// 17
		GAS_17,	// 18
		GAS_18,	// 19
		GAS_19,	// 20
		GAS_20,	// 21
		GAS_21,	// 22
		GAS_22,	// 23
		GAS_23,	// 24
		GAS_24,	// 25
		GAS_25,	// 26
		GAS_26,	// 27
		GAS_27,	// 28
		GAS_28,	// 29
		GAS_29,	// 30
		GAS_30,	// 31
		// fluid tiles
		WATER,	// 32
		LAVA,	// 33
		ACID,	// 34
		HONEY,	// 35
		LIQUID_GLASS,	// 36
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
		WAX,	// 76
		TEMPORARY_SNOW,	// 75
		ICE,	// 78
		ASH,	// 79
		CONVEYOR_BELT,	// 80
		CONVEYOR_TOOTH,	// 81
		INSULATION,	// 82
		INSULATION_FOAM,	// 83
		CHARCOAL,	// 84
		LIGHT_EMBER,	// 85
		IRON,	// 86
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
		MORTAR,		// 132
		TRANS_BLUE,	// 133
		TRANS_PINK,	// 134
		WHITE,		// 135
		BEE_YELLOW,	// 136
		BEE_BLACK,	// 137
		GREY,		// 138
		DARK_GREY,	// 139
		ELECTRIC_BLUE,	// 140

		
		// indestructible tiles
		BEDROCK	// 255

	};
	const uint8_t BACKGROUND_TILE_START_INDEX = BRICK_1;
	const uint8_t SOLID_TILE_START_INDEX = GLASS;
	const uint8_t LIQUID_TILE_START_INDEX = WATER;
	extern std::array<uint8_t, 256> is_solid_tile;
	extern std::array<uint8_t, 256> is_tile_fixed;

	
	enum tile_simple_type
	{
		GAS,
		LIQUID,
		SOLID,
		BACKGROUND_TILE
	};
	
	// tile temperature config
	extern std::array<float, 256> tile_max_temperature;
	extern std::unordered_map<tile_type, tile_type> max_temp_tile_change;
	extern std::array<float, 256> tile_min_temperature;
	extern std::unordered_map<tile_type, tile_type> min_temp_tile_change;
	extern float absolute_max_temperature;
	extern std::array<float, 256> tile_heat_capacity;
	extern std::array<float, 256> tile_thermal_conductivity;
	extern std::array<int8_t, 256> tile_phase_change_count_multiplier;
	extern std::array<float, 256> tile_debris_fall_speed_multiplier;

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
		struct line_mapping_pair
	{
		std::pair<float, float> p1{-1.f, -1.f};
		std::pair<float, float> p2{-1.f, -1.f};

		line_mapping_pair() {}
		line_mapping_pair(std::pair<float, float> p)
		{
			p1 = p;
		}

		void insert(std::pair<float, float> p)
		{
			if (p1.first == -1)
			{
				p1 = p;
			}
			else if (p2.first == -1)
			{
				p2 = p;
			}
			else
			{
				printf("Error: line_mapping_pair already has two points\n");
			}
		}
		std::pair<float, float> get_next()
		{
			if (p2.first != -1)
			{
				std::pair<float, float> p = p2;
				p2 = {-1.f, -1.f};
				return p;
			}
			else if (p1.first != -1)
			{
				std::pair<float, float> p = p1;
				p1 = {-1.f, -1.f};
				return p;
			}
			else
			{
				printf("Error: line_mapping_pair has no points\n");
				return {-1.f, -1.f};
			}
		}
		void remove_point(std::pair<float, float> p)
		{
			if (p1 == p)
			{
				if (p2.first != -1)
				{
					p1 = p2;
					p2 = {-1.f, -1.f};
				}
				else
					p1 = {-1.f, -1.f};
			}
			else if (p2 == p)
			{
				p2 = {-1.f, -1.f};
			}
			else
			{
				printf("Error: line_mapping_pair does not contain point\n");
			}
		}
	};

	tile_simple_type get_simple_tile_type(uint8_t tile);

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

	struct chunk_neighbour_tile_buffer
	{
		std::array<uint8_t, CHUNK_SIZE> top_tiles;
		std::array<uint8_t, CHUNK_SIZE> bottom_tiles;
		std::array<uint8_t, CHUNK_SIZE> left_tiles;
		std::array<uint8_t, CHUNK_SIZE> right_tiles;

		std::array<float, CHUNK_SIZE> top_temps;
		std::array<float, CHUNK_SIZE> bottom_temps;
		std::array<float, CHUNK_SIZE> left_temps;
		std::array<float, CHUNK_SIZE> right_temps;

		std::array<uint16_t, CHUNK_SIZE> top_misc;
		std::array<uint16_t, CHUNK_SIZE> bottom_misc;
		std::array<uint16_t, CHUNK_SIZE> left_misc;
		std::array<uint16_t, CHUNK_SIZE> right_misc;
	};
	

	struct chunk
	{
	private:
		uint16_t get_tile_edginess(int x, int y);
		std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE> data;
		std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE> data_copy;
		std::array<std::array<float, CHUNK_SIZE>, CHUNK_SIZE> temperature_data;
		std::array<std::array<uint16_t, CHUNK_SIZE>, CHUNK_SIZE> misc_data;
		std::shared_mutex chunk_mutex;
		std::shared_mutex chunk_mutex_copy;
		chunk_neighbour_tile_buffer neighbour_tile_buffer;

	public:
		uint16_t chunk_x;
		uint16_t chunk_y;
		entity *background_entity;
		entity *foreground_entity;
		entity *light_entity;
		uint8_t current_frame = 0;
		// int dx[8] = {0, 1, 1, 1, 0, -1, -1, -1};
		// int dy[8] = {-1, -1, 0, 1, 1, 1, 0, -1};
		const int8_t dx[4] = {0, 1, 0, -1};
		const int8_t dy[4] = {-1, 0, 1, 0};
        chunk(const chunk&) = delete;
        chunk& operator=(const chunk&) = delete;
        chunk(chunk&&) = delete;
        chunk& operator=(chunk&&) = delete;

		chunk() = default;
		chunk(uint16_t x, uint16_t y) : chunk_x(x), chunk_y(y)
		{
			data = std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE>{};
			data_copy = std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE>{};
			temperature_data = std::array<std::array<float, CHUNK_SIZE>, CHUNK_SIZE>{};
			misc_data = std::array<std::array<uint16_t, CHUNK_SIZE>, CHUNK_SIZE>{};
			chunk_mutex.lock();
			chunk_mutex.unlock();
		}

		void create_chunk(uint32_t x, uint32_t y);

		void to_string();

		// void create_texture_from_chunk(GLuint &texture);
		std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE>* get_data();
		std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE>* get_data_copy();
		std::array<std::array<float, CHUNK_SIZE>, CHUNK_SIZE>* get_temperature_data();
		std::array<std::array<uint16_t, CHUNK_SIZE>, CHUNK_SIZE>* get_misc_data();

		void copy_to_data_copy();

		void set_tile(int x, int y, uint8_t value);
		void set_tile_copy(int x, int y, uint8_t value);
		uint8_t get_tile(int x, int y);
		uint8_t get_tile_using_buffer(int x, int y);
		uint8_t get_tile_copy(int x, int y);
		float get_tile_temperature(int x, int y);
		float get_tile_temperature_using_buffer(int x, int y);
		void set_tile_temperature(int x, int y, float temperature);
		void add_tile_temperature(int x, int y, float temperature);
		uint16_t get_misc_data_at(int x, int y);
		void set_misc_data_at(int x, int y, uint16_t value);
		void add_misc_data_at(int x, int y, int16_t value);

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


		uint32_t delete_circle(int x, int y, int radius, std::unordered_set<uint8_t> tile_deny_list);
		uint32_t explode_circle(int x, int y, int radius, float max_temperature, std::unordered_set<uint8_t> tile_deny_list);
		bool find_tile_in_rect(std::pair<int, int> &result, int x, int y, int w, int h, std::unordered_set<uint8_t> tile_types);
		void update_frame(uint8_t frame);

		bool try_place_tile_with_displacement_no_lock(int x, int y, tile_type tile_type, float temperature, uint16_t misc_data, int recursion_depth, int search_size);
		bool try_consume_nearby_tile_no_lock(int x, int y, tile_type tile_type, int search_size);
		void switch_tiles_no_lock(int x1, int y1, int x2, int y2);

		chunk_neighbour_tile_buffer * get_neighbour_tile_buffer_pointer();
		void get_neighbour_tile_buffer(chunk_neighbour_tile_buffer *buffer, uint8_t side);
		void update_neighbour_tiles(chunk_neighbour_tile_buffer *buffer, chunk_neighbour_tile_buffer *buffer_original, uint8_t side);

		void lock_chunk()
		{
			chunk_mutex.lock();
		}
		void unlock_chunk()
		{
			chunk_mutex.unlock();
		}
		void lock_chunk_copy()
		{
			chunk_mutex_copy.lock();
		}
		void unlock_chunk_copy()
		{
			chunk_mutex_copy.unlock();
		}
		// get shared lock
		void lock_chunk_shared()
		{
			chunk_mutex.lock_shared();
		}
		void unlock_chunk_shared()
		{
			chunk_mutex.unlock_shared();
		}
		void lock_chunk_copy_shared()
		{
			chunk_mutex_copy.lock_shared();
		}
		void unlock_chunk_copy_shared()
		{
			chunk_mutex_copy.unlock_shared();
		}

	};
}