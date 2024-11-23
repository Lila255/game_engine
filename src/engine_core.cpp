#include "engine_core.hpp"

namespace game_engine
{
	engine *game_engine_pointer;
	task_scheduler* task_scheduler_pointer;
	uint64_t task_scheduler::task_counter = 0;
	std::unordered_map<uint64_t, uint64_t> task_scheduler::task_count_by_type;
	std::unordered_map<uint64_t, std::string> task_scheduler::task_names;

    std::vector<GLuint> shader_programs;
    const uint16_t window_width = 1920;
    const uint16_t window_height = 1080;

    float projection_matrix[16] = {
		8.0f / window_width, 0.0f, 0.0f, 0.0f,
		0.0f, 8.0f / window_height, 0.0f, 0.0f,
		0.0f, 0.0f, -1.0f, 0.0f,
		-8.0f, -8.0f, 0.0f, 1.0f};

    float view_matrix[16]{
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f};
} 