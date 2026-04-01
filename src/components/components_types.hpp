#pragma once

class ComponentCounter {
    static inline unsigned int m_counter = 0;

public:
    template <typename T>
    static unsigned int getTypeID() {
        static unsigned int id = m_counter++;
        return id;
    }
};