#include "profiler.h"

void Profiler::begin()
{
    frame_start = glfwGetTime();
    last_tick = frame_start;
    std::cout << "PROFILER BEGIN" << std::endl;
    idx = 0;
}

void Profiler::tick(const std::string &msg)
{
    glFinish();
    double t = glfwGetTime();
    if (move_avg.size() <= idx)
    {
        move_avg.push_back(t - last_tick);
    }
    else
    {
        move_avg[idx] *= alpha;
        move_avg[idx] += (1 - alpha) * (t - last_tick);
    }
    std::cout << "PROFILER TICK " << msg << ":   \t" << t - last_tick << ", \t" << t - frame_start << "  \tavg " << move_avg[idx] << std::endl;
    idx++;
    last_tick = t;
}

void Profiler::end()
{
    double t = glfwGetTime();
    std::cout << "PROFILER END"
              << ": \t" << t - last_tick << ",   \t" << t - frame_start << std::endl
              << std::endl;
    alpha = 1 - (1 - alpha) * beta;
}
