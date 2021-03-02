#include "TextPacket.h"
namespace Lunia {
	namespace XRated {
		namespace Http {
			std::string ExtractOperation(const std::string& source, const char header, const char footer, const char eol)
			{
				/*
					extract operation in Url like "operation" in
					"http://blabla.com/sub/operation.ext"

					and eol(end of line) character makes cut the query string off like
					"/operation.exe?key=value&next" to "/operation.exe"
				*/

				std::string::size_type qposition = source.find(eol);
				std::string src = ((qposition == std::string::npos) ? source : source.substr(0, qposition));

				std::string::size_type lastPathSeparatorPosition = source.find_last_of(header);
				if (lastPathSeparatorPosition == std::string::npos) lastPathSeparatorPosition = 0;
				else ++lastPathSeparatorPosition;

				std::string::size_type lastFileExtentionPosition = source.find_last_of(footer);
				if (lastFileExtentionPosition == std::string::npos) lastFileExtentionPosition = source.size();

				if (lastPathSeparatorPosition > lastFileExtentionPosition) // how it can be ?
				{
					LoggerInstance().Error("error parsing string : {}", source.c_str());
					return "";
				}

				return std::string(source.c_str() + lastPathSeparatorPosition, lastFileExtentionPosition - lastPathSeparatorPosition);
			}
			TextPacket::Subpacket::Subpacket()
			{
				subelements.reserve(20);
			}
			TextPacket::Subpacket::Subpacket(std::size_t reserve)
			{
				subelements.reserve(reserve);
			}
			TextPacket::Subpacket::Subpacket(const std::string& param)
			{
				Parse(param.c_str());
			}
			TextPacket::Subpacket::Subpacket(const char* param)
			{
				Parse(param);
			}
			TextPacket::Subpacket& TextPacket::Subpacket::operator<<(const XRated::InstanceEx& param)
			{
				Append(param.Instance); Append(param.ExpireDate.ToString()); return *this;
			}
			void TextPacket::Subpacket::Parse(const char* content)
			{
				if (subelements.empty() == false)
					subelements.clear(); // now, subelements.capacity()==0

				if (*content == Separator || *content == Terminator) return; // nothing to parse

				const char* cur = content;
				const char* begin = cur;
				while (*cur != Separator && *cur != '\0') // end of parsing
				{
					if (*cur == Subseparator || *cur == Separator || *cur == Terminator)
					{
						subelements.push_back(std::string(begin, cur));
						begin = cur + 1;
					}
					++cur;
				}

				subelements.push_back(std::string(begin, cur));

				return;
			}
			std::string& TextPacket::Subpacket::operator[](uint16 index)
			{
				if (subelements.size() > index) return subelements.at(index); else throw "Index Outside range";
			}
			const std::string& TextPacket::Subpacket::operator[](uint16 index) const
			{
				if (subelements.size() > index) return subelements.at(index); else throw "Index Outside range";
			}
			bool TextPacket::Subpacket::IsEmpty() const
			{
				return subelements.empty();
			}
			uint16 TextPacket::Subpacket::GetCount() const
			{
				return static_cast<uint16>(subelements.size());
			}
			std::string TextPacket::Subpacket::ToString() const
			{
				std::string buffer; buffer.reserve(102400);
				std::vector< std::string >::const_iterator end = subelements.end();
				for (std::vector< std::string >::const_iterator i = subelements.begin(); i != end;)
				{
					buffer += *i; ++i; if (i != end) buffer += Subseparator;
				}

				return buffer;
			}
			void TextPacket::Parse(const char* content)
			{
				if (elements.empty() == false)
					elements.clear();	// now, elements.capacity()==0
				if (*content == Terminator) return; // nothing to parse

				const char* cur = content;
				const char* begin = cur;

				while (*cur != Terminator) // end of parsing
				{
					if (*cur == Separator || *cur == Terminator)
					{
						elements.push_back(std::string(begin, cur));
						begin = cur + 1;
					}
					++cur;
				}

				elements.push_back(std::string(begin, cur));

				return;
			}
			const std::string& TextPacket::GetOperation() const
			{
				return operation;
			}
			TextPacket::Methods TextPacket::GetMethod() const
			{
				return method;
			}
			bool TextPacket::IsEmpty() const
			{
				return elements.empty();
			}
			uint32 TextPacket::GetCount() const
			{
				return static_cast<uint32>(elements.size());
			}
			std::string& TextPacket::operator[](uint32 index)
			{
				return elements.at(index);
			}
			const std::string& TextPacket::operator[](uint32 index) const
			{
				return elements.at(index);
			}
			std::string TextPacket::ToString() const
			{
				std::string buffer; buffer.reserve(102400);
				std::vector< std::string >::const_iterator end = elements.end();
				for (std::vector< std::string >::const_iterator i = elements.begin(); i != end;)
				{
					buffer += *i; ++i; if (i != end) buffer += Separator;
				}

				return buffer;
			}
			std::vector<std::string> TextPacket::Copy()
			{
				return elements;
			}
		}
	}
}