#ifndef LUNIA_RESOURCE_ITEXTLOADERLISTENER_H
#define LUNIA_RESOURCE_ITEXTLOADERLISTENER_H

/**
Loader for the configuration files. Currently the format is based on XML, but as the loader
gives you just key/value pairs, the actual fileformat is totally transparent to the user of
this class. The interface is designed for simple key/value kind of data. It doesnt suit well
for binary style data. There is another loader for a chunk based binary data format.


@see Resource::IBinaryLoader

Singleton Factory:
@see Resource::GetLoaderInstance()

@author juhnu
@date 1.12.2003
*/

#include <string>
#include <map>
#include <Core/Utils/Math/Math.h>

namespace Lunia {
	namespace Resource {

		class ITextLoaderListener {
		public:

			class Parameter {
				std::wstring value;
			public:

				operator std::string() inline const { return StringUtil::To<std::string>(value); }

				operator std::wstring() inline  const { return value.c_str();/* c_str() avoids buffer sharing between string objects, and therefore avoids heap memory deallocation problems across dlls */ }


				operator unsigned int() inline const {
					return StringUtil::To<unsigned int>(value);
				}

				operator int() inline  const {
					return StringUtil::To<int>(value);
				}

				operator float() inline const {
					return StringUtil::To<float>(value);
				}

				operator double() inline const {
					return StringUtil::To<double>(value);
				}

				operator float2() inline const {
					return StringUtil::To<float2>(value);
				}

				operator float3() inline const {
					return StringUtil::To<float3>(value);
				}

				operator float4() const {
					return StringUtil::To<float4>(value);
				}

				inline Parameter(const std::wstring& pValue) :value(pValue) {}
				inline Parameter(const wchar_t* pValue) :value(pValue) {}
				inline Parameter() {}
			};

			enum Iteration {
				Normal,
				SkipChildren,
				Terminate

			};

			class Parameters {

				Parameter empty;
				typedef std::map<std::wstring, Parameter> Params;
				Params buffer;

			public:
				typedef Params::const_iterator const_iterator;
				typedef Params::iterator iterator;
				typedef Params::value_type value_type;

				Params::const_iterator begin() inline const { return buffer.begin(); }
				Params::const_iterator end() inline const { return buffer.end(); }

				inline const Parameter& operator[](const std::wstring& index) const {
					Params::const_iterator it = buffer.find(index);
					if (it == buffer.end()) {
						return empty;
					}
					return it->second;
				}

				inline void Add(const std::wstring& value, const std::wstring& param) {
					buffer[value] = Parameter(param);
				}
				inline const_iterator find(const std::wstring& index) const {
					return buffer.find(index);
				}
			};



			//		typedef std::map<std::wstring, Parameter> Parameters;

			virtual Iteration OnElement(const std::wstring& element, const Parameters& params) = 0;

			inline virtual ~ITextLoaderListener() {}

		};
	}
}



#endif