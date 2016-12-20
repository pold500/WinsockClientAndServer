#include "SendGeometryCmd_v2.h"
#include "Helpers.h"
#include "GeometryStructures.h"
#undef min
#undef max

SendGeometryCmd_v2::SendGeometryCmd_v2(const int socket, ObjFileData_v2 * objectData, const Helpers::PolygonCmd& cmdParams):
	m_socket(socket),
	m_objectData(objectData),
	m_polygonCmdParams(cmdParams)
{
}

void SendGeometryCmd_v2::execute()
{
	//prepare data
	ObjFileData_v2 dataToSend;
	dataToSend.m_vertices = m_objectData->m_vertices;
	if (m_polygonCmdParams.polyList.polygons_list.size())
	{
		dataToSend.m_faces.reserve(m_polygonCmdParams.polyList.polygons_list.size());
		for (const int index : m_polygonCmdParams.polyList.polygons_list)
		{
			if (index < m_objectData->m_faces.size() && index >= 0)
			{
				dataToSend.m_faces.push_back(m_objectData->m_faces[index]);
			}
		}
	}
	else //range
	{
		const size_t lower_bound = m_polygonCmdParams.polyRange.lower_bound;
		const size_t upper_bound = m_polygonCmdParams.polyRange.upper_bound;
		if (lower_bound >= m_objectData->m_faces.size() ||
			upper_bound >= m_objectData->m_faces.size() ||
			lower_bound == upper_bound)
		{
			console_log << "failed to parse user range. \n";
			Helpers::sendPacket(m_socket, Helpers::CStringPacket("failed to parse user range"));
			return;
		}
		const size_t validated_upper_bound = std::min(upper_bound, m_objectData->m_faces.size());
		dataToSend.m_faces.insert(end(dataToSend.m_faces), begin(m_objectData->m_faces) + lower_bound,
			begin(m_objectData->m_faces) + validated_upper_bound + 1);
	}

	Helpers::CBinaryVertexDataPacket_V2 packet(&dataToSend);

	Helpers::sendPacket(m_socket, packet);
}

