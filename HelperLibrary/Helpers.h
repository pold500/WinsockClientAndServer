#ifndef __HELPERS__
#define __HELPERS__

#include <memory>
#include <string>
#include <array>
#include <functional>
#include <vector>
#include <WinSock2.h>		
#include <iostream>
#include "LogStream.h"
#include <boost/optional.hpp>
#include "GeometryStructures.h"


namespace Helpers
{
	template<typename T>
	using custom_deleter_unique_ptr = std::unique_ptr<T, std::function<void(T*)>>;

	template<typename T>
	class Socket
	{
		T m_socket;
	public:
		Socket() : m_socket(INVALID_SOCKET) {}
		Socket(const T& socket) : m_socket(socket)
		{

		}
		Socket<T> operator=(const Socket<T>& _socket)
		{
			this->m_socket = _socket.getSocket();
			return *this;
		}
		Socket(const Socket<T>& _socket)
		{
			this->m_socket = _socket.getSocket();
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
		operator const T&() const { return m_socket; }
	};

	class CPacket
	{
		
	public:
		enum PacketType
		{
			StringResponse,
			BinaryVertexData,
			BinaryVertexData_V2
		};
		virtual std::string serialize() const = 0;
		virtual void deserialize(const std::string& dataString) = 0;
		inline CPacket(const PacketType& packetType);
		Helpers::CPacket::PacketType GetPacketType() const { return m_packetType; }
	protected:
		PacketType m_packetType;
	};

	class CStringPacket: public CPacket
	{
		
		
	public:
		CStringPacket();
		CStringPacket(const std::string& strData);
		virtual std::string serialize() const;
		virtual void deserialize(const std::string& dataString);
		const std::string& GetStringData() const;

		
	private:

		std::string m_stringData;
		friend class cereal::access;

		template <class Archive>
		void save(Archive & archive) const
		{
			archive(m_packetType);
			archive(m_stringData);
		}

		template<class Archive>
		void load(Archive & archive)
		{
			archive(m_packetType);
			archive(m_stringData);
		}

	};

	class CBinaryVertexDataPacket: public CPacket
	{
		struct ObjData 
		{
			std::vector<Polygon3D> m_polygonsData;

			template<class Archive>
			void save(Archive & archive) const
			{
				archive(m_polygonsData);
			}
			template<class Archive>
			void load(Archive & archive)
			{
				archive(m_polygonsData);
			}
			std::string toString() const
			{
				std::stringstream result;
				for (const auto& polygon : m_polygonsData)
				{
					result << polygon.toString() << "\n";
				}
				return result.str();
			}
		};
		ObjData m_objectData;
	public:

		inline CBinaryVertexDataPacket(): CPacket(PacketType::BinaryVertexData) {}
		CBinaryVertexDataPacket(const std::vector<Polygon3D>& polygonsData);
		virtual std::string serialize() const;
		virtual void deserialize(const std::string& dataString);
		const Helpers::CBinaryVertexDataPacket::ObjData& GetObjectData() const { return m_objectData; }
		void SetObjectData(const Helpers::CBinaryVertexDataPacket::ObjData& val) { m_objectData = val; }
	};
	
	class CBinaryVertexDataPacket_V2 : public CPacket
	{
	public:
		inline CBinaryVertexDataPacket_V2();
		CBinaryVertexDataPacket_V2(ObjFileData_v2* objectData);
		virtual std::string serialize() const;
		virtual void deserialize(const std::string& dataString);
		ObjFileData_v2* GetData() const { return m_objectData; }
		void SetData(ObjFileData_v2* val) { m_objectData = val; }
	private:
		ObjFileData_v2* m_objectData;
		std::unique_ptr<ObjFileData_v2> m_objectDataClient;
	};

	size_t readSizePacket(const std::string& sizePacket);
	std::string createSizePacket(const size_t user_packet_size);
	std::string createEncodedPacket(const std::string& user_data);
	std::string decodePacket(const std::string& user_packet);
	bool sendPacket(const SOCKET ClientSocket, const CPacket& dataPacket);
	std::unique_ptr<CPacket> receivePacket(const SOCKET ClientSocket);

	template<typename SizeType>
	size_t calculatePacketLengthPrefix()
	{
		return sizeof(SizeType);
	}
	std::vector<std::string> split(const std::string & s, const char delim);
	std::vector<std::string> split(const std::string &s, const std::string& delim);
	
	struct PolygonCmd
	{
		static PolygonCmd invalidCmd;
		PolygonCmd() {}
		struct PolygonList
		{
			std::vector<int> polygons_list;
		};
		struct PolygonRange
		{
			size_t lower_bound;
			size_t upper_bound;
			PolygonRange() :
				lower_bound(-1),
				upper_bound(-1)
			{

			}
		};
		std::string file_name; //file name to load geometry from
		PolygonList polyList;
		PolygonRange polyRange;
		enum class Type
		{
			List,
			Interval
		};
		bool isInvalid;
		Type parameters_type;
		PolygonCmd(bool _isInvalid) : isInvalid(_isInvalid) {}
	};

	boost::optional<PolygonCmd> parsePolygonCmd(const std::vector<std::string>& command_tokens, const ObjFileData_v2* objectData);
	
	std::unique_ptr<Helpers::CPacket> createPacketByType(const std::string& rawData);

	void writeObjToFile(const std::string& file_name, const std::vector<Point3D<float>>& vertices, const std::vector<std::vector<int>>& faces);
	
};


#endif


