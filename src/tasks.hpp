#pragma once

#include <stdio.h>
#include <string>

#define entity uint32_t

namespace game
{
	struct create_debris_params
	{
		float x, y, v, r;
		create_debris_params(float x_pos, float y_pos, float vel, float radius) : x(x_pos), y(y_pos), v(vel), r(radius){}
	};
	void create_debris_task(void *parameters);

	void shutdown_task_schedular_task(void * parameters);
}