#pragma once
#include <cstddef>
#include <cstdlib>
#include <unordered_map>

#include "engine_core.hpp"

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
}