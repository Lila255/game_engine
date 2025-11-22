#include "chunk_frame_system.hpp"
// #include ""

namespace game
{
	void chunk_frame_system::start_thread()
	{
		printf("Starting chunk frame system thread\n");
		running = 1;
		uint64_t step_count = 0;

		while(running)
		{	
			// std::chrono::time_point<std::chrono::steady_clock> start = std::chrono::steady_clock::now();
			b2d_mutex.lock();
			printf("Updating chunk frame system\nframe current: %ld\n", current_frame);
			
			update();
			b2d_mutex.unlock();
			std::chrono::time_point<std::chrono::steady_clock> end = std::chrono::steady_clock::now();
			// std::chrono::microseconds elapsed_ms = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
			// printf("Elapsed time: %ld\n", elapsed_ms.count());
			double noise = perlin_noise_chunk_frame.noise1D_01(step_count / 10.0);
			uint64_t time_sleep_ms = 666 + (noise * 1000) - 500;
			printf("Sleeping for %ldms\n", time_sleep_ms);
			add_time(time_sleep_ms * 1000);
			// if(elapsed_ms.count() < time_step_ms)
			// {
				std::this_thread::sleep_for(std::chrono::microseconds(time_sleep_ms * 1000));
				// std::this_thread::sleep_for(std::chrono::microseconds((time_step_ms - elapsed_ms.count()) * 1000));
			// }

			step_count++;
		}
	}

	void chunk_frame_system::update()
	{
		increment_counter();
		// std::array<game::chunk *, 16Ui64> *chunks = world_tile_sys->get_chunks_base();
		// if(current_frame == 0)
		// {
		// 	forwards = true;
		// }
		// else if(current_frame == CHUNK_FRAMES - 1)
		// {
		// 	forwards = false;
		// }

		// current_frame += forwards ? 1 : -1;
		// // if(current_frame >= CHUNK_FRAMES)
		// // {
		// // 	current_frame = CHUNK_FRAMES - 1;
		// // }
		// // else if(current_frame < 0)
		// // {
		// // 	current_frame = 0;
		// // 	forwards = !forwards;
		// // }
		
		// for (auto &chunk : *chunks)
		// {
		// 	chunk->update_frame(current_frame);
		// }
		// std::array<uint8_t, game::NUM_CHUNKS> * modified_chunks = world_tile_sys->get_modified_chunks();

		// // set all values to 1
		// std::fill(modified_chunks->begin(), modified_chunks->end(), 1);

	}
}