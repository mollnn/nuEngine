#include "scene.h"

Scene::Scene() : root(std::make_shared<Object>())
{
}

void Scene::add(std::shared_ptr<Object> obj, const glm::mat4 &offset)
{
    root->addChildren(obj, offset);
}