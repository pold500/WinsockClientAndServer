// TestCereal.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <string>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/archives/binary.hpp>
#include <fstream>
#include <array>
#include <vector>

struct MyRecord
{
	uint8_t x, y;
	float z;
	MyRecord():
		x(0),y(0),z(0.f)
	{
		
	}
	template<class Archive>
	void save(Archive & archive) const
	{
		archive(x, y, z);
	}

	template<class Archive>
	void load(Archive & archive)
	{
		archive(x, y, z);
	}
};

template<typename T>
struct Point3D
{
	T x;
	T y;
	T z;
	Point3D() : x(0), y(0), z(0){}
	Point3D(T _x, T _y, T _z) : x(_x), y(_y), z(_z) { }
	// This method lets cereal know which data members to serialize
	
	template<class Archive>
	void save(Archive & archive) const
	{
		archive(x, y, z);
	}

	template<class Archive>
	void load(Archive & archive)
	{
		archive(x, y, z);
	}
};

struct Polygon3D
{
	std::vector<Point3D<float>> m_polygonVertices; //for triangulated model only
	
	Polygon3D() 
	{
		m_polygonVertices.resize(3);
	}


	template<class Archive>
	void save(Archive & archive) const
	{
		archive(m_polygonVertices[0]);
		archive(m_polygonVertices[1]);
		archive(m_polygonVertices[2]);
	}

	template<class Archive>
	void load(Archive & archive)
	{
		archive(m_polygonVertices[0]);
		archive(m_polygonVertices[1]);
		archive(m_polygonVertices[2]);
	}
};

int main()
{
	{
		std::stringstream os;//(std::ios::binary);

		cereal::BinaryOutputArchive archive(os);

		MyRecord myData1, myData2;
		myData1.x = 5;
		myData1.y = 6;
		myData1.z = 7.89f;
		myData1.save(archive);
		std::string result;
		std::cout << os.str();
		result = os.str();
		//deserialize
		cereal::BinaryInputArchive bi(os);
		//bi.loadBinary(result);
		myData2.load(bi);
	}
	
	{
		std::stringstream str_stream;
		cereal::BinaryOutputArchive outputArchive(str_stream);
		Polygon3D polygon, polygon2;
		polygon.m_polygonVertices[0] = Point3D<float>(1, 2, 3);
		polygon.m_polygonVertices[1] = Point3D<float>(4, 5, 6);
		polygon.m_polygonVertices[2] = Point3D<float>(7, 8, 9);
		polygon.save(outputArchive);
		cereal::BinaryInputArchive inputArchive(str_stream);
		polygon2.load(inputArchive);

	}







	return 0;
}


