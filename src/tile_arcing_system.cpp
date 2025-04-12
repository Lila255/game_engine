#include "tile_arcing_system.hpp"
#include "world_tile_system.hpp"
// #include "game_core.hpp"

namespace game
{
	tile_arcing_system::~tile_arcing_system()
	{
	}

	void tile_arcing_system::update()
	{
	}

	std::pair<int, int> tile_arcing_system::get_box_coordinate(entity ent)
	{
		game_engine::box_lerp &box = box_sys->get(ent);
		return std::make_pair((int)((box.x + box.x2) / 2.0), (int)((box.y + box.y2) / 2.0));
	}

	void tile_arcing_system::start_thread()
	{
		uint16_t tick_ms = 10;
		uint64_t tick_count = 0;
		running = 1;
		while (running)
		{
			update(tick_count++);
			std::this_thread::sleep_for(std::chrono::milliseconds(tick_ms));
		}
	}

	void tile_arcing_system::update(uint64_t tick_count)
	{
		std::vector<entity> entities = tile_arcs.get_entities();
		for (auto &ent : entities)
		{
			tile_arc &arc = tile_arcs.get(ent);

			std::pair<int, int> start_pos	= arc.start_ent ? get_box_coordinate(arc.start_ent) : arc.start_pos;
			std::pair<int, int> end_pos		= arc.end_ent ? get_box_coordinate(arc.end_ent) : arc.end_pos;

			switch (arc.type)
			{
				case tile_arc_type::LINE:
				{
					int dx = end_pos.first - start_pos.first;
					int dy = end_pos.second - start_pos.second;




					// draw line from start to end
					float x = start_pos.first;
					float y = start_pos.second;

					int steps = sqrt(dx * dx + dy * dy);
					float x_change = dx / (float)steps;
					float y_change = dy / (float)steps;

					for (int i = 0; i < steps; i++)
					{
						x += x_change;
						y += y_change;
						world_tile_sys->set_tile_copy_at((int)x, (int)y, ELECTRIC_BLUE);
					}

					break;
				}

				case tile_arc_type::ELECTRIC:
				{
					// draw line from start to end
					float x = start_pos.first;
					float y = start_pos.second;

					int dx = end_pos.first - start_pos.first;
					int dy = end_pos.second - start_pos.second;

					int steps = sqrt(dx * dx + dy * dy);
					float x_change = dx / (float)steps;
					float y_change = dy / (float)steps;

					for (int i = 0; i < steps; i++)
					{
						x += x_change;
						x += perlin_noise_1.noise1D((i + tick_count * 5) / 10.0) * 2;
						y += y_change;
						y += perlin_noise_1.noise1D((i + tick_count * 5) / 10.0) * 2;
						world_tile_sys->set_tile_copy_at((int)x, (int)y, ELECTRIC_BLUE);
					}

					break;
				}
			}
		}
	}
}