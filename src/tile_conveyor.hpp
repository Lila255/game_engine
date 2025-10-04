#pragma once

#include "engine_core.hpp"

namespace game
{
	struct tile_conveyor
	{
		int start_x, start_y;
		int end_x, end_y;
		int speed;
		int moving;
		int teeth_spacing = 3;
		std::vector<std::pair<int, int>> path;	// path of the tiles that make up the conveyor core
		std::vector<std::pair<int, int>> tooth_path;	// path of the tiles the conveyor teeth move along around teh outside
		tile_conveyor() = default;
	};
}