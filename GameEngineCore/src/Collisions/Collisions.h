//
//
// I followed this : 
// https://winter.dev/articles/physics-engine
//
// actually more this :
// https://winter.dev/articles/physics-engine/DirkGregorius_Contacts.pdf
//
//

#include "Scene/Scene.h"
#include <glm/glm.hpp>
#include "GJKEPA.h"

namespace Engine {
namespace Collisions {


struct ContactPoint {
    glm::vec3 Position; 
    float Penetration;
};

struct ContactManifold
{
    int pointCount;
    ContactPoint points[4];
    glm::vec3 normal;

    ContactManifold();
    ContactManifold(int pointCount, ContactPoint contactPoints[4], glm::vec3 normal);
};

void ManageCollision(Scene& scene);

}
}

