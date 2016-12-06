#pragma once
#ifndef LogStream_h__
#define LogStream_h__
#include <mutex>

class LogStream
{
	std::ostream* myStream;
public:
	LogStream();

	//template<typename T>
	//inline LogStream & operator<<(/*LogStream & ls,*/ const T & obj)
	//{
	//	static std::mutex m_print_output_mutex;
	//	std::lock_guard<std::mutex> lock(m_print_output_mutex);
	//	{
	//		*(this->myStream) << obj;
	//	}
	//	return *this;
	//}

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


#endif // LogStream_h__

