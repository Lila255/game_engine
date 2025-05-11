#pragma once

#include "engine_core.hpp"
#include "chunk.hpp"
// #include "world_tile_system.hpp"

namespace game
{
	enum building_component_type
	{
		none = 0,
		small_l_shape,
		large_l_shape,
		custom
	};

	extern std::array<std::unordered_set<std::pair<uint8_t, uint8_t>, pair_hash>, building_component_type::custom> building_component_shapes;



	struct building_component
	{
		building_component_type type;
		tile_type primary_tile_type;

		building_component() = default;
		building_component(building_component_type t, tile_type p) : type(t), primary_tile_type(p) {}
		std::unordered_set<std::pair<uint8_t, uint8_t>, pair_hash> custom_shape;
	};
}