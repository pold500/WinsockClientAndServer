#ifndef LogStream_h__
#define LogStream_h__
#include <mutex>
#include <iostream>
class LogStream
{
	std::ostream* myStream;
public:
	inline LogStream() : myStream(&std::cout)
	{}

	template <typename T>
	friend LogStream& operator<<(LogStream& ls, const T& obj)
	{
		static std::mutex m_print_output_mutex;
		std::lock_guard<std::mutex> lock(m_print_output_mutex);
		{
			*(ls.myStream) << obj;
		}
		return ls;
	}
};

LogStream& GetLogStream();

#define console_log GetLogStream()

#define printVar(x) console_log << #x << " : " << x << "\n";

#define print(x)    console_log << x << "\n";

#endif // LogStream_h__

