#ifndef SendGeometryCmd_v2_h__
#define SendGeometryCmd_v2_h__
#include "UserCommand.h"
#include "Helpers.h"

struct ObjFileData_v2;

class SendGeometryCmd_v2: public UserCommand
{
	int m_socket;
	ObjFileData_v2* m_objectData;
	Helpers::PolygonCmd m_polygonCmdParams;
public:
	SendGeometryCmd_v2(const int socket, ObjFileData_v2* objectData, const Helpers::PolygonCmd& cmdParams);
	
	virtual void execute() override;

};



#endif // SendGeometryCmd_v2_h__


