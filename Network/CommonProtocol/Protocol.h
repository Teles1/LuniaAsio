#ifndef Protocol_H
#define Protocol_H
#pragma once
#include <Core/Serializer/Serializer.h>

namespace Lunia {
	namespace XRated {
		namespace Net {
			namespace Protocol {
				struct Address : public Serializer::ISerializable {
					std::string Ip;
					uint16 Port;

					Address();
					Address(const std::string& address);
					Address(const char* addresss);
					Address(const std::string& ip, uint16 port);

					std::string ToString();  ///< make string like "xxx.xxx.xxx.xxx:pppp"
					void FromString(const std::string&
						address);  ///< parse string like "xxx.xxx.xxx.xxx:pppp"

					void Serialize(Serializer::IStreamWriter& out) const;
					void Deserialize(Serializer::IStreamReader& in);
				};
			}
		}
	}
}
#endif // ! Protocol_H