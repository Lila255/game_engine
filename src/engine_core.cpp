#include "engine_core.hpp"

namespace game_engine
{
	engine *game_engine_pointer;
	task_scheduler* task_scheduler_pointer;

    std::vector<GLuint> shader_programs;
    const uint16_t window_width = 2000;
    const uint16_t window_height = 900;

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