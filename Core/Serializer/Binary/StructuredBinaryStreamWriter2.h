#pragma once
#include "StructuredBinaryStream.h"

namespace Lunia {
	namespace Serializer {
		/**
		Version 2 structured binary writer


		@author alkee
		@author juhnu


		bugs fixed:

		@juhnu writes the string's datasize now in bytes, not in characters
		*/

		class StructuredBinaryStreamWriter2 : public ReferenceCountedImpl<IRefCountedStreamWriter> {
		private:
			Lunia::IStreamWriter* stream;
			com_ptr<IRefCountedStreamWriter> refcountedStream;

		private: // helpers
			template <typename T> void inline write(const wchar_t* name, T& value)
			{
				//ALLM_INFO((L"writing name=%s, size=%d", name,sizeof(value)));
				unsigned int hash = StringUtil::Hash(name);
				int dataSize = sizeof(value);
				stream->Write((const unsigned char*)&hash, sizeof(hash));
				stream->Write((const unsigned char*)&dataSize, sizeof(dataSize)); // write data size information to skip
				stream->Write((const unsigned char*)&value, (unsigned int)dataSize);
			}

			template <typename T> void inline writeString(const wchar_t* name, const T& value)
			{

				unsigned int hash = StringUtil::Hash(name);
				int size = (int)value.size(); ++size; // end of string

				//ALLM_INFO((L"writing string name=%s, size=%s", (unsigned int)(size*sizeof(T::value_type)));
				uint32 datasize = (unsigned int)(size * sizeof(T::value_type));
				stream->Write((const unsigned char*)&hash, sizeof(hash));
				stream->Write((const unsigned char*)&datasize, sizeof(datasize));
				stream->Write((const unsigned char*)value.c_str(), (unsigned int)(size * sizeof(T::value_type)));
			}

		public:
			StructuredBinaryStreamWriter2(Lunia::IStreamWriter& pStream)
				: stream(&pStream)
			{
				WriteHeader();
			}

			StructuredBinaryStreamWriter2(com_ptr<Lunia::IRefCountedStreamWriter>& pStream)
				: stream(pStream.get()), refcountedStream(pStream)
			{
				WriteHeader();
			}

			void inline WriteHeader()
			{
				StructuredBinaryStreamHeader header;
				stream->Write(reinterpret_cast<unsigned char*>(&header), sizeof(header));
			}

		public: /* IStreamWriter implementation */

			virtual Type inline GetType() const {
				return Type::Structured;
			}

			virtual void inline Begin(const wchar_t* name, int majorVersion, int minorVersion)
			{
				unsigned int hashCode = StringUtil::Hash(name);
				stream->Write((const unsigned char*)&hashCode, sizeof(hashCode));
				stream->Write((const unsigned char*)&majorVersion, sizeof(majorVersion));
				stream->Write((const unsigned char*)&minorVersion, sizeof(minorVersion));
			}
			virtual void inline Write(const wchar_t* name, const Locator& locator) {
				Write(name, locator.GetRelativePath());
			}
			virtual void inline Write(const wchar_t* name, bool value) { uint8 val = value; write(name, val); }
			virtual void inline Write(const wchar_t* name, char value) { write(name, value); }
			virtual void inline Write(const wchar_t* name, unsigned char value) { write(name, value); }
			virtual void inline Write(const wchar_t* name, signed short value) { write(name, value); }
			virtual void inline Write(const wchar_t* name, unsigned short value) { write(name, value); }
			virtual void inline Write(const wchar_t* name, int value) { write(name, value); }
			virtual void inline Write(const wchar_t* name, unsigned int value) { write(name, value); }
			virtual void inline Write(const wchar_t* name, __int64 value) { write(name, value); }
			virtual void inline Write(const wchar_t* name, unsigned __int64 value) { write(name, value); }
			virtual void inline Write(const wchar_t* name, double value) { write(name, value); }
			virtual void inline Write(const wchar_t* name, float value) { write(name, value); }
			virtual void inline Write(const wchar_t* name, const float2& value) { write(name, value); }
			virtual void inline Write(const wchar_t* name, const float3& value) { write(name, value); }
			virtual void inline Write(const wchar_t* name, const float4& value) { write(name, value); }
			virtual void inline WriteRawData(const wchar_t* name, const unsigned char* buffer, unsigned int dataSize) {
				unsigned int hash = StringUtil::Hash(name);
				stream->Write((const unsigned char*)&hash, sizeof(hash));
				stream->Write((const unsigned char*)&dataSize, sizeof(dataSize)); // write data size information to skip
				stream->Write(buffer, dataSize);
			}
			virtual void inline Write(const wchar_t* name, const ISerializable& value)
			{
				//ALLM_INFO((L"writing object, name=%s", name));
				unsigned int hashCode = StringUtil::Hash(name);
				stream->Write((const unsigned char*)&hashCode, sizeof(hashCode));

				int position = stream->GetWriteCursor(); // get index position to write struct size
				stream->Write((const unsigned char*)&hashCode, sizeof(int)); // reserve place for datasize

				value.Serialize(*this); // serialize

				int dataSize = stream->GetWriteCursor() - position - sizeof(dataSize); // last stream position

				//ALLM_INFO((L"writing object, name=%s datasize=%d", name, dataSize));
				stream->SetWriteCursor(position, IStream::CursorPosition::Begin); // move cursor to index position
				stream->Write((const unsigned char*)&dataSize, sizeof(dataSize)); // write datasize
				stream->SetWriteCursor(0, IStream::CursorPosition::End); // move cursor to end of stream
			}
			virtual void inline Write(const wchar_t* name, const wchar_t* buffer) { writeString(name, std::wstring(buffer)); }
			virtual void inline Write(const wchar_t* name, const char* buffer) { writeString(name, std::string(buffer)); }
		};
	}
}
