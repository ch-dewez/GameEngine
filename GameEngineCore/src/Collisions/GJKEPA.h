//
//
//
// This followed / Copy this
// https://winter.dev/articles/gjk-algorithm
//
// And this for the EPA :
// https://cs.brown.edu/courses/cs195u/lectures/04_advancedCollisionsAndPhysics.pdf
//
// As we said there are two types of programmer the one who copy paste and the one who lie about it.
//
//



#include <glm/glm.hpp>
#include "Scene/Components/Physics/Colliders.h"

namespace Engine {
namespace Collisions {

// forward declaration of collision.h
struct ContactPoint;
struct ContactManifold;

//forward declaration of cpp
struct Simplex;

std::pair<bool, Simplex> GJK(const Components::Collider& colliderA, const Components::Collider& colliderB);
ContactManifold EPA(const Components::Collider* colliderA, const Components::Collider* colliderB);
ContactManifold EPA(Simplex& simplex, const Components::Collider& colliderA, const Components::Collider& colliderB);


}
}
