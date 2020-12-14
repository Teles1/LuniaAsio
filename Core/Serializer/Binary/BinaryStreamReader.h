#pragma once

#include <Core/Serializer/Serializer.h>
#include <Core/com_ptr.h>
#include <Core/ReferenceCounted.h>

namespace Lunia {
	namespace Serializer {
		class BinaryStreamReader : public Lunia::ReferenceCountedImpl<IRefCountedStreamReader> {
		private:
			Lunia::IStreamReader* stream;
			com_ptr<Lunia::IRefCountedStreamReader> refcountedStream;

		private: // helpers
			template <typename T> void inline read(const wchar_t* /*name*/, T& value)
			{
				stream->Read((unsigned char*)&value, sizeof(value));
			}

			template <typename T> void inline readString(const wchar_t* /*name*/, T& value)
			{
				int size;
				stream->Read((unsigned char*)&size, (unsigned int)sizeof(size));

				typename T::value_type* buffer = new typename T::value_type[size];
				stream->Read((unsigned char*)buffer, (unsigned int)(size * sizeof(T::value_type)));
				value = buffer;
				delete[] buffer;
			}

			void Read(const wchar_t* name, std::string& value) { readString(name, value); }
			void Read(const wchar_t* name, std::wstring& value) { readString(name, value); }


		public:
			inline BinaryStreamReader(Lunia::IStreamReader& pStream)
				: stream(&pStream)
			{
			}
			inline BinaryStreamReader(unsigned char* buffer, int bufferSize)/* : stream(&pStream) */
			{
				Lunia::IStreamWriter* writer;
				writer->Write(buffer, bufferSize);
			}

			inline BinaryStreamReader(com_ptr<Lunia::IRefCountedStreamReader>& pStream)
				: stream(pStream.get()), refcountedStream(pStream)
			{
			}

			virtual Type inline GetType() const {
				return Type::Flat;
			}


			virtual void inline Begin(const wchar_t* /*name*/, int* /*majorVersion*/, int* /*minorVersion */) {}

			virtual bool inline Exists(const wchar_t* /*name*/) const {
				throw Exception(L"not implemented");
			}

			virtual size_t inline GetCount(const wchar_t* /*type*/) const {
				throw Exception(L"not implemented");
			}

			virtual void inline Read(const wchar_t* name, Locator& value) {
				std::wstring path;
				Read(name, path);
				value = Locator(path.c_str(), stream->GetName());
			}

			virtual void inline Read(const wchar_t* name, bool& value) { read(name, value); }
			virtual void inline Read(const wchar_t* name, char& value) { read(name, value); }
			virtual void inline Read(const wchar_t* name, unsigned char& value) { read(name, value); }
			virtual void inline Read(const wchar_t* name, signed short& value) { read(name, value); }
			virtual void inline Read(const wchar_t* name, unsigned short& value) { read(name, value); }
			virtual void inline Read(const wchar_t* name, int& value) { read(name, value); }
			virtual void inline Read(const wchar_t* name, unsigned int& value) { read(name, value); }
			virtual void inline Read(const wchar_t* name, __int64& value) { read(name, value); }
			virtual void inline Read(const wchar_t* name, unsigned __int64& value) { read(name, value); }
			virtual void inline Read(const wchar_t* name, double& value) { read(name, value); }
			virtual void inline Read(const wchar_t* name, float& value) { read(name, value); }
			virtual void inline Read(const wchar_t* name, float2& value) { read(name, value); }
			virtual void inline Read(const wchar_t* name, float3& value) { read(name, value); }
			virtual void inline Read(const wchar_t* name, float4& value) { read(name, value); }

			virtual void inline Read(const wchar_t* /*name*/, ISerializable& value) { value.Deserialize(*this); }

			virtual unsigned int inline Read(const wchar_t* name, wchar_t* buffer, unsigned int size) {
				std::wstring source;
				Read(name, source);
				wcsncpy_s(buffer, size, source.c_str(), size - 1);
				buffer[size - 1] = 0;
				return unsigned int(source.size());
			}

			virtual unsigned int inline Read(const wchar_t* name, char* buffer, unsigned int size) {
				std::string source;
				Read(name, source);
				strncpy_s(buffer, size, source.c_str(), size - 1);
				buffer[size - 1] = 0;
				return unsigned int(source.size());
			}
		};
	}
}