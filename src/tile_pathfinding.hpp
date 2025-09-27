#pragma once
#include <vector>
#include "chunk.hpp"
#include "tile_pathfinding.hpp"

namespace game
{
	struct tile_pathfinding : public game_engine::component
	{
		int start_x, start_y;
		entity start_ent;
		int end_x, end_y;
		entity end_ent;
		int directions = 8;
		int clearance = 4;	// tile distance that must be clear of solid tiles
		int can_fly = 0;
		int step_distance = 8;
		std::vector<std::pair<int, int>> path;
		tile_pathfinding() : start_x(0), start_y(0), start_ent(0), end_x(0), end_y(0), end_ent(0) {}
		tile_pathfinding(int sx, int sy, int ex, int ey, int dir = 8, int clear = 4, int step = 8, int fly = 0) : start_x(sx), start_y(sy), start_ent(0), end_x(ex), end_y(ey), end_ent(0), directions(dir), clearance(clear), step_distance(step), can_fly(fly) {}
		tile_pathfinding(entity s_ent, entity e_ent, int dir = 8, int clear = 4, int step = 8, int fly = 0) : start_x(0), start_y(0), start_ent(s_ent), end_x(0), end_y(0), end_ent(e_ent), directions(dir), clearance(clear), step_distance(step), can_fly(fly) {}
	};
}
