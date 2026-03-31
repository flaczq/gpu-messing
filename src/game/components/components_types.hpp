#pragma once

class ComponentCounter {
public:
    static unsigned int getNextID() {
        static unsigned int lastID = 0u;
        return lastID++;
    }
};

template <typename T>
class ComponentType {
public:
    static unsigned int getID() {
        static unsigned int typeID = ComponentCounter::getNextID();
        return typeID;
    }
};