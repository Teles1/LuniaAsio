#pragma once
namespace Lunia {
	namespace Lobby {
		bool IsValidCharacterName(const String& characterName);
		/** @return std::wstring:npos if the string is not found in the source */
		bool IsContained(const std::wstring& src, const std::vector<String>& dictionary);
		bool IsMatched(const std::wstring& src, const std::vector<String>& dictionary)
		{
			for (std::vector<std::wstring>::const_iterator i = dictionary.begin(), end = dictionary.end(); i != end; ++i)
			{
				if (src == *i)
					return true;
			}
			return false;
		}
	}
}