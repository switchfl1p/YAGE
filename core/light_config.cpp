#include <light_config.hpp>

namespace light_config {
    PointLightsData initPointLights() {
        PointLightsData data;

        data.pl_timers[0] = Timer(Timer::TT_LOOP, 15.0f);
        data.pl_timers[1] = Timer(Timer::TT_LOOP, 25.0f);
        data.pl_timers[2] = Timer(Timer::TT_LOOP, 35.0f);

        std::vector<glm::vec3> temp_pl_pos_values;
        temp_pl_pos_values.reserve(60);

        // Light 0 - zigzags and cuts inward, varying heights
        temp_pl_pos_values.push_back(glm::vec3(-5.0f, 1.5f, 8.0f));
        temp_pl_pos_values.push_back(glm::vec3(-2.0f, 3.0f, 5.0f));   // cuts inward, rises
        temp_pl_pos_values.push_back(glm::vec3(-8.0f, 1.0f, 2.0f));   // swings back out, dips
        temp_pl_pos_values.push_back(glm::vec3(-4.0f, 4.0f, -3.0f));  // cuts inward again, peaks
        temp_pl_pos_values.push_back(glm::vec3(-8.0f, 1.0f, -7.0f));  // back to edge, low
        temp_pl_pos_values.push_back(glm::vec3(0.0f,  2.5f, -4.0f));  // cuts across center
        temp_pl_pos_values.push_back(glm::vec3(7.0f,  1.0f, -8.0f));  // swings to far corner
        temp_pl_pos_values.push_back(glm::vec3(4.0f,  3.5f, -2.0f));  // cuts inward, rises
        temp_pl_pos_values.push_back(glm::vec3(8.0f,  1.0f,  3.0f));  // back to edge
        temp_pl_pos_values.push_back(glm::vec3(2.0f,  4.0f,  6.0f));  // cuts inward, high
        temp_pl_pos_values.push_back(glm::vec3(7.0f,  1.0f,  8.0f));  // back to edge, low
        data.pl_interpolators[0].setValues(temp_pl_pos_values);

        // Light 1 - erratic, tight direction changes, big height variance
        temp_pl_pos_values.clear();
        temp_pl_pos_values.push_back(glm::vec3( 3.0f, 4.5f,  2.0f));  // starts high near center
        temp_pl_pos_values.push_back(glm::vec3( 8.0f, 1.0f,  5.0f));  // dives to edge
        temp_pl_pos_values.push_back(glm::vec3( 4.0f, 2.0f, -1.0f));  // cuts back
        temp_pl_pos_values.push_back(glm::vec3( 7.0f, 4.0f, -7.0f));  // far corner, rises
        temp_pl_pos_values.push_back(glm::vec3( 1.0f, 1.0f, -5.0f));  // cuts inward, low
        temp_pl_pos_values.push_back(glm::vec3(-4.0f, 3.5f, -8.0f));  // swings to edge, rises
        temp_pl_pos_values.push_back(glm::vec3(-2.0f, 1.0f, -2.0f));  // cuts to center, dips
        temp_pl_pos_values.push_back(glm::vec3(-8.0f, 4.0f,  1.0f));  // far edge, high
        temp_pl_pos_values.push_back(glm::vec3(-3.0f, 1.5f,  5.0f));  // cuts inward
        temp_pl_pos_values.push_back(glm::vec3( 2.0f, 5.0f,  8.0f));  // peaks high near edge
        temp_pl_pos_values.push_back(glm::vec3(-1.0f, 1.0f,  3.0f));  // drops to near center
        temp_pl_pos_values.push_back(glm::vec3( 6.0f, 3.0f,  1.0f));  // swings out
        data.pl_interpolators[1].setValues(temp_pl_pos_values);

        // Light 2 - figure-8, crosses center, medium height
        temp_pl_pos_values.clear();
        temp_pl_pos_values.push_back(glm::vec3( 7.0f, 2.0f,  5.0f));
        temp_pl_pos_values.push_back(glm::vec3( 4.0f, 2.5f,  8.0f));
        temp_pl_pos_values.push_back(glm::vec3(-1.0f, 2.0f,  5.0f));
        temp_pl_pos_values.push_back(glm::vec3( 0.0f, 1.5f,  0.0f));  // crosses center
        temp_pl_pos_values.push_back(glm::vec3(-1.0f, 2.0f, -5.0f));
        temp_pl_pos_values.push_back(glm::vec3(-4.0f, 2.5f, -8.0f));
        temp_pl_pos_values.push_back(glm::vec3(-7.0f, 2.0f, -5.0f));
        temp_pl_pos_values.push_back(glm::vec3(-4.0f, 1.5f,  0.0f));  // crosses center again
        temp_pl_pos_values.push_back(glm::vec3( 4.0f, 2.0f, -5.0f));
        temp_pl_pos_values.push_back(glm::vec3( 7.0f, 2.5f, -8.0f));
        temp_pl_pos_values.push_back(glm::vec3( 8.0f, 2.0f,  0.0f));
        temp_pl_pos_values.push_back(glm::vec3( 5.0f, 1.5f,  6.0f));
        data.pl_interpolators[2].setValues(temp_pl_pos_values);

        return data;
    }
}