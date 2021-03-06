﻿#ifndef GeometryStructures_h__
#define GeometryStructures_h__

#include <vector>
#include <map>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

template<typename T>
struct Point3D
{
	T x;
	T y;
	T z;
	const T getX() const { return x; }
	const T getY() const { return y; }
	const T getZ() const { return z; }
	inline Point3D() {}
	inline Point3D(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
	// This method lets cereal know which data members to serialize
	template<class Archive>
	void serialize(Archive & archive)
	{
		archive(x, y, z); // serialize things by passing them to the archive
	}
	void debugPrint() const
	{
		print("Point3D");
		printVar(x);
		printVar(y);
		printVar(z);
	}
	std::string toString() const
	{
		std::stringstream result;
		result << "Point3D \n";
		result << (x) << "\n";
		result << (y) << "\n";
		result << (z) << "\n";
		return result.str();
	}
};
template<class T>
static bool operator<(const Point3D<T>& lhs, const Point3D<T>& rhs)
{
	// sqrt( x2 + y2 )
	return (lhs.x != rhs.x) || (lhs.y != rhs.y) || (lhs.z != rhs.z);
}

template<class T>
static bool operator==(const Point3D<T>& lhs, const Point3D<T>& rhs)
{
	// sqrt( x2 + y2 )
	return (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z);
}

struct Polygon3D
{
	std::array<Point3D<float>, 3> m_polygonVertices; //for triangulated model only
	size_t m_polyIndex;
	Polygon3D() {}
	Polygon3D(size_t polyIndex) : m_polyIndex(polyIndex), m_polygonVertices({}) {}

	template<class Archive>
	void serialize(Archive & archive)
	{
		archive(m_polyIndex);
		archive(m_polygonVertices[0]);
		archive(m_polygonVertices[1]);
		archive(m_polygonVertices[2]);
	}
	std::string toString() const
	{
		std::stringstream result;
		result << "Polygon3D \n";
		result << "polyIndex " << m_polyIndex << "\n";
		for (const auto& vertex : m_polygonVertices)
		{
			result << vertex.toString() << "\n";
		} 
		return result.str();
	}
	void debugPrint() const
	{
		print("Polygon3D");
		printVar(m_polyIndex);
		for (const auto& vertex : m_polygonVertices)
		{
			vertex.debugPrint();
		}
	}
};

struct ObjFileData {
	std::vector<Polygon3D> m_polygons;
	std::string m_object_name;
	inline ObjFileData() {}
	ObjFileData(const std::string& object_name, const std::vector<Polygon3D>& polygons):
		m_object_name(object_name),
		m_polygons(polygons)
	{

	}
	
	template<class Archive>
	void load(Archive & archive)
	{
		archive(m_object_name); // serialize things by passing them to the archive
		archive(m_polygons);
	}
	template<class Archive>
	void save(Archive & archive)
	{
		archive(m_object_name); // serialize things by passing them to the archive
		archive(m_polygons);
	}
	void debugPrint() const
	{
		printVar(m_object_name);
		printVar(m_polygons.size());
		for (const auto& polygon : m_polygons)
		{
			polygon.debugPrint();
		}
	}
};

struct ObjFileData_v2 {
	std::vector<Point3D<float>> m_vertices;
	std::vector<std::vector<int>> m_faces;
	inline ObjFileData_v2() {}
	ObjFileData_v2(const std::vector<Point3D<float>>& vertices,
		const std::vector<std::vector<int>> & faces) :
		m_vertices(vertices),
		m_faces(faces)
	{

	}
	template<class Archive>
	void save(Archive & archive) const
	{
		archive(m_vertices);
		archive(m_faces);
	}

	template<class Archive>
	void load(Archive & archive)
	{
		archive(m_vertices);
		archive(m_faces);
	}
};

using ObjFileDataMap = std::map < std::string, std::unique_ptr<ObjFileData>>;
using ObjFileDataMap_v2 = std::map < std::string, std::unique_ptr<ObjFileData_v2>>;

#endif // GeometryStructures_h__
