#include <glm/glm.hpp>
#include <memory>
#include "../Component.h"
#include "../Transform.h"

namespace Engine {
namespace Components {

// FORCE : directly add to m_force -> is affected by mass and dt
// IMPULSE : adding to m_acceleration but divided my mass -> is affected by mass but not dt
// ACCELERATION : adding to m_force but multiplied by mass -> mass canceled out so is affected by dt but not mass 
// VELOCITYCHANGE: adding directly to m_acceleration -> is affected by nothing
//
// Which one to chose ?
// normal force : force which happened in a period (not a single moment) and is affected by mass
// Impulse: if the force happened in a difined moment in time (me hitting a ball or a ball bouncing of somehting)
// Acceleration : if something continiously and is not affected by mass -> gravity (either you say the gravity force is multiplied by mass and you use Force or don't multiply it and use this)
// VELOCITY CHANGE : if you want to directly change the velocity
enum class ForceMode {
    Force,
    Impulse,
    Acceleration,
    VelocityChange
};

class RigidBody : public Component {
public:
    RigidBody();

    void update(float dt) override;
    void start() override;

    void addForce(glm::vec3 force, ForceMode mode = ForceMode::Force);
    void setGravity(glm::vec3 gravity);

    glm::vec3 getCurrentAcceleration() {return m_acceleration;};
    void resetAcceleration() {m_acceleration = glm::vec3(0.0f);};

public:
    float mass = 1;
    glm::vec3 gravity = {0.0, -9.81, 0.0};
private:
    glm::vec3 m_force = {0.0, 0.0, 0.0};
    glm::vec3 m_acceleration = {0.0, 0.0, 0.0};
    glm::vec3 m_angularVelocity = {0.0f, 0.0f, 0.0f};
    std::weak_ptr<Transform> m_transform;
};

}
}


