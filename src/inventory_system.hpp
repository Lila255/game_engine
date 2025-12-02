#pragma once

#include "engine_comp.hpp"
#include "building_component_system.hpp"
#include "world_tile_system.hpp"

namespace game
{

	struct inventory_item
	{
		uint32_t quantity;
		tile_type tile_item_type = VACCUUM;
		building_component building_comp;

		inventory_item() = default;
		inventory_item(uint16_t q, tile_type t) : quantity(q), tile_item_type(t) {}
		inventory_item(uint16_t q, building_component bc) : quantity(q), building_comp(bc) {}
	};

	struct inventory
	{
		uint32_t item_width = 0;
		uint32_t item_height = 0;

		std::vector<std::vector<inventory_item>> items;



		inventory() = default;
		inventory(uint32_t w, uint32_t h) : item_width(w), item_height(h)
		{
			items.resize(item_height);
			for (uint32_t i = 0; i < item_height; i++)
			{
				items[i].resize(item_width);
			}
		}

		void add_item(uint32_t x, uint32_t y, inventory_item item)
		{
			if (x >= item_width || y >= item_height)
				return;
			items[y][x] = item;
		}
	};



	struct inventory_system : public game_engine::system
	{
	private:
		game_engine::sparse_component_set<inventory> inventories;


	public:
		inventory_system() = default;
		void update() override {}

		void add_inventory(entity ent, inventory inv)
		{
			inventories.add(ent, inv);
		}

		void remove_inventory(entity ent)
		{
			inventories.remove(ent);
		}


	};
}