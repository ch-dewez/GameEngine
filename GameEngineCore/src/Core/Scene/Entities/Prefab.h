#pragma once
#include "Entity.h"

namespace Engine {
namespace Entities {

class Prefab : public Entity {
public:
    Prefab(std::string name): Entity(name) {};
    virtual void load()=0;
};

}
}
