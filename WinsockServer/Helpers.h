#ifndef __HELPERS__
#define __HELPERS__
#pragma once
/* simple multithreaded output */
class mt_output
{
};

template<typename T>
std::ostream& operator<<(const mt_output &class_instance, const T& output)
{
	static std::mutex m_print_output_mutex;
	std::lock_guard<std::mutex> lock(m_print_output_mutex);
	std::ostringstream newOstream;
	std::cout << output;
	return std::cout;
}

static mt_output _mt_output_instance;
#define console_log _mt_output_instance

template<typename T>
using custom_deleter_unique_ptr = std::unique_ptr<T, std::function<void(T*)>>;

template<typename T>
class Socket
{
	T m_socket;
public:
	Socket() : m_socket(::INVALID_SOCKET) {}
	Socket(const T& socket): m_socket(socket)
	{

	}
	~Socket()
	{
		::closesocket(m_socket);
		m_socket = INVALID_SOCKET;
	}
	inline const T getSocket() const
	{
		return m_socket;
	}
	operator T&() { return m_socket; }
	operator T() const { return m_socket; }
};

#endif
