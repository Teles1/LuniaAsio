#pragma once
#include "StructuredBinaryStream.h"
#include <stack>
#include <Core/Utils/ConfigReader.h>

namespace Lunia {
	namespace Serializer {
		class StructuredBinaryStreamException : public Exception
		{
		public:
			inline StructuredBinaryStreamException() : Exception("unable to read serialized stream") { }
			inline StructuredBinaryStreamException(const wchar_t* message) : Exception(message) {}
		};

		class StructuredBinaryStreamReader : public ReferenceCountedImpl<IRefCountedStreamReader> {
		private:
			Lunia::IStreamReader* stream;
			com_ptr<IRefCountedStreamReader> refcountedStream;
			std::stack<uint32> sizeIndex;
			std::stack<uint32> levelStartPosition;
		private: // helpers
			template <typename T> void inline read(const wchar_t* name, T& value)
			{
				Logger::GetInstance().Debug(L"reading name={0}, size={1}", name, sizeof(value));
				uint32 nameHash = StringUtil::Hash(name);
				size_t dataSize = sizeof(T);

				uint32 nameHashFromFile = 0;
				stream->Read(reinterpret_cast<uint8*>(&nameHashFromFile), sizeof(uint32));
				if (nameHashFromFile != nameHash) {
					throw Exception(L"name's hash is not same in the file, name={0}", name);
				}

				uint32 dataSizeFromFile = 0;
				stream->Read(reinterpret_cast<uint8*>(&dataSizeFromFile), sizeof(uint32));
				if (dataSizeFromFile != dataSize) {
					throw Exception("data size is not same in the file, size={0}", dataSize);
				}

				stream->Read(reinterpret_cast<uint8*>(&value), sizeof(T));
			}

			template <typename T> void inline readString(const wchar_t* name, T& value)
			{
				Logger::GetInstance().Debug(L"reading string name = {0}", name);
				unsigned int nameHash = StringUtil::Hash(name);

				uint32 nameHashFromFile = 0;
				stream->Read(reinterpret_cast<uint8*>(&nameHashFromFile), sizeof(uint32));
				if (nameHashFromFile != nameHash) {
					throw Exception(L"name's hash is not same in the file, name={0}", name);
				}
				uint32 size = 0;
				stream->Read(reinterpret_cast<uint8*>(&size), sizeof(uint32));

				uint8* buffer = new uint8[size * sizeof(T::value_type)];

				/*for (size_t i=0; i<sizeof(T::value_type); ++i) {
					buffer[size+i]=0;
				}*/
				stream->Read(buffer, size * sizeof(T::value_type));
				value = reinterpret_cast<T::value_type*>(buffer);
				delete buffer;

			}

			void inline Read(const wchar_t* name, std::string& value) { readString(name, value); }
			void inline Read(const wchar_t* name, std::wstring& value) { readString(name, value); }


		public:
			StructuredBinaryStreamReader(Lunia::IStreamReader& pStream)
				: stream(&pStream)
			{
				VerifyHeader();
			}

			StructuredBinaryStreamReader(com_ptr<Lunia::IRefCountedStreamReader>& pStream)
				: stream(pStream.get()), refcountedStream(pStream)
			{
				VerifyHeader();
			}

			void inline VerifyHeader()
			{
				StructuredBinaryStreamHeader header;
				stream->Read(reinterpret_cast<unsigned char*>(&header), sizeof(header));
				if (!header.IsVersion1())
					throw(StructuredBinaryStreamException(L"invalid stream header"));
			}

			virtual Type inline GetType() const {
				return Type::Structured;
			}


			virtual void inline Begin(const wchar_t* name, int* majorVersionOut, int* minorVersionOut)
			{
				//ALLM_INFO((L"name = %s", name));
				unsigned int nameHash = StringUtil::Hash(name);

				uint32 nameHashFromFile = 0;
				stream->Read(reinterpret_cast<uint8*>(&nameHashFromFile), sizeof(uint32));
				if (nameHashFromFile != nameHash) {
					throw Exception(L"name's hash is not same in the file, name={0}", name);
				}

				int32 majorVersionFromFile = 0;
				//ALLM_INFO((L"reading majorVersion"));
				stream->Read(reinterpret_cast<uint8*>(&majorVersionFromFile), sizeof(int32));
				int32 minorVersionFromFile = 0;
				//ALLM_INFO((L"reading minorVersion"));
				stream->Read(reinterpret_cast<uint8*>(&minorVersionFromFile), sizeof(int32));

				if (majorVersionOut) *majorVersionOut = majorVersionFromFile;
				if (minorVersionOut) *minorVersionOut = minorVersionFromFile;


				/*				if (majorVersionFromFile != majorVersion) {
								   throw Exception(ALLM_EXCEPTION((L"a class %s version mismatch, serialized version=(%d,%d), required version=(%d,%d)", name, majorVersionFromFile, minorVersionFromFile, majorVersion, minorVersion)));
								}

								if (minorVersionFromFile != minorVersion) {
									ALLM_INFO((L"a class %s being deserialized has a lower minor version id, serialized version=(%d,%d), required version=(%d,%d)", name, majorVersionFromFile, minorVersionFromFile, majorVersion, minorVersion));
								}*/

								//ALLM_INFO((L"completed"));
			}

