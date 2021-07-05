#pragma once
namespace Lunia {
	namespace XRated {
		namespace Http {
			class UrlEncoder
			{
			public:
				static bool IsUnsafe(const char compareChar);
				static std::string Encode(const char* src, int size);
				static std::string Encode(const std::string& src);

			private:
				UrlEncoder() {}
				~UrlEncoder() {}

				const static std::string unsafeCharacters;
			};
		}
	}
}