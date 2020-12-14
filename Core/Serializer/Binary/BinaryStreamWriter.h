#pragma once

#include <Core/Serializer/Serializer.h>
#include <Core/com_ptr.h>
#include <Core/ReferenceCounted.h>

namespace Lunia {
	namespace Serializer {
#pragma warning( push )
#pragma warning( disable:4512 ) // assignment operator couldnot be generated
#pragma warning( disable:4355 ) // this used in base member initializer list

		class BinaryStreamWriter : public Lunia::ReferenceCountedImpl<IRefCountedStreamWriter> {
		private:
			Lunia::IStreamWriter* stream;
			com_ptr<Lunia::IRefCountedStreamWriter> refcountedStream;


		private: // helpers
			template <typename T> void write(const wchar_t* /*name*/, T& value)
			{
				stream->Write((const unsigned char*)&value, sizeof(value));
			}

			template <typename T> void writeString(const wchar_t* /*name*/, const T& value)
			{
				int size = (int)value.size();
				++size; // end of string
				stream->Write((const unsigned char*)&size, (unsigned int)sizeof(size));
				stream->Write((const unsigned char*)value.c_str(), (unsigned int)(size * sizeof(T::value_type)));
			}
		public:
			BinaryStreamWriter(Lunia::IStreamWriter& pStream)
				: stream(&pStream)
			{
			}

			BinaryStreamWriter(com_ptr<Lunia::IRefCountedStreamWriter>& pStream)
				: stream(pStream.get()), refcountedStream(pStream)
			{
			}

		public: /* IStreamWriter implementation */

			virtual Type GetType() const {
				return Type::Flat;
			}

			virtual void Begin(const wchar_t* /*name*/, int /*majorVersion*/, int /*minorVersion*/) {}

			virtual void Write(const wchar_t* name, const Locator& locator) {
				Write(name, locator.GetRelativePath());
			}

			virtual void Write(const wchar_t* name, bool value) { write(name, value); }
			virtual void Write(const wchar_t* name, char value) { write(name, value); }
			virtual void Write(const wchar_t* name, unsigned char value) { write(name, value); }
			virtual void Write(const wchar_t* name, signed short value) { write(name, value); }
			virtual void Write(const wchar_t* name, unsigned short value) { write(name, value); }
			virtual void Write(const wchar_t* name, int value) { write(name, value); }
			virtual void Write(const wchar_t* name, unsigned int value) { write(name, value); }
			virtual void Write(const wchar_t* name, __int64 value) { write(name, value); }
			virtual void Write(const wchar_t* name, unsigned __int64 value) { write(name, value); }
			virtual void Write(const wchar_t* name, double value) { write(name, value); }
			virtual void Write(const wchar_t* name, float value) { write(name, value); }
			virtual void Write(const wchar_t* name, const float2& value) { write(name, value); }
			virtual void Write(const wchar_t* name, const float3& value) { write(name, value); }
			virtual void Write(const wchar_t* name, const float4& value) { write(name, value); }

			//			virtual void Write(const wchar_t* name, const std::string& value) { writeString(name, value); }
			//			virtual void Write(const wchar_t* name, const std::wstring& value) { writeString(name, value); }
			virtual void Write(const wchar_t* /*name*/, const ISerializable& value) { value.Serialize(*this); }

			virtual void  Write(const wchar_t* name, const wchar_t* buffer) { writeString(name, std::wstring(buffer)); }
			virtual void  Write(const wchar_t* name, const char* buffer) { writeString(name, std::string(buffer)); }



		};


#pragma warning( pop ) // recover warning state
	}
}