#include "stdafx.h"
#include "Helpers.h"
#include "GeometryStructures.h"

using namespace Helpers;

std::string Helpers::serializeInt(int intValue)
{
	char charString[4];
	memcpy(&charString[0], (char*)&intValue, sizeof(float));
	return charString;
}

int Helpers::deserializeInt(const std::string& strValue)
{
	assert(strValue.size() == sizeof(int));
	int intValue = 0;
	memcpy_s(&intValue, sizeof(int), &strValue[0], sizeof(int));
	return intValue;
}

std::string Helpers::serializeFloat(float floatValue)
{
	char charString[4];
	memcpy(&charString[0], (char*)&floatValue, sizeof(float));
	return charString;
}

float Helpers::deserializeFloat(const std::string& stringValue)
{
	assert(stringValue.size() == sizeof(float));
	float floatValue = 0.f;
	memcpy_s(&floatValue, sizeof(float), &stringValue[0], sizeof(float));
	return floatValue;
}

std::string Helpers::serializePolygonData(const std::vector<Polygon3D>& polygonsVector)
{
	std::string serializedPolygonData;
	serializedPolygonData += Helpers::serializeInt(polygonsVector.size());
	for (auto polygon : polygonsVector)
	{
		serializedPolygonData += Polygon3D::Serialize(polygon);
	}
	return std::move(serializedPolygonData);
}

void Helpers::deseralizePolygonData(const std::string& input, std::vector<Polygon3D>& polygonsVector)
{
		Polygon3D samplePolygon;
		const int numberOfPolygons = Helpers::deserializeInt(input.substr(0, sizeof(int)));
		Polygon3D::Deserialize(input.substr(sizeof(int)));
}

bool Helpers::ListInterval::parse(const std::string& input)
{
	if (input.find(',') != std::string::npos) //this is list
	{
		const auto string_vertices_list = Helpers::split(input, ",");
		for (const auto stringIndex : string_vertices_list)
		{
			vertices_list.push_back(std::stoi(stringIndex));
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
		vertices_range[0] = std::stoi(str_vertices_range.at(0));
		vertices_range[1] = std::stoi(str_vertices_range.at(1));
		m_isList = false;
	}
	else return false;

	return validate();
}
