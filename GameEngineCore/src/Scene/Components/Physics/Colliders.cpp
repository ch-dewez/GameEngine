#include "Colliders.h"
#include <limits>
#include <memory>
#include <iostream>

namespace Engine {
namespace Components {

glm::vec3 SphereCollider::getWorldCenter() const {
    return m_entity->getComponent<Transform>().value().lock().get()->position + center;
}

glm::vec3 CubeCollider::getWorldCenter() const {
    return m_entity->getComponent<Transform>().value().lock().get()->position + center;
}


glm::vec3 CapsuleCollider::getWorldCenter1() const{
    return m_entity->getComponent<Transform>().value().lock().get()->position + center;
};

glm::vec3 CapsuleCollider::getWorldCenter2() const{

    return m_entity->getComponent<Transform>().value().lock().get()->position + center2;
};

float SphereCollider::getRadius() const {
    return radius * m_entity->getComponent<Transform>().value().lock().get()->scale.x;
}

float CapsuleCollider::getRadius() const {
    return radius * m_entity->getComponent<Transform>().value().lock().get()->scale.x;
}

glm::vec3 CapsuleCollider::findFurthestPoint(glm::vec3 dir) const{
    // normalize dir so that radius * dir really is of length r
    glm::vec3 nDir = glm::normalize(dir);

    // axis from center (C1) to center2 (C2)
    glm::vec3 axis = center2 - center;

    // project onto axis: if positive, pick C2, else C1
    float t = glm::dot(nDir, axis);
    const glm::vec3& endPoint = (t > 0.0f ? center2 : center);

    // Minkowski sum: segment support + sphere support
    return endPoint + radius * nDir;
};

glm::vec3 SphereCollider::findFurthestPoint(glm::vec3 dir) const{
    return getWorldCenter() + getRadius() * glm::normalize(dir);
};

glm::vec3 CubeCollider::findFurthestPoint(glm::vec3 dir) const{
    auto vertices = getAllVertices();
    glm::vec3 maxPoint = glm::vec3(0);
    float maxDistance = std::numeric_limits<float>::lowest();

    for (glm::vec3 vertex : vertices) {
        float distance = glm::dot(vertex, glm::normalize(dir));
        if (distance > maxDistance) {
            maxDistance = distance;
            maxPoint = vertex;
        }
    }

    return maxPoint;
};

//TODO: use the transform
std::vector<glm::vec3> CubeCollider::getAllVertices() const{
    std::vector<glm::vec3> result(8);
    glm::vec3 forwardAndSize = forward * forwardHalfSize;
    glm::vec3 upAndSize = up * upHalfSize;
    glm::vec3 rightAndSize = right * rightHalfSize;
    // forward up right
    result[0] = forwardAndSize + upAndSize + rightAndSize + center;
    // forward up left
    result[1] = forwardAndSize + upAndSize - rightAndSize + center;
    // forward down right
    result[2] = forwardAndSize - upAndSize + rightAndSize + center;
    // forward down left
    result[3] = forwardAndSize - upAndSize - rightAndSize + center;
    // backward up right
    result[4] = -forwardAndSize + upAndSize + rightAndSize + center;
    // backward up left
    result[5] = -forwardAndSize + upAndSize - rightAndSize + center;
    // backward down right
    result[6] = -forwardAndSize - upAndSize + rightAndSize + center;
    // backward down left
    result[7] = -forwardAndSize - upAndSize - rightAndSize + center;

    auto model = m_entity->getComponent<Transform>().value().lock()->getModelMatrix();
    //std::cout << "obj" << std::endl;
    for (int i = 0;i<result.size();i++) {
        result[i] = model * glm::vec4(result[i], 1.0f);
        //std::cout << "x " << vertex.x << " y " << vertex.y << " z " << vertex.z << std::endl;
    }

    return std::move(result);
};

/*std::vector<glm::vec3> CubeCollider::getAllAxisSAT(CubeCollider& other) {*/
/*    std::vector<glm::vec3> result(15);*/
/*    result[0] = up;*/
/*    result[1] = right;*/
/*    result[2] = forward;*/
/**/
/*    result[3] = other.up;*/
/*    result[4] = other.right;*/
/*    result[5] = other.forward;*/
/**/
/*    int idx = 6;*/
/*    const glm::vec3 axesA[3] = { up, right, forward };*/
/*    const glm::vec3 axesB[3] = { other.up, other.right, other.forward };*/
/**/
/*    for (int i = 0; i < 3; ++i) {*/
/*        for (int j = 0; j < 3; ++j) {*/
/*            glm::vec3 cp = glm::cross(axesA[i], axesB[j]);*/
/*            result[idx++] = glm::normalize(cp);*/
/*        }*/
/*    }*/
/**/
/*    return std::move(result);*/
/*}*/



}
}

