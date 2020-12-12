#include <Core/ServiceHelper/LobbyService.h>
#include <Core/GameConstants.h>

namespace Lunia {
	namespace Lobby {
		bool IsValidCharacterName(const String& characterName)
		{
			std::string name = StringUtil::To<std::string>(characterName);
			if (name.size() < XRated::Constants::CreateCharacterNameAsciiMinLength || name.size() > XRated::Constants::CreateCharacterNameAsciiMaxLength)
			{
				Logger::GetInstance().Error("IsValidCharacterNanme : wrong name size : {0}", name.size());
				return false;
			}

			if (IsContained(characterName, invalidStrings.shouldNotBeContained)) 
				return false;

			if (IsContained(characterName, invalidStrings.shouldNotBeContainedButChat))
				return false;

			if (IsMatched(characterName, invalidStrings.shouldNotBeEqual))
				return false;

			for (std::wstring::const_iterator i = characterName.begin(), end = characterName.end(); i < end; ++i){
				if (!InvalidStrings::IsAlphabet(*i) && !InvalidStrings::IsNumeric(*i) && !invalidStrings.IsLocal(*i))
					return false;
			}

			return true;
		}
		bool IsContained(const std::wstring& src, const std::vector<String>& dictionary){
			for (std::vector<std::wstring>::const_iterator i = dictionary.begin(), end = dictionary.end(); i != end; ++i)
				{
					//wchar_t* f = StringUtil::wcsistr(src.c_str(), (*i).c_str()); // case insensitive comparing
					//if (f != NULL)												 // found a string
					//	return true;
				}
				return false;
		}
		bool IsMatched(const std::wstring& src, const std::vector<String>& dictionary){
			for (std::vector<std::wstring>::const_iterator i = dictionary.begin(), end = dictionary.end(); i != end; ++i)
			{
				if (src == *i)
					return true;
			}
			return false;
		}
	}
}