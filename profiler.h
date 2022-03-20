#ifndef _PROFILER_H
#define _PROFILER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <bits/stdc++.h>


class Profiler
{
    std::vector<double> move_avg;
    double frame_start, last_tick;
    int idx;
    double alpha = 0.5;
    double beta = 0.8;

public:
    void begin();

    void tick(const std::string &msg = "");

    void end();
};

#endif