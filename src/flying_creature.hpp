#pragma once

#include "engine_core.hpp"

namespace game
{
	enum flying_creature_type
	{
		DEAD_CREATURE,
		BEE,
		LOCUST,
		MOTH
	};

	enum flying_creature_state
	{
		IDLE,
		EATING,
		RETRIEVING
	};

	struct flying_creature
	{
	private:
		flying_creature_type type;
		flying_creature_state state = IDLE;
		uint16_t collected_mass = 0;
		
	public:
		flying_creature() = default;
		bool is_flying = true;
		std::pair<int, int> target_home;
		uint64_t last_eat_time = 0;
		
		flying_creature(flying_creature_type t) : type(t) {};

		flying_creature_type get_type() { return type; }
		void set_type(flying_creature_type t) { type = t; }
		flying_creature_state get_state() { return state; }
		void set_state(flying_creature_state s) { state = s; }
		uint16_t get_collected_mass() { return collected_mass; }
		void set_collected_mass(uint16_t mass) { collected_mass = mass; }
		void add_collected_mass(uint16_t mass) { collected_mass += mass; }
	};
}