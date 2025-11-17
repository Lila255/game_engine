#include "util.hpp"

namespace game_engine
{
	std::mutex id_mutex;
	static uint32_t current_max = 0;
	static std::priority_queue<uint32_t> free_ids;

	uint32_t id_generator::generate() noexcept
	{
		if (free_ids.empty())
		{
			return current_max++;
		}
		else
		{
			std::lock_guard<std::mutex> lock(id_mutex);
			uint32_t id = free_ids.top();
			free_ids.pop();
			return id;
		}
	}

	void id_generator::free_id(uint32_t id) noexcept
	{
		std::lock_guard<std::mutex> lock(id_mutex);
		free_ids.push(id);
	}
}