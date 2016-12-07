#include "stdafx.h"
#include "GetVertexesCommand.h"

SendGeometryCmd::SendGeometryCmd(const SOCKET socket, const ObjFileData& objectData,
	const Helpers::ListInterval& list) :
	m_socket(socket),
	m_objectData(objectData),
	m_list(list)
{
}

void SendGeometryCmd::execute()
{
	//Serialize polygon data
	std::stringstream str_stream;
	cereal::BinaryOutputArchive outputArchive(str_stream);
	m_objectData.save(outputArchive);
	Helpers::sendPacket(m_socket, str_stream.str());
	//Test it
	//cereal::BinaryInputArchive inputArchive(str_stream);
	//ObjFileData  fd2;
	//fd2.load(inputArchive);
}
