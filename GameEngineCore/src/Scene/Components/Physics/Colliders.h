// collider components will just hold information like radius and all that
#pragma once
#include "../Component.h"
#include "../Transform.h"
#include <array>
#include <glm/glm.hpp>

namespace Engine {
namespace Components {

enum ColliderType {
    Sphere,
    Cube,
    Capsule
};

struct Collider : Component {
    Collider(ColliderType type):Component(), type(type) {};
    ColliderType type;

    virtual glm::vec3 findFurthestPoint(glm::vec3 dir)const =0;
};

struct CubeCollider : Collider {
    CubeCollider() : Collider(ColliderType::Cube){};

    glm::vec3 getWorldCenter() const;

    glm::vec3 findFurthestPoint(glm::vec3 dir) const override;

    std::vector<glm::vec3> getAllVertices() const;
private:
    glm::vec3 center = glm::vec3(0.0f);

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    glm::vec3 right = glm::vec3(1.0f, 0.0f, 0.0f);
    glm::vec3 forward = glm::vec3(0.0f, 0.0f, 1.0f);

    float upHalfSize = 0.5f;
    float rightHalfSize = 0.5f;
    float forwardHalfSize = 0.5f;
};

struct SphereCollider: Collider {
    SphereCollider() : Collider(ColliderType::Sphere){};
    glm::vec3 getWorldCenter() const;
    float getRadius() const;

    glm::vec3 findFurthestPoint(glm::vec3 dir) const override;
private:
    glm::vec3 center;
    float radius;
};

struct CapsuleCollider: Collider {
    CapsuleCollider() : Collider(ColliderType::Capsule){};
    glm::vec3 getWorldCenter1() const;
    glm::vec3 getWorldCenter2() const;

    float getRadius() const;

    glm::vec3 findFurthestPoint(glm::vec3 dir) const override;
private:
    glm::vec3 center;
    glm::vec3 center2;
    float radius;

};


}
}




