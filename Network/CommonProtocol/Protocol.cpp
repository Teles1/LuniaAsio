#include "Protocol.h"
namespace Lunia {
	namespace Protocol {
		Address::Address() : Ip(""), Port(0) {}
		Address::Address(const std::string& address) { FromString(address); }
		Address::Address(const char* address) { FromString(address); }
		Address::Address(const std::string& ip, uint16 port) : Ip(ip), Port(port) {}

		std::string Address::ToString() {
			std::string buf(Ip);
			buf += ":";
			buf += StringUtil::ToASCII(Port);
			return buf;
		}

		void Address::FromString(const std::string& address) {
			std::size_t pos = address.find(':');
			if (pos == std::string::npos) {
				Ip = address;
				Port = 0;
				return;
			}
			Ip = address.substr(0, pos);
			Port = static_cast<uint16>(StringUtil::ToInt(address.substr(pos + 1)));
		}

		void Address::Serialize(Serializer::IStreamWriter& out) const {
			out.Begin(L"AllM::Net::Address");
			out.Write(L"Ip", Ip);
			out.Write(L"Port", Port);
		}

		void Address::Deserialize(Serializer::IStreamReader& in) {
			in.Begin(L"AllM::Net::Address");
			in.Read(L"Ip", Ip, std::string(""));
			in.Read(L"Port", Port, static_cast<uint16>(0));
		}
	}
}