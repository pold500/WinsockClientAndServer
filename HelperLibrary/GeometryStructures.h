#pragma once
#ifndef GeometryStructures_h__
#define GeometryStructures_h__

#include <vector>
#include <map>

template<typename T>
struct Point3D
{
	T x;
	T y;
	T z;
	static Point3D<float> Deserialize(const std::string& data)
	{
		Point3D<float> point;
		point.x = Helpers::deserializeFloat(data.substr(0, sizeof(float)));
		point.y = Helpers::deserializeFloat(data.substr(sizeof(float), sizeof(float)));
		point.z = Helpers::deserializeFloat(data.substr(sizeof(float)*2, sizeof(float)));
		return point;
	}
	static std::string Serialize(const Point3D<float>& point)
	{
		std::string result;
		result += Helpers::serializeFloat(point.x);
		result += Helpers::serializeFloat(point.y);
		result += Helpers::serializeFloat(point.z);
		return result;
	}
	static size_t GetSize()
	{
		return sizeof(float) * 3;
	}
};

struct Polygon3D
{
	std::array<Point3D<float>, 3> m_polygonVertices; //for triangulated model only
	static Polygon3D Deserialize(const std::string& data)
	{
		Polygon3D polygon;
		const size_t sizeOfPoint3D = Point3D<float>::GetSize();
		polygon.m_polygonVertices[0] = Point3D<float>::Deserialize(data.substr(0, sizeOfPoint3D));
		polygon.m_polygonVertices[0] = Point3D<float>::Deserialize(data.substr(sizeOfPoint3D, sizeOfPoint3D));
		polygon.m_polygonVertices[0] = Point3D<float>::Deserialize(data.substr(sizeOfPoint3D * 2, sizeOfPoint3D));
		return polygon;
	}
	size_t GetSize() const
	{
		return Point3D<float>::GetSize() * m_polygonVertices.size();
	}
	static std::string Serialize(const Polygon3D& polygon) 
	{
		std::string result;
		for (const auto& point : polygon.m_polygonVertices)
		{
			result += Point3D<float>::Serialize(point);
		}
		return result;
	}
};

struct ObjFileData {
	std::vector<Polygon3D> m_polygons;
	std::string m_object_name;
	ObjFileData(const std::string& object_name, const std::vector<Polygon3D>& polygons):
		m_object_name(object_name),
		m_polygons(polygons)
	{

	}
};
using ObjFileDataMap = std::map < std::string, std::unique_ptr<ObjFileData>>;


#endif // GeometryStructures_h__
