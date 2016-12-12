#ifndef GetVertexesCommand_h__
#define GetVertexesCommand_h__
#include "UserCommand.h"
#include "Helpers.h"
#include "GeometryStructures.h"

class SendGeometryCmd: public UserCommand
{
	SOCKET m_socket; 
	ObjFileData m_objectData;
	Helpers::PolygonCmd m_cmdParameters;
public:
	SendGeometryCmd(const SOCKET socket, const ObjFileData& objectData, const Helpers::PolygonCmd& polygonCmd);
	virtual void execute();

};

#endif // GetVertexesCommand_h__
