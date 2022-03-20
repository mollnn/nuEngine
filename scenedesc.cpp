#include "scenedesc.h"

SceneDesc::SceneDesc()
{
}

SceneDesc::SceneDesc(std::shared_ptr<Object> obj)
{
    std::function<void(std::shared_ptr<Object>, glm::mat4)> solve = [&](std::shared_ptr<Object> p, glm::mat4 model)
    {
        if (std::dynamic_pointer_cast<Model>(p))
        {
            models.addChildren(std::dynamic_pointer_cast<Model>(p), model);
        }
        if (std::dynamic_pointer_cast<PointLight>(p))
        {
            PointLight tmp = *std::dynamic_pointer_cast<PointLight>(p);
            tmp.position = glm::vec3(model * glm::vec4(tmp.position, 1.0f));
            point_lights.push_back(tmp);
        }
        for (auto [q, o] : p->children)
        {
            solve(q, model * o);
        }
    };

    solve(obj, glm::mat4(1.0));
}