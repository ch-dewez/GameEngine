#include "RigidBody.h"
#include <iostream>


namespace Engine {
namespace Components {

RigidBody::RigidBody() 
: Component()
{

}

void RigidBody::start(){
    //m_transform = m_entity->getComponent<Transform>().value();
};

void RigidBody::update(float dt){
    /*std::cout << "rb" << std::endl;*/
    m_transform = m_entity->getComponent<Transform>().value();
    auto transform = m_transform.lock();

    addForce(gravity, ForceMode::Acceleration);
    /*std::cout << "m_force " << m_force.x << " " << m_force.y << " " << m_force.z << std::endl;*/

    m_acceleration += m_force / mass * dt;
    /*std::cout << "m_acceleration " << m_acceleration.x << " " << m_acceleration.y << " " << m_acceleration.z << std::endl;*/

    transform->position += m_acceleration * dt;

    m_force = {0.0, 0.0, 0.0};
};

void RigidBody::addForce(glm::vec3 force, ForceMode mode) {
    // see header file to understand
    switch (mode) {
        case ForceMode::Force: {
            m_force += force;
            break;
        }
        case ForceMode::Impulse:{
            m_acceleration += force / mass;
            break;
        }
        case ForceMode::Acceleration:{
            m_force += force * mass;
            break;
        }
        case ForceMode::VelocityChange:{
            m_acceleration += force;
            break;
        }

    }
}


}
}
