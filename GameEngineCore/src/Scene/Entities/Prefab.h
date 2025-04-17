#include "Entity.h"

namespace Engine {
namespace Entities {

class Prefab : public Entity {
public:
    Prefab(std::string name): Entity(name) {};
    virtual void load(std::shared_ptr<Entity> self)=0;
};

}
}
