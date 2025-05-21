//
//
//
// https://graphics.stanford.edu/courses/cs448b-00-winter/papers/phys_model.pdf
// AND
// https://www.youtube.com/watch?v=4r_EvmPKOvY&list=PLCXN4kwf4S4JjPR86LWU5ThVsGsKdiOGv&index=4
//
//
// Later :
// https://people.eecs.berkeley.edu/~jfc/mirtich/massProps.html
//
//
//
//

#include "../Component.h"
#include "../Transform.h"
#include "Core/Ressources/Mesh.h"
#include <glm/glm.hpp>
#include <memory>

namespace Engine {
namespace Components {

// FORCE : directly add to m_force -> is affected by mass and dt
// IMPULSE : adding to m_acceleration but divided my mass -> is affected by mass
// but not dt ACCELERATION : adding to m_force but multiplied by mass -> mass
// canceled out so is affected by dt but not mass VELOCITYCHANGE: adding
// directly to m_acceleration -> is affected by nothing
//
// Which one to chose ?
// normal force : force which happened in a period (not a single moment) and is
// affected by mass Impulse: if the force happened in a difined moment in time
// (me hitting a ball or a ball bouncing of somehting) Acceleration : if
// something continiously and is not affected by mass -> gravity (either you say
// the gravity force is multiplied by mass and you use Force or don't multiply
// it and use this) VELOCITY CHANGE : if you want to directly change the
// velocity
enum class ForceMode { Force, Impulse, Acceleration, VelocityChange };

class RigidBody : public Component {
public:
  RigidBody(Ressources::Mesh *mesh);
  // com offset
  RigidBody(glm::vec3 centerOfMass, glm::vec3 bodyInv);

  static glm::vec3 InvInertiaCuboidDensity(float forwardHalfSize,
                                           float upHalfSize,
                                           float righHalfSize);

  void update(float dt) override;
  void start() override;

  void updatePositionAndRotations(float dt);

  void addForce(glm::vec3 force, ForceMode mode = ForceMode::Force, bool recompute = true);
  void addTorque(glm::vec3 torque, ForceMode mode, bool recompute = true);

  void addForceAtPoint(glm::vec3 force, glm::vec3 point, ForceMode mode=ForceMode::Force, bool recompute=true);
  void addForceAtBodyPoint(const glm::vec3 &force, const glm::vec3 &point,
                           ForceMode mode);
  glm::vec3 getPointInWorldSpace(const glm::vec3 &point) const;

  void setGravity(glm::vec3 value) { gravity = value; };

    void recomputeVariables();

  glm::vec3 getOmega() { return m_Omega; };
  glm::mat3 getinvInertiaTensor() { return m_invInertiaTensor; };

  glm::vec3 getCenterOfMass() { return m_centerOfMass; };
  glm::vec3 getWorldCenterOfMass();

  glm::vec3 getCurrentVelocity() { return m_velocity; };
  void resetVelocity() { m_velocity = glm::vec3(0.0f); };

public:
  float mass = 1;
  glm::vec3 gravity = {0.0, -9.81, 0.0};

private:
  void clearAccumulators();

private:
  glm::vec3 m_centerOfMass;
  glm::vec3 m_IbodyInv;

  glm::mat3 m_invInertiaTensor;

  glm::vec3 m_forceAccum = {0.0, 0.0, 0.0};
  glm::vec3 m_velocity = {0.0, 0.0, 0.0};

  glm::vec3 m_torqueAccum = {
      0.0f, 0.0f, 0.0f}; // = axis of rotation / length = magnitude of torque
  glm::vec3 m_angularMomentum; // L(t)
  glm::vec3 m_Omega = {0.0f, 0.0f, 0.0f};

  Transform* m_transform;
};

} // namespace Components
} // namespace Engine
