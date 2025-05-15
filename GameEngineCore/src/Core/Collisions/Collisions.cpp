#include "Collisions.h"
#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <memory>
#include <optional>
#include <utility>
#include <vector>
#include "Core/Scene/Entities/Entity.h"
#include "Core/Scene/Components/Physics/Colliders.h"
#include "Core/Scene/Components/Physics/RigidBody.h"
#include "glm/fwd.hpp"

namespace Engine{
namespace Collisions {


struct Object {
    std::shared_ptr<Entity> entity;
    std::vector<std::shared_ptr<Components::Collider>> colliders;
    std::shared_ptr<Components::Transform> transform;
    std::optional<std::weak_ptr<Components::RigidBody>> rigidBody;

    
};

ContactManifold::ContactManifold()
: normal(0.0)
{
}

ContactManifold::ContactManifold(std::vector<ContactPoint> contactPoints, glm::vec3 normal, float penetration)
: normal(normal), points(contactPoints), penetration(penetration)
{
}


// idk how to call the other vec but it's the normal for contact constraint or the tangent for friction (basically the vec at which the force is applied)
float calculateEffectiveMassOneRb(float mass, glm::mat3& inertiaTensor, glm::vec3& relativePosition, glm::vec3& otherVec){
    glm::vec3 cross = glm::cross(relativePosition, otherVec);
    return 1 / ((1.0f/mass) + glm::dot(cross, (inertiaTensor * cross)));
};

float calculateEffectiveMassTwoRb(float massA, glm::mat3& inertiaTensorA, glm::vec3& relativePositionA,
                                  float massB, glm::mat3& inertiaTensorB, glm::vec3& relativePositionB,
                                  glm::vec3& otherVec){
    glm::vec3 crossA = glm::cross(relativePositionA, otherVec);
    glm::vec3 crossB = glm::cross(relativePositionB, otherVec);
    return 1 / ((1.0f/massA) + (1.0f/massB) + 
                glm::dot(crossA, (inertiaTensorA * crossA)) +
                glm::dot(crossB, (inertiaTensorB * crossB)));
};

struct PreStepInfo{
    bool oneRb = false;

    std::vector<glm::vec3> relativePositionA;
    std::vector<glm::vec3> relativePositionB;
    std::vector<float> normalEffectiveMass;
    std::vector<float> tangent1EffectiveMass;
    std::vector<float> tangent2EffectiveMass;


    void calculatePreStepInfo(Components::RigidBody* rbA, Components::RigidBody* rbB, ContactManifold& manifold){
        if (!rbA || !rbB){
            oneRb = true;
            assert(rbA);
        }

        relativePositionA.resize(manifold.points.size());
        relativePositionB.resize(manifold.points.size());
        normalEffectiveMass.resize(manifold.points.size());
        tangent1EffectiveMass.resize(manifold.points.size());
        tangent2EffectiveMass.resize(manifold.points.size());

        glm::mat3 inertiaTensorA = rbA->getinvInertiaTensor();
        glm::mat3 inertiaTensorB;
        if (!oneRb){
            inertiaTensorB = rbA->getinvInertiaTensor();
        }
        //
        for (int i=0;i<manifold.points.size();i++){

            ContactPoint& point = manifold.points[i];
            relativePositionA[i] = point.position - rbA->getWorldCenterOfMass();
            if (!oneRb){
                relativePositionB[i] = point.position - rbB->getWorldCenterOfMass();
            }

            if (oneRb){
                float mass = rbA->mass;
                normalEffectiveMass[i] = calculateEffectiveMassOneRb(mass, inertiaTensorA, relativePositionA[i], manifold.normal);
                tangent1EffectiveMass[i] = calculateEffectiveMassOneRb(mass, inertiaTensorA, relativePositionA[i], manifold.tangent.vec1);
                tangent2EffectiveMass[i] = calculateEffectiveMassOneRb(mass, inertiaTensorA, relativePositionA[i], manifold.tangent.vec2);
            }else {
                float massA = rbA->mass;
                float massB = rbB->mass;
                normalEffectiveMass[i] = calculateEffectiveMassTwoRb(massA, inertiaTensorA, relativePositionA[i],
                                                                     massB, inertiaTensorB, relativePositionB[i],
                                                                     manifold.normal);
                tangent1EffectiveMass[i] = calculateEffectiveMassTwoRb(massA, inertiaTensorA, relativePositionA[i],
                                                                       massB, inertiaTensorB, relativePositionB[i],
                                                                       manifold.tangent.vec1);
                tangent2EffectiveMass[i] = calculateEffectiveMassTwoRb(massA, inertiaTensorA, relativePositionA[i],
                                                                       massB, inertiaTensorB, relativePositionB[i],
                                                                       manifold.tangent.vec2);
            }
        }
    }
};

struct Collision {
    Object& objA;
    Object& objB;
    ContactManifold manifold;

