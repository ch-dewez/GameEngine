#include "Entity.h"
#include "Core/Scene/Components/Component.h"
#include "Core/Log/Log.h"

namespace Engine {

Entity::Entity(std::string name)
: name(name)
{
    uuid.generate_uuid_v4();
}

Entity::~Entity(){
    LogDebug("calling entity deconstrutor");
}


}
