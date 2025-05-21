#include "RigidBody.h"
#include "glm/gtx/quaternion.hpp"
#include <iostream>

namespace Engine {
namespace Components {

RigidBody::RigidBody(glm::vec3 centerOfMass, glm::vec3 bodyInv)
    : Component(), m_IbodyInv(bodyInv), m_centerOfMass(centerOfMass) {}

glm::vec3 RigidBody::InvInertiaCuboidDensity(float forwardSize, float upSize,
                                             float righSize) {
  float volume = forwardSize * upSize * righSize;
  return std::move(
      1.0f /
                   ((volume * glm::vec3(upSize * upSize + righSize*righSize,
                          forwardSize * forwardSize + righSize * righSize,
                          forwardSize * forwardSize + upSize * upSize)/12.0f)));
};

glm::mat3 diagonal(glm::vec3 vec) {
  glm::mat3 mat(0.0f);
  for (int i = 0; i < 3; i++) {
    for (int j = 0; j < 3; j++) {
      if (i != j) {
        continue;
      }
      mat[i][j] = vec[i];
    }
  }
  return std::move(mat);
};

void RigidBody::updatePositionAndRotations(float dt){


  // linear thing
  auto m_acceleration = m_forceAccum / mass;
  m_velocity += m_acceleration * dt;
  auto positionDelta = m_velocity  * dt;
  m_transform->position += positionDelta;

  // angular :(
  m_angularMomentum += m_torqueAccum * dt;

  recomputeVariables();

  glm::quat qDot = 0.5f * (glm::quat(0.0f, m_Omega) * m_transform->rotation) * dt;

  m_transform->rotation += qDot;
  m_transform->rotation = glm::normalize(m_transform->rotation);
};

void RigidBody::start(){
  m_transform = m_entity->getComponent<Transform>().value();
}

void RigidBody::update(float dt) {
  // removing gravity for implementing non-constraint rigidbody simpulation
  addForce(gravity, ForceMode::Acceleration);

  updatePositionAndRotations(dt);

  clearAccumulators();
};

void RigidBody::recomputeVariables() {
  glm::mat3 rotationMatrix = m_transform->getRotationMatrix();
  m_invInertiaTensor =
      rotationMatrix * diagonal(m_IbodyInv) * glm::transpose(rotationMatrix);
  m_Omega = m_invInertiaTensor * m_angularMomentum;
};

glm::vec3 RigidBody::getWorldCenterOfMass() {

  return m_transform->transform(m_centerOfMass);
};

void RigidBody::addForceAtPoint(glm::vec3 force, glm::vec3 point,
                                ForceMode mode, bool recompute) {

  auto pt = point - getWorldCenterOfMass();

  addTorque(glm::cross(pt, force), mode, false);
  addForce(force, mode, false);
    if (recompute){
        recomputeVariables();
    }

}

void RigidBody::addForceAtBodyPoint(const glm::vec3 &force,
                                    const glm::vec3 &point, ForceMode mode) {
  glm::vec3 pt = getPointInWorldSpace(point);
  addForceAtPoint(force, pt, mode);
}

glm::vec3 RigidBody::getPointInWorldSpace(const glm::vec3 &point) const {
  return m_transform->transform(point);
}

void RigidBody::addTorque(glm::vec3 torque, ForceMode mode, bool recompute) {

  switch (mode) {
  case ForceMode::Force: {
    m_torqueAccum += torque;
    break;
  }
  case ForceMode::Impulse: {
    m_angularMomentum += torque;
    break;
  }
  case ForceMode::Acceleration: {
    m_torqueAccum += glm::inverse(m_invInertiaTensor) * torque;
    break;
  }
  case ForceMode::VelocityChange: {
    m_angularMomentum += glm::inverse(m_invInertiaTensor) / torque;
    break;
  }
  }

    if (recompute){
        recomputeVariables();
    }
}

void RigidBody::addForce(glm::vec3 force, ForceMode mode, bool recompute) {
    // see header file to understand
    switch (mode) {
        case ForceMode::Force: {
            m_forceAccum += force;
            break;
        }
        case ForceMode::Impulse: {
            m_velocity += force / mass;
            break;
        }
        case ForceMode::Acceleration: {
            m_forceAccum += force * mass;
            break;
        }
        case ForceMode::VelocityChange: {
            m_velocity += force;
            break;
        }
    }
    if (recompute){
        recomputeVariables();
    }
}

void RigidBody::clearAccumulators() {
  m_forceAccum = {0.0, 0.0, 0.0};
  m_torqueAccum = {0.0, 0.0, 0.0};
}

} // namespace Components
} // namespace Engine
