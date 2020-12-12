#pragma once
namespace Lunia {
	namespace Lobby {
        struct InvalidStrings {
            /* dictionaries */
            std::vector<String> shouldNotBeContained;
            std::vector<String> shouldNotBeContainedButChat;
            std::vector<String> shouldNotBeEqual;

            /* local characters set */
            std::vector<wchar_t> localCharacterLowerBound;
            std::vector<wchar_t> localCharacterUpperBound;

            InvalidStrings();
            bool IsLocal(const wchar_t ch) const;

            static bool IsNumeric(const wchar_t ch);
            static bool IsUppercase(const wchar_t ch);
            static bool IsLowercase(const wchar_t ch);
            inline static bool IsAlphabet(const wchar_t ch) {
                return IsLowercase(ch) || IsUppercase(ch);
            }
        };
		InvalidStrings invalidStrings;
		bool IsValidCharacterName(const String& characterName);
		/** @return std::wstring:npos if the string is not found in the source */
		bool IsContained(const std::wstring& src, const std::vector<String>& dictionary);
		bool IsMatched(const std::wstring& src, const std::vector<String>& dictionary);
		
	}
}