			virtual bool inline Exists(const wchar_t* name) const {

				unsigned int hashCode = StringUtil::Hash(name);
				uint32 dataSize = sizeIndex.top();
				int lastPosition = stream->GetReadCursor();
				uint32 check;

				//dataSize-=sizeof(uint32)*3; // begin header - type hash, maj ver, min ver

				while (levelStartPosition.top() + dataSize > stream->GetReadCursor())
				{
					/*if (stream->GetSizeLeft() < 2*sizeof(uint32)) {
						stream->SetReadCursor(lastPosition, IStream::CursorPosition::Begin);
						return false;
					}*/
					stream->Read(reinterpret_cast<uint8*>(&check), sizeof(uint32));
					if (check == hashCode) {
						stream->SetReadCursor(lastPosition, IStream::CursorPosition::Begin);
						return true;
					}
					stream->Read(reinterpret_cast<uint8*>(&check), sizeof(uint32));
					stream->SetReadCursor(check, IStream::CursorPosition::Current); // skip data
				}

				if (levelStartPosition.top() + dataSize != stream->GetReadCursor()) {
					throw Exception(L"size mismatch, {0}!={1}", levelStartPosition.top() + dataSize, stream->GetReadCursor());
				}
				// release
				stream->SetReadCursor(lastPosition, IStream::CursorPosition::Begin);
				return false;
			}

			virtual size_t inline GetCount(const wchar_t* type) const {

				//ALLM_INFO((L"GetCount, type=%s", type));
				size_t count = 0;
				unsigned int hashCode = StringUtil::Hash(type);
				int dataSize = sizeIndex.top();
				int lastPosition = stream->GetReadCursor();
				//dataSize-=sizeof(int)*3; // begin header - type hash, maj ver, min ver

				while (levelStartPosition.top() + dataSize > stream->GetReadCursor())
				{
					/*if (stream->GetSizeLeft() < 2*sizeof(uint32)) {
						stream->SetReadCursor(lastPosition, IStream::CursorPosition::Begin);
						return 0;
					}*/
					uint32 nameHashFromFile;
					stream->Read(reinterpret_cast<uint8*>(&nameHashFromFile), sizeof(uint32));
					if (nameHashFromFile == hashCode) ++count;

					uint32 dataSizeFromFile;
					stream->Read(reinterpret_cast<uint8*>(&dataSizeFromFile), sizeof(uint32));
					stream->SetReadCursor(dataSizeFromFile, IStream::CursorPosition::Current); // skip data
				}

				if (levelStartPosition.top() + dataSize != stream->GetReadCursor()) {
					throw Exception(L"size mismatch, {0}!={1}", levelStartPosition.top() + dataSize, stream->GetReadCursor());
				}
				//assert(lastPosition+dataSize==stream->GetReadCursor());  // size mismatched

				// release
				stream->SetReadCursor(lastPosition, IStream::CursorPosition::Begin);
				//ALLM_INFO((L"GetCount, count=%d", count));
				return count;
			}

			virtual void inline Read(const wchar_t* name, Locator& value) {
				std::wstring path;
				Read(name, path);
				value = Locator(path.c_str(), stream->GetName());
			}

			virtual void inline Read(const wchar_t* name, bool& value) { uint8 tmp; read(name, tmp); if (tmp) { value = 1; } else { value = 0; } } //we do this to avoid possible configuration specific changes in bool size(byte or int?)
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

			virtual void inline ReadRawData(const wchar_t* name, unsigned char* buffer, unsigned int dataSize) {
				//ALLM_INFO((L"reading raw data, name=%s", name));
				uint32 nameHash = StringUtil::Hash(name);
				uint32 nameHashFromFile = 0;
				stream->Read(reinterpret_cast<uint8*>(&nameHashFromFile), sizeof(uint32));
				if (nameHashFromFile != nameHash) {
					throw Exception(L"name's hash is not same in the file, name={0}", name);
				}
				uint32 dataSizeFromFile = 0;
				stream->Read(reinterpret_cast<uint8*>(&dataSizeFromFile), sizeof(uint32));
				if (dataSizeFromFile != dataSize) {
					throw Exception(L"data size is not same in the file, size={0}", dataSize);
				}
				stream->Read(buffer, dataSize);
			}

			virtual void inline Read(const wchar_t* name, ISerializable& value)
			{
				uint32 nameHash = StringUtil::Hash(name);
				uint32 nameHashFromFile = 0;
				stream->Read(reinterpret_cast<uint8*>(&nameHashFromFile), sizeof(uint32));
				if (nameHashFromFile != nameHash) {
					throw Exception(L"name's hash is not same in the file, name={0}", name);
				}
				uint32 dataSize = 0;
				stream->Read((unsigned char*)&dataSize, sizeof(uint32));
				sizeIndex.push(dataSize);
				levelStartPosition.push(stream->GetReadCursor());
				value.Deserialize(*this);
				sizeIndex.pop();
				levelStartPosition.pop();
			}

			virtual unsigned int inline Read(const wchar_t* name, wchar_t* buffer, unsigned int size) {
				std::wstring source;
				Read(name, source);
				wcsncpy(buffer, source.c_str(), size - 1);
				buffer[size - 1] = 0;
				return unsigned int(source.size());
			}

			virtual unsigned int inline Read(const wchar_t* name, char* buffer, unsigned int size) {
				std::string source;
				Read(name, source);
				strncpy(buffer, source.c_str(), size - 1);
				buffer[size - 1] = 0;
				return unsigned int(source.size());
			}
		};
	}
}