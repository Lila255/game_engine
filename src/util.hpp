#pragma once

#include <cstddef>
#include <cstdlib>
#include <unordered_map>
#include <queue>

#include "PerlinNoise.hpp"
#include "opengl_util.hpp"
// #include "engine_core.hpp"

namespace game_engine
{
    // template<> struct std::hash<game_engine::entity> {
    //     std::size_t operator()(const game_engine::entity& e) const {
    //         return std::hash<uint32_t>()(e.m_id);
    //     }
    // };

    //     template<> struct std::hash<Block> {
    //     std::size_t operator()(const Block & block) const noexcept {
    //         return std::hash<std::string>{}(block.getName());
    //     }
    // };

    // template<> struct std::hash<game_engine::entity> {
    //     std::size_t operator()(const game_engine::entity &e) const noexcept {
    //         return std::hash<uint32_t>{}(e.m_id);
    //     }
    // };

    struct family {
    private:
        static uint32_t identifier() noexcept {
            static uint32_t id = 0;
            return id++;
        }
    public:
        template<typename>
        static uint32_t type() noexcept {
            static const uint32_t id = identifier();
            return id;
        }
    };

    struct id_generator {
    public:
        static uint32_t generate() noexcept ;// {
            // static uint32_t current_max = 0;
            // static std::priority_queue<uint32_t> free_ids;
            
            // printf("Free is empty: %d\n", free_ids.empty());
            // if (free_ids.empty()) {
            //     return current_max++;
            // } else {
            //     uint32_t id = free_ids.top();
            //     free_ids.pop();
            //     return id;
            // }
        // }


        static void free_id(uint32_t id) noexcept;// {
        //     static std::priority_queue<uint32_t> free_ids;
        //     free_ids.push(id);
        //     if(free_ids.empty()) {
        //         printf("Error: free_ids is empty\n");
        //     } else {
        //         printf("free_ids is not empty: %d\n", free_ids.size());
        //     }
        // }
    };

}