#include "Collisions.h"
#include <algorithm>
#include <cmath>
#include <iostream>
#include <memory>
#include <optional>
#include <utility>
#include <vector>
#include "Scene/Entities/Entity.h"
#include "Scene/Components/Physics/Colliders.h"
#include "Scene/Components/Physics/RigidBody.h"

namespace Engine{
namespace Collisions {

struct Object {
    std::shared_ptr<Entity> entity;
    std::vector<std::shared_ptr<Components::Collider>> colliders;
    std::shared_ptr<Components::Transform> transform;
    std::optional<std::weak_ptr<Components::RigidBody>> rigidBody;
};


ContactManifold::ContactManifold()
: pointCount(0), normal(0.0)
{
}

ContactManifold::ContactManifold(int pointCount, ContactPoint contactPoints[4], glm::vec3 normal)
: pointCount(pointCount), normal(normal), points(*contactPoints)
{
}

struct Collision {
    Object& objA;
    Object& objB;
    ContactManifold manifold;
};

using FindContactFunc = ContactManifold(*)(const Components::Collider*,
                                           const Components::Collider*);


std::vector<Collision> detectCollisions(std::vector<Object>& objects);
void solveCollision(std::vector<Collision>& collisions);

void ManageCollision(Scene &scene) {
    std::vector<Object> entities; // entity with colliders
    for (auto entity : scene.getAllEntities()) {
        auto collidersWeak = entity->getComponents<Components::Collider>();
        if (collidersWeak.size() == 0)
            continue;

        std::vector<std::shared_ptr<Components::Collider>> colliders(collidersWeak.size());
        for (int i = 0; i<collidersWeak.size();i++) {
            colliders[i] = collidersWeak[i].lock();
        }

        entities.push_back(Object(entity, std::move(colliders), entity->getComponent<Components::Transform>().value().lock(), entity->getComponent<Components::RigidBody>()));
    }

    auto collisions = detectCollisions(entities);
    solveCollision(collisions);
}

ContactManifold TestSphereSphere(const Components::Collider* colliderA, const Components::Collider* colliderB) {
    Components::SphereCollider* sphereA = (Components::SphereCollider*)colliderA;
    Components::SphereCollider* sphereB = (Components::SphereCollider*)colliderB;
    glm::vec3 aCenter = sphereA->getWorldCenter();
    glm::vec3 bCenter = sphereA->getWorldCenter();
    float aRadius = sphereA->getRadius(); 
    float bRadius = sphereB->getRadius();


    glm::vec3 ab = aCenter - bCenter;
    float distance = glm::distance(aCenter, bCenter);

    if (distance < 0.00001f) {
        ContactPoint point;
        glm::vec3 normal(0.0f, 1.0f, 0.0f);
        glm::vec3 surfacePointA = aCenter + aRadius * normal;
        glm::vec3 surfacePointB = bCenter + bRadius * -normal;
        point.Position = surfacePointA + (surfacePointB - surfacePointA) / 2.0f;
        point.Penetration = aRadius + bRadius;
        return ContactManifold(1, &point, normal);
    }

    if (distance > aRadius + bRadius) {
        return ContactManifold();
    }

    glm::vec3 normal = glm::normalize(ab);

    ContactPoint point;
    glm::vec3 surfacePointA = aCenter + aRadius * normal;
    glm::vec3 surfacePointB = bCenter + bRadius * -normal;
    point.Position = surfacePointA + (surfacePointB - surfacePointA) / 2.0f;
    point.Penetration = distance - (aRadius + bRadius);
    return ContactManifold(1, &point, normal);

}

// https://gamedev.stackexchange.com/questions/44500/how-many-and-which-axes-to-use-for-3d-obb-collision-with-sat
/*bool Separated(std::vector<glm::vec3>& vertsA, std::vector<glm::vec3> vertsB, glm::vec3 axis)*/
/*{*/
/*    // Handles the cross product = {0,0,0} case*/
/*    if (axis == glm::vec3(0))*/
/*        return false;*/
/**/
/*    float aMin = std::numeric_limits<float>::max();*/
/*    float aMax = -std::numeric_limits<float>::max();*/
/*    float bMin = std::numeric_limits<float>::max();*/
/*    float bMax = -std::numeric_limits<float>::max();*/
/**/
/*    // Define two intervals, a and b. Calculate their min and max values*/
/*    for (int i = 0; i < vertsA.size(); i++)*/
/*    {*/
/*        float aDist = glm::dot(vertsA[i], axis);*/
/*        aMin = aDist < aMin ? aDist : aMin;*/
/*        aMax = aDist > aMax ? aDist : aMax;*/
/*    }*/
/**/
/*    for (int i = 0; i < vertsB.size(); i++)*/
/*    {*/
/*        float bDist = glm::dot(vertsB[i], axis);*/
/*        bMin = bDist < bMin ? bDist : bMin;*/
/*        bMax = bDist > bMax ? bDist : bMax;*/
/*    }*/
/**/
/*    // One-dimensional intersection test between a and b*/
/*    float longSpan = std::max(aMax, bMax) - std::max(aMin, bMin);*/
/*    float sumSpan = aMax - aMin + bMax - bMin;*/
/*    return longSpan >= sumSpan; // > to treat touching as intersection*/
/*}*/

glm::vec3 getOrthogonalVector(glm::vec3 vec) {
    glm::vec3 C;
    if (vec.y != 0 || vec.z != 0)
        C = glm::vec3(1.0f, 0.0f, 0.0f);
    else
        C = glm::vec3(0.0f, 1.0f, 0.0f);
    return glm::cross(vec, C);
}


/*ContactManifold TestCubeCube(const Components::Collider* colliderA, const Components::Collider* colliderB) {*/
/*    Components::CubeCollider* cubeA = (Components::CubeCollider*)colliderA;*/
/*    Components::CubeCollider* cubeB = (Components::CubeCollider*)colliderB;*/
/**/
/*    auto axes = cubeA->getAllAxisSAT(*cubeB);  // axes = plural of axis (that's weird)*/
/*    auto cubeAVertices = cubeA->getAllVertices();*/
/*    auto cubeBVertices = cubeB->getAllVertices();*/
/**/
/*    for (auto axis : axes) {*/
/*        bool separated = Separated(cubeBVertices, cubeBVertices, axis);*/
/*    }*/
/*}*/
/**/

ContactManifold TestSphereCapsule(const Components::Collider* colliderA, const Components::Collider* colliderB) {
    Components::SphereCollider* sphere = (Components::SphereCollider*)colliderA;
    Components::CapsuleCollider* capsule = (Components::CapsuleCollider*)colliderB;
    glm::vec3 SphereCenter = sphere->getWorldCenter();
    glm::vec3 CapsuleCenter1 = capsule->getWorldCenter1();
    glm::vec3 CapsuleCenter2 = capsule->getWorldCenter2();
    float SphereRadius = sphere->getRadius(); 
    float CapsuleRadius = capsule->getRadius();

    glm::vec3 bottomSphereVec = CapsuleCenter2 - SphereCenter;
    glm::vec3 bottomTopVec = CapsuleCenter1 - CapsuleCenter2;
    glm::vec3 normBottomTop = glm::normalize(bottomTopVec);

    glm::vec3 capsuleCenterProj = CapsuleCenter2 + normBottomTop * glm::clamp(glm::dot(normBottomTop, bottomSphereVec), 0.0f, glm::length(bottomTopVec));

    glm::vec3 projSphereVec = SphereCenter - capsuleCenterProj;
    float distance = glm::length(projSphereVec);


    if (distance < 0.00001f) {
        ContactPoint point;
        glm::vec3 normal = glm::normalize(getOrthogonalVector(bottomSphereVec));
        glm::vec3 surfacePointA = capsuleCenterProj + CapsuleRadius * normal;
        glm::vec3 surfacePointB = SphereCenter + SphereRadius * -normal;
        point.Position = surfacePointA + (surfacePointB - surfacePointA) / 2.0f;
        point.Penetration = SphereRadius + CapsuleRadius;
        return ContactManifold(1, &point, normal);
    }

    if (distance > SphereRadius + CapsuleRadius) {
        return ContactManifold();
    }

    glm::vec3 normal = glm::normalize(projSphereVec);

    ContactPoint point;
    glm::vec3 surfacePointA = SphereCenter + SphereRadius * -normal;
    glm::vec3 surfacePointB = capsuleCenterProj + CapsuleRadius * normal;
    point.Position = surfacePointA + (surfacePointB - surfacePointA) / 2.0f;
    point.Penetration = distance - (SphereRadius + CapsuleRadius);
    return ContactManifold(1, &point, normal);
}

ContactManifold findCollision(const Components::Collider* a,
                              const Components::Collider* b) {
    static const FindContactFunc tests[3][3] = 
        {
            // Sphere             Cube              Capsule
            { TestSphereSphere, EPA,  TestSphereCapsule }, // Sphere
            { nullptr,          EPA,       EPA },  // Cube 
            { nullptr,          nullptr,            nullptr }  // Capsule 
        };

    bool swap = b->type > a->type;

    if (swap)
    {
        std::swap(a, b);
    }

    ContactManifold points = tests[a->type][b->type](a, b);


    return points;
}

std::vector<Collision> detectCollisions(std::vector<Object>& objects) {
    std::vector<Collision> collisions;
    for (int i = 0 ;i<objects.size();i++) {
        Object& a = objects[i];
        // we start at i+1 like that we only have unique pairs (if i then i==j and entity == entity)
        for (int j=i+1;j<objects.size();j++) {
            Object& b = objects[j];
            for (std::shared_ptr<Components::Collider> colliderA : a.colliders)
            for (std::shared_ptr<Components::Collider> colliderB : b.colliders) {

                ContactManifold points = findCollision(
                    colliderA.get(),
                    colliderB.get()
                );

                if (points.pointCount > 0){
                    /*std::cout << "Collisions Detected between " << a.entity->name << " and " << b.entity->name << std::endl;*/
                    /*std::cout << "depth " << points.points[0].Penetration;*/
                    /*std::cout << " and position " << points.points[0].Position.x<< " " << points.points[0].Position.y << " " << points.points[0].Position.z;*/
                    /*std::cout << " normal " << points.normal.x << " "<< points.normal.y << " "<< points.normal.z << std::endl;*/

                    collisions.emplace_back(a, b, points);
                }
            }
        }
    }

    return std::move(collisions);
}

void solveCollisionOneRb(Collision& collision){
    // always objA that have the rigidbody
    if (collision.objB.rigidBody.has_value()) {
        /*std::cout << "swapping" << std::endl;*/
        std::swap(collision.objA, collision.objB);
        collision.manifold.normal *= -1.0f;
    }
    collision.manifold.normal *= -1;

    //moving the object in bound
    collision.objA.transform->position += collision.manifold.normal * collision.manifold.points[0].Penetration;

    auto rb = collision.objA.rigidBody->lock();
    auto acceleration = rb->getCurrentAcceleration();
    auto normalizedAccel = glm::normalize(acceleration);
    rb->resetAcceleration();

    // getting reflection vector : https://math.stackexchange.com/questions/13261/how-to-get-a-reflection-vector
        //𝑟=𝑑−2(𝑑⋅𝑛)𝑛
    auto reflected = normalizedAccel - 2 * glm::dot(normalizedAccel, collision.manifold.normal) * collision.manifold.normal;

    auto force = reflected * glm::length(acceleration) / rb->mass;

    rb->addForce(force, Components::ForceMode::Impulse);
}

void solveCollisionBothRb(Collision& collision) {

}


void solveCollision(std::vector<Collision>& collisions) {
    for (auto collision : collisions) {
        glm::vec3 delta = collision.manifold.normal * collision.manifold.points[0].Penetration;
        if (collision.objA.rigidBody.has_value() && collision.objB.rigidBody.has_value()) {
            solveCollisionBothRb(collision);
            continue;
        }
        if (collision.objA.rigidBody.has_value() || collision.objB.rigidBody.has_value()) {
            solveCollisionOneRb(collision);
            continue;
        }
    }
}


}
}