    PreStepInfo preStep;

    float normalImpulse; // accumulated impusle (contact constraints)
    float tangent1Impulse; // accumulated impusle along tangent1 for the friction
    float tangent2Impulse; // accumulated impusle along tangent2 for the friction
};

using FindContactFunc = ContactManifold(*)(const Components::Collider*,
                                           const Components::Collider*);


std::vector<Collision> detectCollisions(std::vector<Object>& objects);
void solveCollision(std::vector<Collision>& collisions, float dt);

void ManageCollision(Scene &scene, float dt) {
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
    solveCollision(collisions, dt);
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
        point.position = surfacePointA + (surfacePointB - surfacePointA) / 2.0f;
        auto penetration = aRadius + bRadius;
        return ContactManifold({point}, normal, penetration);
    }

    if (distance > aRadius + bRadius) {
        return ContactManifold();
    }

    glm::vec3 normal = glm::normalize(ab);

    ContactPoint point;
    glm::vec3 surfacePointA = aCenter + aRadius * normal;
    glm::vec3 surfacePointB = bCenter + bRadius * -normal;
    point.position = surfacePointA + (surfacePointB - surfacePointA) / 2.0f;
    auto penetration = distance - (aRadius + bRadius);
    return ContactManifold({point}, normal, penetration);

}

glm::vec3 getOrthogonalVector(glm::vec3 vec) {
    glm::vec3 C;
    if (vec.y != 0 || vec.z != 0)
        C = glm::vec3(1.0f, 0.0f, 0.0f);
    else
        C = glm::vec3(0.0f, 1.0f, 0.0f);
    return glm::cross(vec, C);
}


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
        point.position = surfacePointA + (surfacePointB - surfacePointA) / 2.0f;
        auto penetration = SphereRadius + CapsuleRadius;
        return ContactManifold({point}, normal, penetration);
    }

    if (distance > SphereRadius + CapsuleRadius) {
        return ContactManifold();
    }

    glm::vec3 normal = glm::normalize(projSphereVec);

    ContactPoint point;
    glm::vec3 surfacePointA = SphereCenter + SphereRadius * -normal;
    glm::vec3 surfacePointB = capsuleCenterProj + CapsuleRadius * normal;
    point.position = surfacePointA + (surfacePointB - surfacePointA) / 2.0f;
    auto penetration = distance - (SphereRadius + CapsuleRadius);
    return ContactManifold({point}, normal, penetration);
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
            // No response if none has rigidbody so no need to check them
            if (!a.rigidBody.has_value() && !b.rigidBody.has_value()){
                continue;
            }
            for (std::shared_ptr<Components::Collider> colliderA : a.colliders)
            for (std::shared_ptr<Components::Collider> colliderB : b.colliders) {

                ContactManifold manifold = findCollision(
                    colliderA.get(),
                    colliderB.get()
                );

                if (manifold.points.size() > 0){
                    collisions.emplace_back(a, b, manifold);
                }
            }
        }
    }

    return std::move(collisions);
}


