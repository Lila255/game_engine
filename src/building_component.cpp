#include "building_component.hpp"
namespace game
{
	std::array<std::unordered_set<std::pair<uint8_t, uint8_t>, pair_hash>, (int)building_component_type::custom> building_component_shapes = {
		{
			// none
			{},
			// small_l_shape
			{
				std::make_pair(0, 0),
				std::make_pair(1, 0),
				std::make_pair(2, 0),
				std::make_pair(2, 1)
			},
			// large_l_shape
			{
				std::make_pair(0, 0),
				std::make_pair(1, 0),
				std::make_pair(2, 0),
				std::make_pair(3, 0),
				std::make_pair(4, 0),
				std::make_pair(5, 0),
				std::make_pair(6, 0),
				std::make_pair(0, 1),
				std::make_pair(1, 1),
				std::make_pair(2, 1),
				std::make_pair(3, 1),
				std::make_pair(4, 1),
				std::make_pair(5, 1),
				std::make_pair(6, 1),
				std::make_pair(0, 2),
				std::make_pair(1, 2),
				std::make_pair(0, 3),
				std::make_pair(1, 3),
			}
		}
		
	};
	// 	// none
	// 	{std::unordered_set<std::pair<uint8_t, uint8_t>, pair_hash>{}},
	// 	// small_l_shape
	// 	{std::unordered_set<std::pair<uint8_t, uint8_t>, pair_hash>{
	// 		std::make_pair(0, 0),
	// 		std::make_pair(1, 0),
	// 		std::make_pair(2, 0),
	// 		std::make_pair(2, 1)
	// 	}}
	// }
	// };


}