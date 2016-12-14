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
	ObjFileData fileDataToSend;
	fileDataToSend.m_object_name = m_objectData.m_object_name;
	auto& polygonsToReturn = fileDataToSend.m_polygons;

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
			Helpers::sendPacket(m_socket, "error: Wrong range! Specify range carefully.");
			return;
		}
		const size_t validated_upper_bound = std::min(upper_bound, m_objectData.m_polygons.size());
		polygonsToReturn.insert(end(polygonsToReturn), begin(m_objectData.m_polygons) + lower_bound, 
				begin(m_objectData.m_polygons) + validated_upper_bound + 1);
	} 
	std::stringstream str_stream;
	for (const auto& polygon : polygonsToReturn)
	{
		str_stream << polygon.toString() << "\n";
	}
	Helpers::sendPacket(m_socket, str_stream.str());
}
