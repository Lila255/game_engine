#include "building_component_system.hpp"

namespace game
{



	void building_component_system::update()
	{
		
	}

	void building_component_system::update(uint64_t tick_count)
	{
		
	}

	void building_component_system::add(entity ent, building_component &building_comp)
	{
		building_components.add(ent, building_comp);
		
		// create boxy2d body using the building component's shape

		// create/get texture
		// create/get vbo
	}

	void building_component_system::start_thread()
	{
		
	}
	
	void building_component_system::place_building_component(entity ent)
	{
		auto &building_comp = building_components.get(ent);
		auto &box = box_sys->get(ent);
		
		std::unordered_set<std::pair<uint8_t, uint8_t>, pair_hash> shape = building_component_shapes[(int)building_comp.type];



		for(auto &p : shape)
		{
			int x = p.first + box.x;
			int y = p.second + box.y;


			world_tile_sys->set_tile_at_with_lock(box.x, box.y, building_comp.primary_tile_type);
		}
	}
}
