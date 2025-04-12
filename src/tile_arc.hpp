#pragma once

#include "engine_core.hpp"
// #include "world_tile_system.hpp"

namespace game
{
	enum tile_arc_type
	{
		LINE,
		ELECTRIC,
		MAGIC
	};

	struct tile_arc
	{
		tile_arc_type type;
		entity start_ent;
		entity end_ent;
		std::pair<int, int> start_pos;
		std::pair<int, int> end_pos;
		tile_arc() = default;
		tile_arc(tile_arc_type t, entity s, entity e) : type(t), start_ent(s), end_ent(e) {}
		tile_arc(tile_arc_type t, std::pair<int, int> s, std::pair<int, int> e) : type(t), start_pos(s), end_pos(e) {}
	};
}