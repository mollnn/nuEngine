#include "object.h"

void Object::addChildren(std::shared_ptr<Object> obj, const glm::mat4 &offset )
{
    children.push_back({obj, offset});
}