#include "chunk.hpp"
// #include "engine_comp.hpp"

namespace game
{

	void chunk::create_chunk()
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int x = 0; x < CHUNK_SIZE; x++)
			{
				double n_x = (x + chunk_x * game::CHUNK_SIZE) / 35.0;
				double n_y = (y + chunk_y * game::CHUNK_SIZE) / 35.0;
				double noise_1 = perlin_noise_1.noise2D_01(n_x, n_y);
				double noise_2 = perlin_noise_2.noise2D_01(n_x, n_y);
				double noise_3 = perlin_noise_3.noise2D_01(n_x, n_y);

				if (noise_1 > 0.525)
				{ // solid
					if (noise_2 > 0.5)
					{
						data[y][x] = game::STONE;
					}
					else
					{ 
						if(noise_3 > 0.5) {
							data[y][x] = game::SAND;
						}
						else {
							data[y][x] = game::DIRT;
						}
					}
				}
				else
				{
					data[y][x] = game::AIR;
				}

				// data[y][x] = noise_1 > 0.6 ? 1 : 0;

				// if((x / 16) % 2 == 0 && (y / 16) % 2 == 0)
				// 	data[y][x] = 1;
				// else
				// 	data[y][x] = 0;
			}
		}
		
		// spawn some grass
		for (int y = 1; y < CHUNK_SIZE; y++)
		{
			for (int x = 0; x < CHUNK_SIZE; x++)
			{
				if(data[y][x] == game::DIRT && data[y - 1][x] == game::AIR)
				{
					if(rand() % 100 < 10)
						data[y][x] = game::GRASS;
				}
			}
		}
	}

	void chunk::to_string()
	{
		for (int x = 0; x < CHUNK_SIZE; x++)
		{
			for (int y = 0; y < CHUNK_SIZE; y++)
			{
				printf("%d ", data[y][x]);
			}
			printf("\n");
		}
		printf("\n");
	}

	// void chunk::create_texture_from_chunk(GLuint &texture)
	// {
	//     glGenTextures(1, &texture);
	//     glBindTexture(GL_TEXTURE_2D, texture);
	//     // set data and size
	//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//     // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//     // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, CHUNK_SIZE, CHUNK_SIZE, 0, GL_RED, GL_UNSIGNED_BYTE, data.data());
	//     // set texture parameters
	//     // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	//     // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	//     // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	//     // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//     glBindTexture(GL_TEXTURE_2D, 0);
	// }

	std::array<std::array<uint8_t, CHUNK_SIZE>, CHUNK_SIZE> *chunk::get_data()
	{
		return &data;
	}

	void chunk::set_tile(int x, int y, uint8_t value)
	{
		if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE)
			data[y][x] = value;
	}

	uint8_t chunk::get_tile(int x, int y)
	{
		if (x >= 0 && x < CHUNK_SIZE && y >= 0 && y < CHUNK_SIZE)
			return data[y][x];
		else
			return 0;
	}
	

	bool chunk::isBoundaryTile(int x, int y)
	{
		int rows = data.size();
		int cols = data[0].size();
		for (int i = -1; i <= 1; i++)
		{
			for (int j = -1; j <= 1; j++)
			{
				int newX = x + i;
				int newY = y + j;
				if (newX < 0 || newX >= cols || newY < 0 || newY >= rows || data[newY][newX] == 0)
				{
					return true;
				}
			}
		}
		return false;
	}

	std::pair<int, int> chunk::getNextBoundaryTile2(int x, int y, std::pair<int, int> came_from)
	{
		// check N, NE, E, SE, S, SW, W, NW
		// The first direction that is a boundary tile is the next boundary tile
		int rows = data.size();
		int cols = data[0].size();
		int newX;
		int newY;
		for (int i = 0; i < sizeof(dx); i++)
		{
			int newDirection = i;
			newX = x + dx[newDirection];
			newY = y + dy[newDirection];
			if (newX == came_from.first && newY == came_from.second)
			{
				continue;
			}
			if (newX >= 0 && newX < cols && newY >= 0 && newY < rows && data[newY][newX] > 0 && isBoundaryTile(newX, newY))
			{
				return std::make_pair(newX, newY);
			}
		}
		return std::make_pair(-1, -1);
	}

	// float square_distance_between_points(p2t::Point *p1, p2t::Point *p2)
	// {
	//     float x = p1->x - p2->x;
	//     float y = p1->y - p2->y;
	//     return x * x + y * y;
	// }

	bool chunk::is_outline(tile_line l)
	{
		int x1, y1, x2, y2;
		if (l.x1 > l.x2)
		{
			x1 = l.x2;
			y1 = l.y2;
			x2 = l.x1;
			y2 = l.y1;
		}
		else if (l.x1 < l.x2)
		{
			x1 = l.x1;
			y1 = l.y1;
			x2 = l.x2;
			y2 = l.y2;
		}
		else if (l.y1 > l.y2)
		{
			x1 = l.x2;
			y1 = l.y2;
			x2 = l.x1;
			y2 = l.y1;
		}
		else if (l.y1 < l.y2)
		{
			x1 = l.x1;
			y1 = l.y1;
			x2 = l.x2;
			y2 = l.y2;
		}
		else
		{
			return false;
		}
		// l1 is <= l2 in x and y, but < in x or y
		if (x1 < 0 || x1 > CHUNK_SIZE || x2 < 0 || x2 > CHUNK_SIZE)
			return false;
		if (y1 < 0 || y1 > CHUNK_SIZE || y2 < 0 || y2 > CHUNK_SIZE)
			return false;

		if (x1 == x2)
		{
			// vertical line
			if (y1 == 0 && x1 == 0)
			{
				// top left of chunk down line
				return data[y1][x1];
			}
			if (x1 == 0)
			{
				// edge of chunk (left side)
				return data[y1][x1];
			}

			if (x1 == CHUNK_SIZE)
			{
				// edge of chunk (right side)
				return data[y1][x1 - 1];
			}

			return data[y1][x1] && !data[y1][x1 - 1] || !data[y1][x1] && data[y1][x1 - 1];
		}
		else if (y1 == y2)
		{
			// horizontal line
			if (x1 == 0 && y1 == 0)
			{
				// top left right line
				return data[y1][x1];
			}
			if (y1 == 0)
			{
				// edge of chunk (top side)
				return data[y1][x1];
			}
			if (y1 == CHUNK_SIZE)
			{
				// edge of chunk (bottom side)
				return data[y1 - 1][x1];
			}

			return data[y1][x1] && !data[y1 - 1][x1] || !data[y1][x1] && data[y1 - 1][x1];
		}

		// int x1 = l.x1;
		// int y1 = l.y1;
		// int x2 = l.x2;
		// int y2 = l.y2;
		// if (x1 < 0 || x1 >= CHUNK_SIZE || x2 < 0 || x2 >= CHUNK_SIZE)
		// 	return false;
		// if (y1 < 0 || y1 >= CHUNK_SIZE || y2 < 0 || y2 >= CHUNK_SIZE)
		// 	return false;
		// if (x1 == x2)
		// {
		// 	// vertical line, compare tiles to the left and right
		// 	if (y1 < y2)
		// 	{
		// 		// check for edge of chunk
		// 		if (x1 == 0 || x1 == CHUNK_SIZE - 1)
		// 		{
		// 			return data[y1][x1];
		// 		}
		// 		if (data[y1][x1] && !data[y1][x1 - 1] || !data[y1][x1] && data[y1][x1 - 1])
		// 			return true;
		// 	}
		// 	else
		// 	{
		// 		// check for edge of chunk
		// 		if (x1 == 0 || x1 == CHUNK_SIZE - 1)
		// 		{
		// 			return data[y2][x2];
		// 		}
		// 		if (data[y2][x2] && !data[y2][x2 - 1] || !data[y2][x2] && data[y2][x2 - 1])
		// 			return true;
		// 	}
		// }
		// else
		// {
		// 	// horizontal line, compare tiles above and below
		// 	if (x1 < x2)
		// 	{
		// 		// check for edge of chunk
		// 		if (y1 == 0 || y1 == CHUNK_SIZE - 1)
		// 		{
		// 			return data[y1][x1];
		// 		}
		// 		if (data[y1][x1] && !data[y1 - 1][x1] || !data[y1][x1] && data[y1 - 1][x1])
		// 			return true;
		// 	}
		// 	else
		// 	{
		// 		// check for edge of chunk
		// 		if (y1 == 0 || y1 == CHUNK_SIZE - 1)
		// 		{
		// 			return data[y2][x2];
		// 		}
		// 		if (data[y2][x2] && !data[y2 - 1][x2] || !data[y2][x2] && data[y2 - 1][x2])
		// 			return true;
		// 	}
		// }
		// if (x1 == x2)
		// {
		// 	// if edge of chunk
		// 	// if (x1 == 0 || x1 == CHUNK_SIZE - 1)
		// 	// 	return true;
		// 	if (y1 < y2)
		// 	{
		// 		// if (!data[y1][x1] != !data[y2][x1])
		// 		// 	return true;
		// 		if (x1 == 0)
		// 		{
		// 			if (data[])
		// 		}
		// 		if (data[y1][x1] && !data[y2][x1] || !data[y1][x1] && data[y2][x1])
		// 			return true;
		// 	}
		// 	else
		// 	{
		// 		// if (!data[y1][x1] != !data[y2][x1])
		// 		// 	return true;
		// 		if (data[y1][x1] && !data[y2][x1] || !data[y1][x1] && data[y2][x1])
		// 			return true;
		// 	}
		// }
		// else if (y1 == y2)
		// {
		// 	// if edge of chunk
		// 	if (y1 == 0 || y1 == CHUNK_SIZE - 1)
		// 		return true;
		// 	if (x1 < x2)
		// 	{
		// 		// if (!data[y1][x1] != !data[y1][x2])
		// 		// 	return true;
		// 		if (data[y1][x1] && !data[y1][x2] || !data[y1][x1] && data[y1][x2])
		// 			return true;
		// 	}
		// 	else
		// 	{
		// 		// if (!data[y1][x1] != !data[y1][x2])
		// 		// 	return true;
		// 		if (data[y1][x1] && !data[y1][x2] || !data[y1][x1] && data[y1][x2])
		// 			return true;
		// 	}
		// }
		return false;
	}

	bool chunk::is_edge(int x, int y)
	{
		if (!data[y][x])
			return false;

		if (x == 0 || x == CHUNK_SIZE - 1 || y == 0 || y == CHUNK_SIZE - 1)
		{
			return data[y][x];
		}

		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				if (data[y + dy[i]][x + dx[j]] == 0)
					return true;
			}
		}
		return false;
	}

	std::vector<std::pair<int, int>> chunk::trace_outline(tile_line start_line, std::unordered_set<tile_line, tile_line_hash> &visited_lines)
	{
		visited_lines.insert(start_line);
		std::vector<std::pair<int, int>> outline;
		// line current_line = start_line;
		std::pair<int, int> start_point = {start_line.x1, start_line.y1};

		outline.push_back(std::make_pair(start_point.first + chunk_x * game::CHUNK_SIZE, start_point.second + chunk_y * game::CHUNK_SIZE));
		std::pair<int, int> current_point = {start_line.x2, start_line.y2};
		// outline.push_back(current_point);

		while (current_point != start_point)
		{
			std::pair<int, int> p = std::make_pair(current_point.first + chunk_x * game::CHUNK_SIZE, current_point.second + chunk_y * game::CHUNK_SIZE);
			outline.push_back(p);

			tile_line up = {current_point.first, current_point.second, current_point.first, current_point.second - 1};
			tile_line right = {current_point.first, current_point.second, current_point.first + 1, current_point.second};
			tile_line left = {current_point.first, current_point.second, current_point.first - 1, current_point.second};
			tile_line down = {current_point.first, current_point.second, current_point.first, current_point.second + 1};

			if (current_point == std::make_pair(36, 0))
			{
				int a = 0;
			}

			if (!visited_lines.count(up))
			{
				if (is_outline(up))
				{
					visited_lines.insert(up);
					current_point = {up.x2, up.y2};
					continue;
				}
			}

			if (!visited_lines.count(right))
			{
				if (is_outline(right))
				{
					visited_lines.insert(right);
					current_point = {right.x2, right.y2};
					continue;
				}
			}

			if (!visited_lines.count(down))
			{
				if (is_outline(down))
				{
					visited_lines.insert(down);
					current_point = {down.x2, down.y2};
					continue;
				}
			}

			if (!visited_lines.count(left))
			{
				if (is_outline(left))
				{
					visited_lines.insert(left);
					current_point = {left.x2, left.y2};
					continue;
				}
			}
			// no line, exit
			break;
		}

		return outline;
	}

	std::vector<std::vector<std::pair<float, float>>> chunk::create_outlines()
	{
		auto start = std::chrono::high_resolution_clock::now();
		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);

		std::unordered_set<tile_line, tile_line_hash> visited_lines;
		std::vector<std::vector<std::pair<int, int>>> outlines;

		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int x = 0; x < CHUNK_SIZE; x++)
			{
				if (data[y][x] == 0)
					continue;

				if (!is_edge(x, y))
					continue;

				// tile_line tile_top = {x, y, x + 1, y};
				// tile_line tile_bottom = {x, y + 1, x + 1, y + 1};
				// tile_line tile_left = {x, y, x, y + 1};
				// tile_line tile_right = {x + 1, y, x + 1, y + 1};
				tile_line tile_up = {x, y, x, y - 1};
				tile_line tile_right = {x, y, x + 1, y};
				tile_line tile_down = {x, y, x, y + 1};
				tile_line tile_left = {x, y, x - 1, y};

				std::vector<std::pair<int, int>> outline;
				if (!visited_lines.count(tile_up) && is_outline(tile_up))
				{
					// do top stuff
					// outlines.push_back(trace_outline(tile_up, visited_lines));
					outline = trace_outline(tile_up, visited_lines);
				}
				else if (!visited_lines.count(tile_right) && is_outline(tile_right))
				{
					// do bottom stuff
					// outlines.push_back(trace_outline(tile_right, visited_lines));
					outline = trace_outline(tile_right, visited_lines);
				}
				else if (!visited_lines.count(tile_down) && is_outline(tile_down))
				{
					// do left stuff
					// outlines.push_back(trace_outline(tile_down, visited_lines));
					outline = trace_outline(tile_down, visited_lines);
				}
				else if (!visited_lines.count(tile_left) && is_outline(tile_left))
				{
					// do right stuff
					// outlines.push_back(trace_outline(tile_left, visited_lines));
					outline = trace_outline(tile_left, visited_lines);
				}

				if (outline.size() > 3)
				{
					outlines.push_back(outline);

					// // print outline
					// printf("outline: %d points\n", outline.size());
					// for (int i = 0; i < outline.size(); i++)
					// {
					// 	auto point = outline[i];
					// 	printf("(%d, %d) ", point.first, point.second);
					// }
					// printf("\n");
				}
			}
		}
		end = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		printf("Time taken by outline tracing: %d\n", duration.count());
		for (int i = 0; i < outlines.size(); i++)
		{
			std::vector<std::pair<int, int>> outline = outlines[i];
			std::vector<std::pair<int, int>> new_outline;
			new_outline.push_back(outline[0]);

			for (int j = 1; j < outline.size(); j++)
			{
				std::pair<int, int> p1 = outline[j - 1];
				std::pair<int, int> p2 = outline[j];
				std::pair<int, int> p3 = outline[(j + 1) % outline.size()];
				if (p1.first != p2.first || p1.second != p2.second)
				{
					if (p2.first != p3.first || p2.second != p3.second)
					{
						new_outline.push_back(p2);
					}
				}
			}
			outlines[i] = new_outline;
		}
		end = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		printf("Time taken by cleaning: %d\n", duration.count());
		int decimation = 4;
		// remove points to keep only one every decimation value
		for (int i = 0; i < outlines.size(); i++)
		{
			if (outlines[i].size() > 16)
			{
				std::vector<std::pair<int, int>> outline = outlines[i];
				std::vector<std::pair<int, int>> new_outline;
				for (int j = 0; j < outline.size(); j++)
				{
					if ((j % decimation) == 0 || j == 0 || j == outline.size() - 1)
					{
						new_outline.push_back(outline[j]);
					}
				}
				outlines[i] = new_outline;
			}
		}
		end = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		printf("Time taken by decimation: %d\n", duration.count());

		std::vector<std::vector<std::pair<float, float>>> outlines_triangles;
		for (int i = 0; i < outlines.size(); i++)
		{
			std::vector<std::pair<int, int>> &outline = outlines[i];
			std::vector<p2t::Point *> outline_points;
			std::vector<std::pair<float, float>> connected_triangles;
			for (int j = 0; j < outline.size(); j++)
			{
				outline_points.push_back(new p2t::Point(outline[j].first, outline[j].second));
			}

			if (outline_points.size() < 3)
				continue;

			p2t::CDT *cdt = new p2t::CDT(outline_points);

			cdt->Triangulate();

			std::vector<p2t::Triangle *> triangles = cdt->GetTriangles();

			for (p2t::Triangle *triangle : triangles)
			{
				if (*(triangle->GetPoint(0)) == *(triangle->GetPoint(1)) || *(triangle->GetPoint(0)) == *(triangle->GetPoint(2)) || *(triangle->GetPoint(1)) == *(triangle->GetPoint(2)))
					continue;
				for (int j = 0; j < 3; j++)
				{
					p2t::Point *point = triangle->GetPoint(j);
					connected_triangles.push_back(std::make_pair(point->x, point->y));
				}
			}
			outlines_triangles.push_back(connected_triangles);

			for (p2t::Point *point : outline_points)
			{
				delete point;
			}

			delete cdt;
		}
		end = std::chrono::high_resolution_clock::now();
		duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		printf("Time taken by entire function: %d\n", duration.count());
		return outlines_triangles;

		// std::vector<std::vector<std::pair<float, float>>> outlines_triangles;
		// // outlines_triangles.push_back({{chunk_x * CHUNK_SIZE,	// clockwise
		// // 							   chunk_y * CHUNK_SIZE + CHUNK_SIZE / 2.0},
		// // 							  {chunk_x * CHUNK_SIZE + CHUNK_SIZE,
		// // 							   chunk_y * CHUNK_SIZE + CHUNK_SIZE / 2.0},
		// // 							  {chunk_x * CHUNK_SIZE + CHUNK_SIZE / 2.0,
		// // 							   chunk_y * CHUNK_SIZE + CHUNK_SIZE}});
		// outlines_triangles.push_back({{chunk_x * CHUNK_SIZE,	// counter-clockwise
		// 							   chunk_y * CHUNK_SIZE + CHUNK_SIZE / 2.0},
		// 							  {chunk_x * CHUNK_SIZE + CHUNK_SIZE / 2.0,
		// 							   chunk_y * CHUNK_SIZE + CHUNK_SIZE},
		// 							  {chunk_x * CHUNK_SIZE + CHUNK_SIZE,
		// 							   chunk_y * CHUNK_SIZE + CHUNK_SIZE / 2.0 + 50}});
		// return outlines_triangles;
	}

	bool chunk::delete_circle(int x, int y, int radius)
	{
		if (radius == 0)
			return false;

		int local_x = x - chunk_x * CHUNK_SIZE;
		int local_y = y - chunk_y * CHUNK_SIZE;

		int x0 = local_x - radius;
		int x1 = local_x + radius;
		int y0 = local_y - radius;
		int y1 = local_y + radius;
		if (x0 > CHUNK_SIZE || y0 > CHUNK_SIZE || x1 < 0 || y1 < 0)
			return false;

		for (int y = y0; y <= y1; y++)
		{
			for (int x = x0; x <= x1; x++)
			{
				if (x < 0 || y < 0 || x >= CHUNK_SIZE || y >= CHUNK_SIZE)
					continue;
				if ((x - local_x) * (x - local_x) + (y - local_y) * (y - local_y) <= radius * radius)
				{
					data[y][x] = 0;
				}
			}
		}
		return true;
	}
}