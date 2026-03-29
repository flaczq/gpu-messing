#pragma once

#include <iostream>

#ifndef NDEBUG
	#define LOG(msg)   std::cout << "          " << msg << std::endl
	#define LOG_D(msg) std::cout << "[DEBUG]   " << msg << std::endl
	#define LOG_E(msg) std::cerr << "! [ERROR] " << msg << std::endl
#else
	#define LOG(msg)   
	#define LOG_D(msg) 
	#define LOG_E(msg) std::cerr << "! [ERROR] " << msg << std::endl
#endif