#ifndef Api_GUARD
#define Api_GUARD
#pragma warning(disable : 4996)
#pragma once
#include "./Core/Core.h"
#include <cpr/cpr.h>
#include "Json.hpp"
#include <string>

// for convenience
using json = nlohmann::json;
namespace Lunia {
    namespace Net {
        struct Answer {
        public:
            Answer(const std::string& errorMessage, const uint16& errorCode, const json& jsonAnswer = json())
                : errorMessage(errorMessage), errorCode(errorCode), resultObjet(jsonAnswer){}
            Answer() = delete;
            std::string errorMessage;
            uint16 errorCode;
            json resultObjet;
        };
        struct Api {
            static std::string ApiUrl;
        public:
            Api(const std::string& reqPage = "");

            template <typename T> inline void Append(const T& param) { Append(StringUtil::ToASCII(param)); }
            template <> inline void Append(const std::string& param) { m_Url.push_back(param); }

            template<typename T> inline Api& operator<< (const T& param)
            {
                Append(param);
                return *this;
            }

            Answer RequestApi() const;
            Answer RequestPost(json value) const;
            std::string BuildUrl() const;
            ~Api() {}
            void AddHeaders();
        private:
            cpr::Header                                     m_Header;
            std::vector<std::string>                        m_Url;
            json                                            m_Body;
        };
    }
}
#endif // !Api_GUARD