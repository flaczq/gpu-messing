#include "cube_map.h"

CubeMap::CubeMap(CubeMap&& other) noexcept
    : m_CMO(other.m_CMO) {
    other.m_CMO = 0;
}

CubeMap& CubeMap::operator=(CubeMap&& other) noexcept {
    if (this != &other) {
        m_CMO = other.m_CMO;

        other.m_CMO = 0;
    }
    return *this;
}

CubeMap::CubeMap() {
    // nothing yet
}

CubeMap::~CubeMap() {
    // delete...
}