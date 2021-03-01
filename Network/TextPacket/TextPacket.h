#pragma once
#include <Core/GameConstants.h>
#include <Core/Utils/StringUtil.h>
namespace Lunia {
	/** to extract operation in Url like "operation" in "http://blabla.com/sub/operation.ext". */
	std::string ExtractOperation(const std::string& source, const char header = '/', const char footer = '.', const char eol = '?');


	/**
		@brief ASP dependent http packet.

		@code

			Service::Http::TextPacket packet("Start"); // calls "Start.asp"
			packet << ip << ConfigInfoInstance().Port() << ConfigInfoInstance().RoomCnt(); // append separated packet as any types that can be converted by StringUtil::To
			packet << ( Service::Http::TextPacket::Subpacket() << stageList[i] << stageGroup->Capacity ); // appending subseparated packet

		@endcode
	*/
	class TextPacket
	{
	public:
		class Subpacket
		{
		public:
			Subpacket();
			Subpacket(std::size_t reserve);
			Subpacket(const std::string& param);
			Subpacket(const char* param);

			/* function to make a packet */
			template <typename T> inline void Append(const T& param) { Append(StringUtil::ToASCII(param)); }
			template <> inline void Append(const std::string& param) { subelements.push_back(param); }
			template <typename T> inline Subpacket& operator<< (const T& param) { Append(param); return *this; }
			Subpacket& operator<< (const XRated::InstanceEx& param);
			void Parse(const char* content);

			/* function to retrieve */
			std::string& operator[](uint16 index);
			const std::string& operator[](uint16 index) const;
			bool IsEmpty() const;
			uint16 GetCount() const;
			std::string ToString() const;

		private:
			std::vector< std::string > subelements;
		};

		enum Methods { GET = 0, POST, };
		TextPacket(const std::string& operation, Methods method = Methods::GET, std::size_t reserve = 20)
			: operation(operation), method(method) {
			elements.reserve(reserve);
		}

		/* function to make a packet */
		template <typename T> inline void Append(const T& param) { Append(StringUtil::ToASCII(param)); }
		template <> inline void Append(const std::string& param) { elements.push_back(param); }
		template <> inline void Append(const Subpacket& param) { elements.push_back(param.ToString()); }
		template <typename T> inline TextPacket& operator<< (const T& param) { Append(param); return *this; }
		void Parse(const char* content);

		/* function to retrieve */
		const std::string& GetOperation() const;
		Methods GetMethod() const;
		bool IsEmpty() const;
		uint32 GetCount() const;
		std::string& operator[](uint32 index);
		const std::string& operator[](uint32 index) const;
		std::string ToString() const;
		std::vector<std::string> Copy();

	private:
		static const char Separator = 0x08;
		static const char Subseparator = 0x0b;
		static const char Terminator = 0x00;

		std::string operation;
		Methods method;
		std::vector< std::string > elements;
	};
}


