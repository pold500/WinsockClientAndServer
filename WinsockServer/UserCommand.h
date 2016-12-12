#ifndef UserCommand_h__
#define UserCommand_h__
class UserCommand
{
public:
	virtual void execute() = 0;
	virtual ~UserCommand() {}
};

#endif // UserCommand_h__

