
#ifndef MARMALADE_COMPONENT_H
#define MARMALADE_COMPONENT_H

#include "spdlog/spdlog.h"

#include <typeindex>

class Entity;

class Component {
public:
    std::string name;

    virtual void Display(Entity* entity) = 0;
    virtual void Apply(Entity *entity) = 0;

    static std::unordered_map<std::type_index , std::string>& GetRegisteredComponents() {
        static std::unordered_map<std::type_index, std::string> registry;
        return registry;
    }

    static void RegisterComponent(std::type_index type, const std::string& name) {
        GetRegisteredComponents()[type] = name;
    }

    virtual ~Component() = default;
};

// THANKS JAY FOR THE HELP WITH MACROS
#define REGISTER_COMPONENT(TYPE) \
    static bool _registered; \
    static bool Register() { \
    Component::RegisterComponent(typeid(TYPE), #TYPE); \
    return true; \
    }

#endif //MARMALADE_COMPONENT_H
