#pragma once

#include "engine_core.hpp"
#include "box2d_system.hpp"

namespace game
{
	enum legged_creature_type
	{
		SPIDER,
		ANT,
		CENTIPEDE,
		AMORPHOUS_VOID_MONSTER,
	};

	enum legged_creature_state
	{
		WALKING,
		NOT_WALKING,
	};

	// used for keeping track of leg positions for rendering
	struct legged_creature_leg
	{
	public:
		uint16_t bones = 2;
		std::vector<entity> bone_entities;
		std::vector<std::pair<double, double>> end_points;
		std::vector<b2RevoluteJoint *> joints;
		entity foot_entity;
		
		legged_creature_leg() = default;
		// {
		// 	end_points = std::vector<std::pair<double, double>>(lengths);
		// }
		// legged_creature_leg(uint16_t l) : lengths(l) { end_points = std::vector<std::pair<double, double>>(lengths); }
		// void set_end_point(uint16_t i, double x, double y) { end_points[i] = std::pair<double, double>(x, y); }

	};

	const uint64_t spider_leg_lengths[] = {12, 12, 8};
	const uint64_t spider_leg_distance_to_start[] = {0, 12, 20};

	struct legged_creature
	{
	private:
		legged_creature_type type;
		legged_creature_state state = NOT_WALKING;
		uint16_t collected_mass = 0;
		
	public:
		legged_creature() = default;
		bool is_flying = true;
		std::pair<int, int> target_home;
		uint64_t last_eat_time = 0;
		std::vector<legged_creature_leg> legs;
		std::unordered_set<uint64_t> connected_legs;
		uint64_t leg_count = 2;
		
		legged_creature(legged_creature_type t) : type(t){};
		
		void set_params(uint64_t legs_n, uint64_t bones, double x, double y) {
			leg_count = legs_n;
			legs = std::vector<legged_creature_leg>(leg_count);
			for(int i = 0; i < leg_count; i++)
			{
				legs[i].bones = bones;
				legs[i].end_points = {};
				float ang = i * 2 * M_PI / leg_count + 1.1111109090990;

				for(int j = 0; j < bones; j++)
				{
					// legs[i].end_points.push_back(std::pair<double, double>(x + spider_leg_lengths[j] * cos(ang), y + spider_leg_lengths[j] * sin(ang))); 
					legs[i].end_points.push_back(std::pair<double, double>(x, y + spider_leg_distance_to_start[j]));
				}
				legs[i].end_points.push_back(std::pair<double, double>(x, y + (spider_leg_distance_to_start[bones - 1] + spider_leg_lengths[bones - 1])));
			}
		};

		legged_creature_type get_type() { return type; }
		void set_type(legged_creature_type t) { type = t; }
		legged_creature_state get_state() { return state; }
		void set_state(legged_creature_state s) { state = s; }
		uint16_t get_collected_mass() { return collected_mass; }
		void set_collected_mass(uint16_t mass) { collected_mass = mass; }
		void add_collected_mass(uint16_t mass) { collected_mass += mass; }
	};
}