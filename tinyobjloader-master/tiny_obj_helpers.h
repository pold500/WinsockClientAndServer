#ifndef _tiny_helpers_
#define _tiny_helpers_
#pragma once
#include <cassert>

void PrintInfo(const tinyobj::attrib_t& attrib,
	const std::vector<tinyobj::shape_t>& shapes,
	const std::vector<tinyobj::material_t>& materials);

#endif