#include "stdafx.h"
#include "GetVertexesCommand.h"

SendGeometryCmd::SendGeometryCmd(const SOCKET socket, const ObjFileData& objectData,
	const Helpers::ListInterval& list) :
	m_socket(socket),
	m_objectData(objectData),
	m_cmdParameters(list)
{
}

void SendGeometryCmd::execute()
{
	//Serialize polygon data
	ObjFileData fileDataToSend;
	fileDataToSend.m_object_name = m_objectData.m_object_name;
	auto& polygonsToReturn = fileDataToSend.m_polygons;

	if (m_cmdParameters.m_isList)
	{
		for(auto polygonIndex : m_cmdParameters.polygons_list)
		{
			polygonsToReturn.push_back(m_objectData.m_polygons[polygonIndex]);
		}
	}
	else
	{
		const size_t lower_bound = m_cmdParameters.polygons_range.lower_bound;
		const size_t upper_bound = m_cmdParameters.polygons_range.upper_bound;
		if (lower_bound == -1 || upper_bound == -1 || lower_bound == m_objectData.m_polygons.size() ||
			upper_bound == m_objectData.m_polygons.size() || lower_bound == upper_bound)
		{
			console_log << "failed to parse user range. \n";
			return;
		}
		polygonsToReturn.insert(end(polygonsToReturn), begin(m_objectData.m_polygons) + lower_bound, 
				begin(m_objectData.m_polygons) + upper_bound);
	}

	std::stringstream str_stream;
	cereal::BinaryOutputArchive outputArchive(str_stream);
	fileDataToSend.save(outputArchive);
	Helpers::sendPacket(m_socket, str_stream.str());
}
