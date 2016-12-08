#ifndef GeometryStructures_h__
#define GeometryStructures_h__

#include <vector>
#include <map>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/archives/json.hpp>

#define console_log Helpers::GetLogStream()

#define printVar(x) console_log << ##x << " : " << x << "\n";

#define print(x)    console_log << x << "\n";



template<typename T>
struct Point3D
{
	T x;
	T y;
	T z;
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
};

struct Polygon3D
{
	std::array<Point3D<float>, 3> m_polygonVertices; //for triangulated model only
	
	template<class Archive>
	void serialize(Archive & archive)
	{
		archive(m_polygonVertices[0]);
		archive(m_polygonVertices[1]);
		archive(m_polygonVertices[2]);
	}
	void debugPrint() const
	{
		print("Polygon3D");
		for (const auto& vertex : m_polygonVertices)
		{
			vertex.debugPrint();
		}
	}
};

struct ObjFileData {
	std::vector<Polygon3D> m_polygons;
	std::string m_object_name;
	ObjFileData() {}
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
		for (const auto& polygon : m_polygons)
		{
			polygon.debugPrint();
		}
	}
};
using ObjFileDataMap = std::map < std::string, ObjFileData>;


#endif // GeometryStructures_h__
