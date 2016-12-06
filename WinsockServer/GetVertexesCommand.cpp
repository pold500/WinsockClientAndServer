#include "stdafx.h"
#include "GetVertexesCommand.h"


GetVertexesCommand::GetVertexesCommand(const SOCKET socket, const ObjFileData& objectData,
	const Helpers::ListInterval& list) :
	m_socket(socket),
	m_objectData(objectData),
	m_list(list)
{
}

void GetVertexesCommand::execute()
{
	//Serialize polygon data
	auto polygonData = Helpers::serializePolygonData(m_objectData.m_polygons);
	Helpers::sendPacket(m_socket, polygonData);

}
