#pragma once
#ifndef GetVertexesCommand_h__
#define GetVertexesCommand_h__
#include "UserCommand.h"
#include "Helpers.h"

class GetVertexesCommand: public UserCommand
{
	SOCKET m_socket; 
	ObjFileData m_objectData;
	Helpers::ListInterval m_list;
public:
	GetVertexesCommand(const SOCKET socket, const ObjFileData& objectData, const Helpers::ListInterval& list);
	virtual void execute() override;

};

#endif // GetVertexesCommand_h__
