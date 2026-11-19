#pragma once

class CubeMap {
public:
	CubeMap(CubeMap&& other) noexcept;
	CubeMap& operator=(CubeMap&& other) noexcept;
	CubeMap(const CubeMap&) = delete;
	CubeMap& operator=(const CubeMap&) = delete;
	CubeMap();
	~CubeMap();

private:
	unsigned int m_CMO{};
};