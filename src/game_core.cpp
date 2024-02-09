#include "game_core.hpp"

namespace game {
	std::mutex b2d_mutex;

	std::vector<game_engine::vec2> load_normal_vectors()
	{
		std::string path = "normal_vectors.txt";
		// stored in csv format: float, float,\n
		std::vector<game_engine::vec2> normal_vectors;

		std::fstream newfile;
		newfile.open(path, std::ios::in); // open a file to perform read operation using file object
		if (newfile.is_open())
		{ // checking whether the file is open
			std::string tp;
			while (getline(newfile, tp))
			{ // read data from file object and put it into string.
				std::stringstream ss(tp);
				std::string token;
				std::vector<std::string> tokens;
				while (getline(ss, token, ','))
				{
					tokens.push_back(token);
				}
				normal_vectors.push_back(game_engine::vec2(std::stof(tokens[0]), std::stof(tokens[1])));
			}
			newfile.close(); // close the file object.
		}
		return normal_vectors;
	}
}