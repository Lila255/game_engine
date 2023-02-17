#include "chunk.hpp"

// #include "engine_comp.hpp"

namespace game
{
	static siv::PerlinNoise perlin_noise(0.0);

	void chunk::create_chunk()
	{
		for (int y = 0; y < CHUNK_SIZE; y++)
		{
			for (int x = 0; x < CHUNK_SIZE; x++)
			{
				double n_x = (x + chunk_x * game::CHUNK_SIZE) / 50.0;
				double n_y = (y + chunk_y * game::CHUNK_SIZE) / 50.0;
				double noise = perlin_noise.noise2D_01(n_x, n_y);
				// if (noise > 0.6)
				// {
				// 	data[y][x] = 1;
				// }
				// else
				// {
				// 	data[y][x] = 0;
				// }
				data[y][x] = noise > 0.45 ? 1 : 0;
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

	std::vector<std::vector<std::pair<float, float>>> chunk::create_outlines_centers()
	{
		auto start = std::chrono::high_resolution_clock::now();
		std::vector<std::vector<std::pair<float, float>>> outlines;
		std::unordered_set<std::pair<int, int>, game_engine::pair_hash> visited;
		int rows = data.size();
		int cols = data[0].size();
		// Iterate over all tiles
		for (int x = 0; x < CHUNK_SIZE; x++)
		{
			for (int y = 0; y < CHUNK_SIZE; y++)
			{
				if (data[y][x] > 0)
				{
					std::pair<int, int> tile = std::make_pair(x, y);
					// If the tile is solid and not visited, start outlining
					if (!visited.count(tile))
					{
						// if(chunk_x == 1) printf("Current start tile: (%d, %d)\n", x, y);
						if (isBoundaryTile(x, y))
						{
							uint32_t current_x = x;
							uint32_t current_y = y;
							uint32_t previous_x = x;
							uint32_t previous_y = y;
							uint32_t direction = 0;
							std::vector<std::pair<float, float>> outline;
							outline.push_back(std::make_pair(current_x + chunk_x * CHUNK_SIZE + 0.5, current_y + chunk_y * CHUNK_SIZE + 0.5));
							std::pair<int, int> next_tile = getNextBoundaryTile2(current_x, current_y, {current_x, current_y});
							if (visited.count(next_tile))
							{
								continue;
							}
							if (next_tile.first == -1)
								continue;
							current_x = next_tile.first;
							current_y = next_tile.second;

							while (current_x != x || current_y != y)
							{
								std::pair<float, float> p = std::make_pair(current_x + chunk_x * game::CHUNK_SIZE + 0.5, current_y + chunk_y * game::CHUNK_SIZE + 0.5);
								outline.push_back(p);

								// outline.push_back(std::make_pair(current_y + 0.5 + chunk_x * game::CHUNK_SIZE, current_x + chunk_y * game::CHUNK_SIZE + 0.5));

								// outline.push_back(std::make_pair(current_x + 0.5 + chunk_x * game::CHUNK_SIZE, current_y + chunk_y * game::CHUNK_SIZE + 0.5));
								visited.insert(std::make_pair(current_x, current_y));
								// next_tile = getNextBoundaryTile2(current_x, current_y,  {previous_x, previous_y});
								// Do this ^^^ inline

								int newX;
								int newY;
								next_tile = std::make_pair(-1, -1);
								for (int i = 0; i < 4; i++)
								{
									newX = current_x + dx[i];
									newY = current_y + dy[i];
									if (newX == previous_x && newY == previous_y)
									{
										continue;
									}
									// if (newX >= 0 && newX < rows && newY >= 0 && newY < cols && data[newX][newY] > 0)
									// {

									if (newX >= 0 && newX < cols && newY >= 0 && newY < rows && data[newY][newX] > 0 && isBoundaryTile(newX, newY))
									{
										std::pair<float, float> next_next_tile = std::make_pair(newX, newY);
										if (visited.count(next_next_tile))
										{
											continue;
										}
										next_tile = std::make_pair(newX, newY);
										break;
									}
								}
								if (next_tile.first == -1)
								{
									// didnt find a next tile, so we are done
									current_x = x;
									current_y = y;
									continue;
								}

								previous_x = current_x;
								previous_y = current_y;
								current_x = next_tile.first;
								current_y = next_tile.second;
								// printf("Next tile(looping): (%d, %d)\n", current_x, current_y);
							}
							outlines.push_back(outline);
						}
					}
				}
			}
		}
		for (int i = 0; i < outlines.size(); i++)
		{
			std::vector<std::pair<float, float>> outline = outlines[i];
			std::vector<std::pair<float, float>> new_outline;
			new_outline.push_back(outline[0]);
			for (int j = 1; j < outline.size(); j++)
			{
				std::pair<float, float> p1 = outline[j - 1];
				std::pair<float, float> p2 = outline[j];
				std::pair<float, float> p3 = outline[(j + 1) % outline.size()];
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

		int decimation = 8;
		// remove points to keep only one every decimation value
		for (int i = 0; i < outlines.size(); i++)
		{
			std::vector<std::pair<float, float>> outline = outlines[i];
			std::vector<std::pair<float, float>> new_outline;
			for (int j = 0; j < outline.size(); j++)
			{
				if (j % decimation == 0 || j == 0 || j == outline.size() - 1)
				{
					new_outline.push_back(outline[j]);
				}
			}
			// push first point to end to close the outline
			// new_outline.push_back(new_outline[0]);
			outlines[i] = new_outline;
		}

		// std::vector<std::vector<std::pair<float, float>>> outlines_triangles;
		// for (int i = 0; i < outlines.size(); i++)
		// {
		//     std::vector<std::pair<float, float>> &outline = outlines[i];
		//     std::vector<point_2> outline_triangles;
		//     std::vector<Triangulation::Vertex_handle> vertices;
		//     std::vector<std::pair<float, float>> connected_triangles;
		//     for (int j = 0; j < outline.size(); j++)
		//     {
		//         outline_triangles.emplace_back(outline[j].first, outline[j].second);
		//     }
		//     Triangulation t;
		//     t.insert(outline_triangles.begin(), outline_triangles.end());
		//     for(Edge_iterator e = t.finite_edges_begin(); e != t.finite_edges_end(); ++e)
		//     {
		//         Triangulation::Edge it = *e;
		//         Triangulation::Vertex_handle v1 = it.first->vertex((it.second + 1) % 3);
		//         Triangulation::Vertex_handle v2 = it.first->vertex((it.second + 2) % 3);
		//         vertices.push_back(v1);
		//         vertices.push_back(v2);
		//         connected_triangles.push_back(std::make_pair(v1->point().x(), v1->point().y()));
		//         connected_triangles.push_back(std::make_pair(v2->point().x(), v2->point().y()));
		//     }
		//     outlines_triangles.push_back(connected_triangles);
		//     // outlines_triangles.push_back(outline_triangles);
		// }

		std::vector<std::vector<std::pair<float, float>>> outlines_triangles;
		for (int i = 0; i < outlines.size(); i++)
		{
			std::vector<std::pair<float, float>> &outline = outlines[i];
			std::vector<p2t::Point *> outline_points;
			std::vector<std::pair<float, float>> connected_triangles;
			for (int j = 0; j < outline.size(); j++)
			{
				outline_points.push_back(new p2t::Point(outline[j].first, outline[j].second));
			}
			// printf("Points size: %d\n", outline_points.size());
			if (outline_points.size() < 3)
				continue;

			p2t::CDT *cdt = new p2t::CDT(outline_points);
			cdt->Triangulate();

			const float MAX_ASPECT_RATIO = 4.0f; // Adjust as desired
			// add steiner points
			std::vector<p2t::Triangle *> triangles = cdt->GetTriangles();

			// Check each triangle for aspect ratio
			// uint16_t max_points = 16;
			// uint16_t points_added = 0;
			// for(p2t::Triangle * t : triangles)
			// {
			//     float ab = square_distance_between_points(t->GetPoint(0), t->GetPoint(1));
			//     float bc = square_distance_between_points(t->GetPoint(1), t->GetPoint(2));
			//     float ca = square_distance_between_points(t->GetPoint(2), t->GetPoint(0));
			//     float max = std::max(ab, std::max(bc, ca));
			//     float min = std::min(ab, std::min(bc, ca));
			//     int largest_length = -1;
			//     if (max == ab)
			//         largest_length = 0;
			//     else if (max == bc)
			//         largest_length = 1;
			//     else if (max == ca)
			//         largest_length = 2;
			//     float aspect_ratio = max / min;
			//     printf("aspect_ratio: %f\n", aspect_ratio);
			//     if (aspect_ratio > MAX_ASPECT_RATIO && points_added < max_points)
			//     {
			//         points_added++;
			//         // Add a new point in the middle of the longest edge
			//         p2t::Point *p = new p2t::Point((t->GetPoint(largest_length)->x + t->GetPoint((largest_length + 1) % 3)->x) / 2.0f,
			//                                        (t->GetPoint(largest_length)->y + t->GetPoint((largest_length + 1) % 3)->y) / 2.0f);
			//         printf("Adding point between (%f, %f) and (%f, %f) at (%f, %f)\n",
			//                t->GetPoint(largest_length)->x, t->GetPoint(largest_length)->y,
			//                t->GetPoint((largest_length + 1) % 3)->x, t->GetPoint((largest_length + 1) % 3)->y,
			//                p->x, p->y);
			//         // outline_points.push_back(p);
			//         cdt->AddPoint(p);
			//         // cdt->Triangulate();
			//         // break;
			//     }
			// }
			// cdt->Triangulate();
			// triangles = cdt->GetTriangles();

			for (p2t::Triangle *triangle : triangles)
			{
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

		auto end = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
		printf("Time taken by collision creations: %d\n", duration.count());
		// return outlines;
		return outlines_triangles;
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