#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <bits/stdc++.h>
#include <glm/glm.hpp>

struct Object
{
    std::vector<std::pair<std::shared_ptr<Object>, glm::mat4>> children;

    virtual void addChildren(std::shared_ptr<Object> obj, const glm::mat4 &offset = glm::mat4(1.0f));
};

#endif