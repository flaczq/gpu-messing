#pragma once

#include <cstdint>

using ComponentTypeID = std::uint32_t;

class ComponentID {
public:
    template <typename T>
    static ComponentTypeID get() noexcept {
        static const ComponentTypeID ID = getNextID();
        return ID;
    }

private:
    static ComponentTypeID getNextID() noexcept {
        static ComponentTypeID lastID = 0;
        return lastID++;
    }
};

struct IComponent {
    virtual ~IComponent() = default;
};