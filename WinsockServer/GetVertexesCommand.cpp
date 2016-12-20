#include "stdafx.h"
#include "GetVertexesCommand.h"
#include "GeometryStructures.h"

#include "cereal/archives/binary.hpp"


SendGeometryCmd::SendGeometryCmd(const SOCKET socket, const ObjFileData& objectData, 
	const Helpers::PolygonCmd& polygonCmd
	) :
	m_socket(socket),
	m_objectData(objectData),
	m_cmdParameters(polygonCmd)
{
}
 
void SendGeometryCmd::execute()
{
	//Serialize polygon data
	std::vector<Polygon3D> polygonsToReturn;

	if (m_cmdParameters.polyList.polygons_list.size())
	{
		for(auto polygonIndex : m_cmdParameters.polyList.polygons_list)
		{
			if ((size_t)polygonIndex < m_objectData.m_polygons.size())
			{
				polygonsToReturn.push_back(m_objectData.m_polygons[polygonIndex]);
			}
		}
	}
	else 
	{
		const size_t lower_bound = m_cmdParameters.polyRange.lower_bound;
		const size_t upper_bound = m_cmdParameters.polyRange.upper_bound;
		if ( lower_bound >= m_objectData.m_polygons.size() ||
			 upper_bound >= m_objectData.m_polygons.size() ||
			 lower_bound == upper_bound )
		{
			console_log << "failed to parse user range. \n";
			Helpers::sendPacket(m_socket, Helpers::CStringPacket("failed to parse user range"));
			return;
		}
		const size_t validated_upper_bound = std::min(upper_bound, m_objectData.m_polygons.size());
		polygonsToReturn.insert(end(polygonsToReturn), begin(m_objectData.m_polygons) + lower_bound, 
				begin(m_objectData.m_polygons) + validated_upper_bound + 1);
	} 
	
	Helpers::CBinaryVertexDataPacket polygonDataPacket(polygonsToReturn);
	
	Helpers::sendPacket(m_socket, polygonDataPacket);
}
