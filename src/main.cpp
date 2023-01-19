#include <stdio.h>
#include "engine_comp.hpp"

int main()
{
    printf("Hello World!\n");
    std::vector<game_engine::entity> entities;
    game_engine::box_system box_sys;

    for(int i = 0; i < 10; ++i) {
        game_engine::entity e(i);
        entities.push_back(e);
        box_sys.add(e.m_id, { 1.0f, 1.0f, 1.0f, 1.0f * i });
    }

    for(auto& e : entities) {
        auto & box = box_sys.get(e.m_id);
        printf("Entity %d has box: %f, %f, %f, %f\n", e.m_id, box.x, box.y, box.w, box.h);
    }

    try{
        box_sys.update();
    } catch (std::exception& e) {
        printf("Caught exception: %s\n", e.what());
    }

    try{
        box_sys.get(100);
    } catch (std::exception& e) {
        printf("Caught exception: %s\n", e.what());
    }




    printf("Program exiting\n");
    return 0;
}
