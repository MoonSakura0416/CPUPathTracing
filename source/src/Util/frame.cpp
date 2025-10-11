#include "Util/frame.h"

Frame::Frame(const glm::vec3& normal)
{
    yAxis_ = glm::normalize(normal);
    glm::vec3 up = glm::abs(normal.y) < 0.99999 ? glm::vec3(0, 1, 0) : glm::vec3(0, 0, 1);
    xAxis_ = glm::normalize(glm::cross(yAxis_, up));
    zAxis_ = glm::normalize(glm::cross(xAxis_, yAxis_));
}

// static void frisvadTangentZUp(const glm::vec3& n, glm::vec3& t, glm::vec3& b)
// {
//     if (n.z < -0.9999999f) {
//         t = glm::vec3(0.0f, -1.0f, 0.0f);
//         b = glm::vec3(-1.0f, 0.0f, 0.0f);
//     } else {
//         float a = 1.0f / (1.0f + n.z);
//         float bb = -n.x * n.y * a;
//         t = glm::vec3(1.0f - n.x * n.x * a, bb, -n.x);  // tangent
//         b = glm::vec3(bb, 1.0f - n.y * n.y * a, -n.y);  // bitangent
//     }
//     t = glm::normalize(t);
//     b = glm::normalize(glm::cross(n, t));
//     t = glm::normalize(glm::cross(b, n));
// }

// Frame::Frame(const glm::vec3& normal)
// {
//     // Frisvad method: Construct xAxis_ and yAxis_ that are orthogonal to zAxis_
//     glm::vec3 n = glm::normalize(normal);
//     glm::vec3 t, b;
//     frisvadTangentZUp(n, t, b);
//     yAxis_ = n;
//     xAxis_ = t;
//     zAxis_ = glm::cross(xAxis_, yAxis_);
// }

glm::vec3 Frame::localFromWorld(const glm::vec3& world) const
{
    return glm::normalize(
        glm::vec3{glm::dot(world, xAxis_), glm::dot(world, yAxis_), glm::dot(world, zAxis_)});
}

glm::vec3 Frame::worldFromLocal(const glm::vec3& local) const
{
    return glm::normalize(local.x * xAxis_ + local.y * yAxis_ + local.z * zAxis_);
}
