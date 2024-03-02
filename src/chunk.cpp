#include "chunk.hpp"
// #include "engine_comp.hpp"
// #include "game_core.hpp"

namespace game
{
	std::array<uint8_t, 256> is_solid_tile;

	void chunk::create_chunk()
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int x = 0; x < CHUNK_SIZE; x++)
			{
				double n_x = (x + chunk_x * game::CHUNK_SIZE) / 20.0;
				double n_y = (y + chunk_y * game::CHUNK_SIZE) / 20.0;
				double noise_1 = perlin_noise_1.noise2D_01(n_x, n_y);
				double noise_2 = perlin_noise_2.noise2D_01(n_x, n_y);
				double noise_3 = perlin_noise_3.noise2D_01(n_x, n_y);

				if (noise_1 * noise_2 > .30)
				// if (noise_1 + noise_2 > .970)
				{ // solid
					if (noise_2 > 0.725)
					{
						data[y][x] = game::STONE;
					}
					else
					{
						if (noise_3 > 0.758)
						{
							data[y][x] = game::SAND;
						}
						else
						{
							data[y][x] = game::DIRT;
						}
					}
				}
				else
				{
					data[y][x] = game::AIR;
				}
				// data[y][x] = game::AIR;

				// data[y][x] = noise_1 > 0.6 ? 1 : 0;

				// if(((x / 15) % 2 == 0) && ((y / 15) % 2 == 0) && (x / 30) % 2 == 0){
				// if(((x / 15) % 2 == 0) && ((y / 15) % 4 == 0) && (x / 30) % 2 == 0){
					
				// 	data[y][x] = game::STONE;
				// }
				// else if(((x / 15) % 2 == 0) && ((y / 15) % 4 == 2) && (x / 30) % 2 == 0)
				// {
				// 	data[y][x] = game::SAND;
				// }
				// else {
				// 	data[y][x] = game::AIR;
				// }
			}
		}

		// spawn some grass
		for (int y = 1; y < CHUNK_SIZE; y++)
		{
			for (int x = 0; x < CHUNK_SIZE; x++)
			{
				if (data[y][x] == game::DIRT && data[y - 1][x] == game::AIR)
				{
					if (rand() % 100 < 10)
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
		uint16_t rows = (uint16_t)data.size();
		uint16_t cols = (uint16_t)data[0].size();
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
		uint16_t rows = (uint16_t)data.size();
		uint16_t cols = (uint16_t)data[0].size();
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

	const int adjacent_tiles_dx[4] = {-1, 0, 0, -1};	// bottom left, bottom right, top right, top left
	const int adjacent_tiles_dy[4] = {0, 0, -1, -1};
	const int offset[4] = {1, 2, 4, 8};
	uint16_t chunk::get_tile_edginess(int x, int y)
	{
		uint16_t edginess = 0;

		for (int i = 0; i < 4; i++)
		{
			int adjacent_x = x + adjacent_tiles_dx[i];
			int adjacent_y = y + adjacent_tiles_dy[i];
			if (adjacent_x < 0 || adjacent_x >= CHUNK_SIZE || adjacent_y < 0 || adjacent_y >= CHUNK_SIZE)
			{
				continue;
			}
			// if (data[adjacent_y][adjacent_x] >= SOLID_TILE_START_INDEX)
			if (is_solid_tile[data[adjacent_y][adjacent_x]])
			{
				edginess += offset[i];
			}
		}

		return edginess;
	}

	const std::array<std::vector<tile_linef>, 16> edges_lines = {
		{{},
		 {{-0.5, 0, 0, 0.5}},
		 {{0, 0.5, 0.5, 0}},
		 {{-0.5, 0, 0.5, 0}},
		 {{0, -0.5, 0.5, 0}},
		 {{-0.5, 0, 0, -0.5}, {0, 0.5, 0.5, 0}},
		 {{0, 0.5, 0, -0.5}},
		 {{-0.5, 0, 0, -0.5}},
		 {{-0.5, 0, 0, -0.5}},
		 {{0, -0.5, 0, 0.5}},
		 {{-0.5, 0, 0, 0.5}, {0, -0.5, 0.5, 0}},
		 {{0, -0.5, 0.5, 0}},
		 {{-0.5, 0, 0.5, 0}},
		 {{0, 0.5, 0.5, 0}},
		 {{-0.5, 0, 0, 0.5}},
		 {}}};

	struct line_mapping_pair
	{
		std::pair<float, float> p1{-1.f, -1.f};
		std::pair<float, float> p2{-1.f, -1.f};

		line_mapping_pair() {}
		line_mapping_pair(std::pair<float, float> p)
		{
			p1 = p;
		}

		void insert(std::pair<float, float> p)
		{
			if (p1.first == -1)
			{
				p1 = p;
			}
			else if (p2.first == -1)
			{
				p2 = p;
			}
			else
			{
				printf("Error: line_mapping_pair already has two points\n");
			}
		}
		std::pair<float, float> get_next()
		{
			if (p2.first != -1)
			{
				std::pair<float, float> p = p2;
				p2 = {-1.f, -1.f};
				return p;
			}
			else if (p1.first != -1)
			{
				std::pair<float, float> p = p1;
				p1 = {-1.f, -1.f};
				return p;
			}
			else
			{
				printf("Error: line_mapping_pair has no points\n");
				return {-1.f, -1.f};
			}
		}
		void remove_point(std::pair<float, float> p)
		{
			if (p1 == p)
			{
				if (p2.first != -1)
				{
					p1 = p2;
					p2 = {-1.f, -1.f};
				}
				else
					p1 = {-1.f, -1.f};
			}
			else if (p2 == p)
			{
				p2 = {-1.f, -1.f};
			}
			else
			{
				printf("Error: line_mapping_pair does not contain point\n");
			}
		}
	};

	void chunk::create_outlines(std::vector<std::vector<std::pair<float, float>>> * chunk_outline)
	{
		// start time
		auto start = std::chrono::high_resolution_clock::now();
		std::unordered_map<std::pair<float, float>, line_mapping_pair, chunk_coord_pair_hash> edge_lines;
		std::unordered_map<std::pair<float, float>, line_mapping_pair, chunk_coord_pair_hash> edge_lines_reverse;

		for (int y = 0; y < CHUNK_SIZE + 1; y++)
		{
			for (int x = 0; x < CHUNK_SIZE + 1; x++)
			{
				// if (data[y][x] >= SOLID_TILE_START_INDEX)
				// {
					uint16_t edginess = get_tile_edginess(x, y);
					for (tile_linef line : edges_lines[edginess])
					{
						line.x1 += x;
						line.y1 += y;
						line.x2 += x;
						line.y2 += y;
						if (!edge_lines.count({line.x1, line.y1}))
						{
							edge_lines[{line.x1, line.y1}] = line_mapping_pair({line.x2, line.y2});

							if (!edge_lines_reverse.count({line.x2, line.y2}))
								edge_lines_reverse[{line.x2, line.y2}] = line_mapping_pair({line.x1, line.y1});
							else
								edge_lines_reverse[{line.x2, line.y2}].insert({line.x1, line.y1});
						}
						else
						{
							edge_lines[{line.x1, line.y1}].insert({line.x2, line.y2});
							if (!edge_lines_reverse.count({line.x2, line.y2}))
								edge_lines_reverse[{line.x2, line.y2}] = line_mapping_pair({line.x1, line.y1});
							else
								edge_lines_reverse[{line.x2, line.y2}].insert({line.x1, line.y1});
						}
					}
				// }
			}
		}
		auto last_time = std::chrono::high_resolution_clock::now();
		auto running_duration = last_time - start;
		// printf("		marching squares: %f mis\n", running_duration.count() / 1000.0);

		std::vector<std::vector<std::pair<float, float>>> outlines;
		while (1)
		{
			if (edge_lines.empty())
				break;
			std::vector<std::pair<float, float>> current_outline;
			std::pair<float, float> start_point = edge_lines.begin()->first;

			// current_outline.push_back(start_point);
			std::pair<float, float> current_point = start_point;

			do
			{
				std::pair<float, float> next_point;
				if (edge_lines.count(current_point))
				{
					next_point = edge_lines[current_point].get_next();
					if (edge_lines[current_point].p1.first == -1)
					{
						edge_lines.erase(current_point);
					}
					edge_lines_reverse[next_point].remove_point(current_point);
					if (edge_lines_reverse[next_point].p1.first == -1)
					{
						edge_lines_reverse.erase(next_point);
					}
				}
				else if (edge_lines_reverse.count(current_point))
				{
					next_point = edge_lines_reverse[current_point].get_next();
					if (edge_lines_reverse[current_point].p1.first == -1)
					{
						edge_lines_reverse.erase(current_point);
					}
					edge_lines[next_point].remove_point(current_point);
					if (edge_lines[next_point].p1.first == -1)
					{
						edge_lines.erase(next_point);
					}
				}
				else
				{
 					printf("Error: outline ended prematurely\n");
					break;
				}

				if (next_point.first == -1)
				{
					printf("Error: outline ended prematurely\n");
					break;
				}

				current_point = next_point;

				current_outline.push_back(current_point);

			} while (start_point != current_point);

			outlines.push_back(current_outline);
		}

		running_duration = std::chrono::high_resolution_clock::now() - last_time;
		// last_time = std::chrono::high_resolution_clock::now();
		// printf("		Connecting edges: %f mis\n", running_duration.count() / 1000.0);

		// std::vector<std::vector<std::pair<float, float>>> * outlines_triangles = new std::vector<std::vector<std::pair<float, float>>>();
		int vert_retention_count = 4;
		for (int i = 0; i < outlines.size(); i++)
		{
			std::vector<p2t::Point *> outline_points;
			// printf("\nNew outline\n");
			for (int j = 0; j < outlines[i].size(); j++)
			{
				// printf("%d, %d\n", outlines[i][j].x1, outlines[i][j].y1);
				if (outlines[i].size() < 12 || j % vert_retention_count == 0)
				{
					// should also remove useless points
					outline_points.push_back(new p2t::Point(outlines[i][j].first, outlines[i][j].second));
				}
			}
			if (outline_points.size() < 3)
				continue;

			try
			{
				p2t::CDT *cdt = new p2t::CDT(outline_points);
				cdt->Triangulate();
				std::vector<p2t::Triangle *> triangles = cdt->GetTriangles();
				std::vector<std::pair<float, float>> outline_triangles;

				for (int j = 0; j < triangles.size(); j++)
				{
					p2t::Point *p1 = triangles[j]->GetPoint(0);
					p2t::Point *p2 = triangles[j]->GetPoint(1);
					p2t::Point *p3 = triangles[j]->GetPoint(2);

					outline_triangles.push_back(std::make_pair((float)(p1->x + chunk_x * CHUNK_SIZE), (float)(p1->y + chunk_y * CHUNK_SIZE)));
					outline_triangles.push_back(std::make_pair((float)(p2->x + chunk_x * CHUNK_SIZE), (float)(p2->y + chunk_y * CHUNK_SIZE)));
					outline_triangles.push_back(std::make_pair((float)(p3->x + chunk_x * CHUNK_SIZE), (float)(p3->y + chunk_y * CHUNK_SIZE)));
				}

				chunk_outline -> push_back(outline_triangles);
				for (p2t::Point *p : outline_points)
					delete p;
				delete cdt;
			}
			catch (std::exception e)
			{
				printf("Exception: %s\n", e.what());
				continue;
			}
		}

		running_duration = std::chrono::high_resolution_clock::now() - last_time;
		// last_time = std::chrono::high_resolution_clock::now();
		// printf("		triangulating: %f mis\n", running_duration.count() / 1000.0);


		// auto end = std::chrono::high_resolution_clock::now();
		// auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		// printf("	Time taken by entire function: %d\n", duration.count());
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
				if(data[y][x] == tile_type::BEDROCK || data[y][x] < SOLID_TILE_START_INDEX)
					continue;
				if ((x - local_x) * (x - local_x) + (y - local_y) * (y - local_y) <= radius * radius)
				{
					if(game_engine::in_set(data[y][x], ROOT, WOOD, LEAF))
					{
						data[y][x] = EMBER;
					} else {
						data[y][x] = 1;
					}
				}
			}
		}
		return true;
	}
}