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

glm::vec3 calculateFaceNormal(
    const std::vector<uint32_t>& faceIndices,
    const std::vector<glm::vec3>& polyVertices) {
    // Ensure we have enough vertices to define a plane/normal
    if (faceIndices.size() < 3) {
        throw std::runtime_error("Face has fewer than 3 vertices, cannot calculate normal.");
        // Or return a zero vector: return glm::vec3(0.0f);
    }

    // Get the positions of the first three vertices
    const glm::vec3& v0 = polyVertices[faceIndices[0]];
    const glm::vec3& v1 = polyVertices[faceIndices[1]];
    const glm::vec3& v2 = polyVertices[faceIndices[2]];

    // Calculate edge vectors
    glm::vec3 edge1 = v1 - v0;
    glm::vec3 edge2 = v2 - v0;

    // Calculate the raw normal using the cross product (assumes CCW winding)
    glm::vec3 rawNormal = glm::cross(edge1, edge2);

    // Check for degenerate cases (collinear vertices -> zero normal)
    // Using length squared avoids sqrt
    float normalLengthSq = glm::dot(rawNormal, rawNormal);
    constexpr float epsilonSq = 1e-12f; // Tolerance for zero length squared

    if (normalLengthSq < epsilonSq) {
        // Vertices are collinear or coincident.
        throw std::runtime_error("Face vertices are collinear, cannot calculate normal.");
        // Or return a zero vector: return glm::vec3(0.0f);
    }

    // Normalize the normal vector and return it
    // return rawNormal / glm::sqrt(normalLengthSq); // Manual normalize
    return glm::normalize(rawNormal); // Use GLM normalize
}

Polyhedron CubeCollider::getPolyhedron() const {
    Polyhedron cubePolyhedron;

    // 1. Get the world-space vertices
    cubePolyhedron.vertices = getAllVertices();

    // 2. Define the faces using indices (ensure Counter-Clockwise winding from outside)
    // Vertex mapping based on your getVertices comments:
    // 0: +F +U +R (Front-Top-Right)
    // 1: +F +U -R (Front-Top-Left)
    // 2: +F -U +R (Front-Bottom-Right)
    // 3: +F -U -R (Front-Bottom-Left)
    // 4: -F +U +R (Back-Top-Right)
    // 5: -F +U -R (Back-Top-Left)
    // 6: -F -U +R (Back-Bottom-Right)
    // 7: -F -U -R (Back-Bottom-Left)

    cubePolyhedron.faces.resize(6);

    // Front face (+F) : (1, 3, 2, 0) -> FTL, FBL, FBR, FTR
    cubePolyhedron.faces[0].vertexIndices = {1, 3, 2, 0};
    // Back face (-F)  : (4, 6, 7, 5) -> BTR, BBR, BBL, BTL
    cubePolyhedron.faces[1].vertexIndices = {4, 6, 7, 5};
    // Top face (+U)   : (5, 1, 0, 4) -> BTL, FTL, FTR, BTR
    cubePolyhedron.faces[2].vertexIndices = {5, 1, 0, 4};
    // Bottom face (-U): (3, 7, 6, 2) -> FBL, BBL, BBR, FBR
    cubePolyhedron.faces[3].vertexIndices = {3, 7, 6, 2};
    // Right face (+R) : (4, 0, 2, 6) -> BTR, FTR, FBR, BBR
    cubePolyhedron.faces[4].vertexIndices = {4, 0, 2, 6};
    // Left face (-R)  : (1, 5, 7, 3) -> FTL, BTL, BBL, FBL
    cubePolyhedron.faces[5].vertexIndices = {1, 5, 7, 3};

    // 3. Calculate Plane and Normal for each face
    for (Face& face : cubePolyhedron.faces) {
        try {
            face.normal = calculateFaceNormal(face.vertexIndices, cubePolyhedron.vertices); // Store normal separately too
        } catch (const std::runtime_error& e) {
            // Handle degenerate face (shouldn't happen for a non-zero size cube)
            // Log error, mark face as invalid, etc.
            face.normal = glm::vec3(0.0f);
            // Consider re-throwing or logging:
            // std::cerr << "Warning: Could not calculate plane for cube face: " << e.what() << std::endl;
        }
    }

    return cubePolyhedron;
}

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

