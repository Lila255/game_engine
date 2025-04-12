#ifndef TILE_ARCING_SYSTEM_HPP
#define TILE_ARCING_SYSTEM_HPP

#include "world_tile_system.hpp"
#include "chunk.hpp"
#include "tile_arc.hpp"

#include "PerlinNoise.hpp"
#include <vector>

namespace game
{
	struct tile_arcing_system : public game_engine::system
	{
	private:
		game_engine::sparse_component_set<tile_arc> tile_arcs;
		world_tile_system *world_tile_sys;
		game_engine::box_system *box_sys;
		siv::PerlinNoise perlin_noise_1;

		std::pair<int, int> get_box_coordinate(entity);
		
	public:
		bool running = 0;
		tile_arcing_system(world_tile_system *world_system, game_engine::box_system *box_system) : world_tile_sys(world_system), box_sys(box_system) {
			perlin_noise_1 = siv::PerlinNoise(10.0);
		}
		~tile_arcing_system();

		void update();
		void update(uint64_t tick_count);

		void add(entity ent, tile_arc &arc)
		{
			tile_arcs.add(ent, arc);
		}

		void start_thread();
	};
}

#endif // TILE_ARCING_SYSTEM_HPP