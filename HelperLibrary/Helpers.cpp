#include "stdafx.h"
#include "Helpers.h"
#include "GeometryStructures.h"

using namespace Helpers;


bool Helpers::ListInterval::parse(const std::string& input)
{
	if (input.find(',') != std::string::npos) //this is list
	{
		const auto string_vertices_list = Helpers::split(input, ",");
		for (const auto stringIndex : string_vertices_list)
		{
			polygons_list.push_back(std::stoi(stringIndex));
		}
		m_isList = true;
	}
	else if (input.find("..") != std::string::npos) //this is range
	{
		const auto str_vertices_range = Helpers::split(input, "..");
		if (str_vertices_range.size() != 2)
		{
			//wrong format
			return false;
		}
		polygons_range.lower_bound = std::stoi(str_vertices_range.at(0));
		polygons_range.upper_bound = std::stoi(str_vertices_range.at(1));
		m_isList = false;
	}
	else
	{
		return false;
	}

	return validate();
}
