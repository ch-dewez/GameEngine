//
//
// I followed this : 
// https://winter.dev/articles/physics-engine
//
// actually more this :
// https://winter.dev/articles/physics-engine/DirkGregorius_Contacts.pdf
//
//

#include "Core/Scene/Scene.h"
#include <glm/glm.hpp>
#include "GJKEPA.h"

namespace Engine {
namespace Collisions {


struct ContactPoint {
    glm::vec3 position; 
};

struct Tangent {
    glm::vec3 vec1;
    glm::vec3 vec2;
};

struct ContactManifold
{
    std::vector<ContactPoint> points;
    glm::vec3 normal;
    Tangent tangent;
    float penetration;

    ContactManifold();
    ContactManifold(std::vector<ContactPoint> contactPoints, glm::vec3 normal, float penetration);
};

void ManageCollision(Scene& scene, float dt);

}
}