// TODO: Physics material
void solveCollisionOneRb(Collision& collision, float dt){
    // Ensure objA has the rigidbody
    if (collision.objB.rigidBody.has_value()) {
        std::swap(collision.objA, collision.objB);
        collision.manifold.normal *= -1.0f;
    }


    auto rb = collision.objA.rigidBody->lock();

    /*glm::mat3 worldInvInertiaTensor = localInvInertia;*/

    auto omega = rb->getOmega();
    auto velocity = rb->getCurrentVelocity();

    PreStepInfo& preStep = collision.preStep;

    for (int i=0;i<collision.manifold.points.size();i++){
        ContactPoint& point = collision.manifold.points[i];

        glm::vec3 relativeVelocity = velocity + glm::cross(omega, preStep.relativePositionA[i]);

        // friction
        {
            const float velocityAlongT1 = glm::dot(relativeVelocity, collision.manifold.tangent.vec1);
            const float velocityAlongT2 = glm::dot(relativeVelocity, collision.manifold.tangent.vec2);

            float deltaJf1_desired = (collision.preStep.tangent1EffectiveMass[i] > 0.00001f) ? -velocityAlongT1 / collision.preStep.tangent1EffectiveMass[i] : 0.0f;
            float deltaJf2_desired = (collision.preStep.tangent2EffectiveMass[i] > 0.00001f) ? -velocityAlongT2 / collision.preStep.tangent2EffectiveMass[i] : 0.0f;

            float tangent1Impulse = std::max(collision.tangent1Impulse + deltaJf1_desired, 0.f);;
            deltaJf1_desired = tangent1Impulse - collision.tangent1Impulse;
            collision.tangent1Impulse = tangent1Impulse;

            float tangent2Impulse = std::max(collision.tangent2Impulse + deltaJf2_desired, 0.f);;
            deltaJf2_desired = tangent2Impulse - collision.tangent2Impulse;
            collision.tangent2Impulse = tangent2Impulse;

            glm::vec3 impulseVecF1 = collision.manifold.tangent.vec1 * deltaJf1_desired;
            glm::vec3 impulseVecF2 = collision.manifold.tangent.vec2 * deltaJf2_desired;
            glm::vec3 totalFrictionImpulseVec = impulseVecF1 + impulseVecF2;

            float coeffictionOfFriction = 0.03f;

            rb->addForceAtPoint(totalFrictionImpulseVec * coeffictionOfFriction, point.position, Components::ForceMode::Impulse);
        }



        // contact constraint
        {
            float separatingVelocity = glm::dot(collision.manifold.normal, relativeVelocity);
            if (separatingVelocity > 0.0f) continue;;

            float baumgarteFactor = 0.2f;
            float baumgarteImpulse = baumgarteFactor * collision.manifold.penetration / dt;

            float bounciness = 0.5f;
            float desiredVelocity = (-(1.0f + bounciness) * separatingVelocity) + baumgarteImpulse;
            float lambda = desiredVelocity * preStep.normalEffectiveMass[i];

            float newImpulse = std::max(collision.normalImpulse + lambda, 0.f);;
            lambda = newImpulse - collision.normalImpulse;
            collision.normalImpulse = newImpulse;

            rb->addForceAtPoint(collision.manifold.normal * lambda, collision.manifold.points[i].position, Components::ForceMode::Impulse);

        }

    }
}

void solveCollisionBothRb(Collision& collision) {
}


void solveCollision(std::vector<Collision>& collisions, float dt) {
    const int nbIteration = 13;
    for (int iteration=0;iteration<nbIteration;iteration++){
        for (Collision& collision : collisions) {
            if (collision.objA.rigidBody.has_value() || collision.objB.rigidBody.has_value()) {
                if (collision.objB.rigidBody.has_value()) {
                    std::swap(collision.objA, collision.objB);
                    collision.manifold.normal *= -1.0f;
                }


                if (iteration == 0) {
                    collision.preStep.calculatePreStepInfo(collision.objA.rigidBody->lock().get(), nullptr, collision.manifold);
                }

                solveCollisionOneRb(collision, dt);
            }


            if (collision.objA.rigidBody.has_value() && collision.objB.rigidBody.has_value()) {
                if (iteration == 0) {
                    collision.preStep.calculatePreStepInfo(collision.objA.rigidBody->lock().get(), nullptr, collision.manifold);
                }

                solveCollisionBothRb(collision);
                continue;
            }
        }
    }
}


}
}




