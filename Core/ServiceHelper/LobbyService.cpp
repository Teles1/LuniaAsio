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
				//case insensitive search.
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
		InvalidStrings::InvalidStrings()
		{
		}
		bool InvalidStrings::IsNumeric(const wchar_t ch)
		{
			return (ch >= 0x0030 && ch <= 0x0039) ? true : false;
		}

		bool InvalidStrings::IsUppercase(const wchar_t ch)
		{
			return (ch >= 0x0041 && ch <= 0x005a) ? true : false;
		}

		bool InvalidStrings::IsLowercase(const wchar_t ch)
		{
			return (ch >= 0x0061 && ch <= 0x007a) ? true : false;
		}

		bool InvalidStrings::IsLocal(const wchar_t ch) const
		{
			size_t cnt = localCharacterLowerBound.size();
			while (cnt > 0)
			{
				--cnt;
				if (ch >= localCharacterLowerBound[cnt] && ch <= localCharacterUpperBound[cnt])
					return true;
			}
			return false;
		}
	}
}