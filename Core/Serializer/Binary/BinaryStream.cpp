#pragma once

#include <Core/Serializer/Serializer.h>
#include <Core/IStream.h>
#include <Core/com_ptr.h>

#include "BinaryStreamWriter.h"
#include "BinaryStreamReader.h"

#include "StructuredBinaryStreamWriter2.h"
#include "StructuredBinaryStreamReader.h"
#include "StructuredBinaryStreamReader2.h"

#include <Core/FileIO/FileIO.h>
#include <Core/Resource/Xml/Xml.h>
#include <iostream>

namespace Lunia {
	namespace Serializer {
		com_ptr<IRefCountedStreamWriter> CreateBinaryStreamWriter(Lunia::IStreamWriter& stream) {
			return new BinaryStreamWriter(stream);
		}
		com_ptr<IRefCountedStreamReader>  CreateBinaryStreamReader(Lunia::IStreamReader& stream) {
			return new BinaryStreamReader(stream);
		}

		com_ptr<IRefCountedStreamWriter> CreateBinaryStreamWriter(com_ptr<Lunia::IRefCountedStreamWriter> stream) {
			return new BinaryStreamWriter(stream);
		}
		com_ptr<IRefCountedStreamReader>  CreateBinaryStreamReader(com_ptr<Lunia::IRefCountedStreamReader> stream) {
			return new BinaryStreamReader(stream);
		}

		com_ptr<IRefCountedStreamWriter> CreateStructuredBinaryStreamWriter(Lunia::IStreamWriter& stream) {
			return new StructuredBinaryStreamWriter2(stream);
		}

		com_ptr<IRefCountedStreamWriter> CreateStructuredBinaryStreamWriter(com_ptr<Lunia::IRefCountedStreamWriter> stream) {
			return new StructuredBinaryStreamWriter2(stream);
		}

		com_ptr<IRefCountedStreamReader>  CreateStructuredBinaryStreamReader(com_ptr<Lunia::IRefCountedStreamReader> stream) {
			int startPosition = stream->GetReadCursor();
			StructuredBinaryStreamHeader header;
			stream->Read(reinterpret_cast<unsigned char*>(&header), sizeof(header));
			stream->SetReadCursor(startPosition, IStream::CursorPosition::Begin);

			if (header.IsVersion1()) {
				//	ALLM_INFO((L"Loading Structured Binary Format 1, stream=%s", stream->GetName()));
				return new StructuredBinaryStreamReader(stream);
			}

			if (header.IsVersion2()) {
				//	ALLM_INFO((L"Loading Structured Binary Format 2, stream=%s", stream->GetName()));
				return new StructuredBinaryStreamReader2(stream);
			}
			throw Exception(L"Invalid stream header, unknown version");
		}

		com_ptr<IRefCountedStreamReader>  CreateStructuredBinaryStreamReader(Lunia::IStreamReader& stream) {
			int startPosition = stream.GetReadCursor();
			StructuredBinaryStreamHeader header;
			stream.Read(reinterpret_cast<unsigned char*>(&header), sizeof(header));
			stream.SetReadCursor(startPosition, IStream::CursorPosition::Begin);

			if (header.IsVersion1()) {
				//ALLM_INFO((L"Loading Structured Binary Format 1, stream=%s", stream.GetName()));
				return new StructuredBinaryStreamReader(stream);
			}

			if (header.IsVersion2()) {
				//ALLM_INFO((L"Loading Structured Binary Format 2, stream=%s", stream.GetName()));
				return new StructuredBinaryStreamReader2(stream);
			}
			throw Exception(L"Invalid stream header, unknown version");

		}


		/**
		Exports Version 2 format

		*/
		void RecursiveWrite(Xml::Element* element, FileIO::File& writeTo)
		{
			unsigned int hash = StringUtil::Hash(element->Name.c_str());
			writeTo.WriteStruct(hash); // write hash
			const std::wstring& type = element->Attributes[L"type"];
			assert(!type.empty());
			int datasize = 0;

			if (element->SubElements.empty()) // leaf
			{
				const std::wstring& value = element->Attributes[L"value"];

				if (type == L"int16")
				{
					int16 v = (int16)StringUtil::ToInt(value);
					datasize = sizeof(v);
					writeTo.Write((unsigned char*)&datasize, sizeof(datasize));
					writeTo.Write((unsigned char*)&v, datasize);
				}
				else if (type == L"uint16")
				{
					uint16 v = (uint16)StringUtil::ToInt(value);
					datasize = sizeof(v);
					writeTo.Write((unsigned char*)&datasize, sizeof(datasize));
					writeTo.Write((unsigned char*)&v, datasize);
				}
				else if (type == L"int32")
				{
					int32 v = StringUtil::ToInt(value);
					datasize = sizeof(v);
					writeTo.Write((unsigned char*)&datasize, sizeof(datasize));
					writeTo.Write((unsigned char*)&v, datasize);
				}
				else if (type == L"uint32")
				{
					uint32 v = StringUtil::ToInt(value);
					datasize = sizeof(v);
					writeTo.Write((unsigned char*)&datasize, sizeof(datasize));
					writeTo.Write((unsigned char*)&v, datasize);
				}
				else if (type == L"double")
				{
					double v = StringUtil::ToDouble(value);
					datasize = sizeof(v);
					writeTo.Write((unsigned char*)&datasize, sizeof(datasize));
					writeTo.Write((unsigned char*)&v, datasize);
				}
				else if (type == L"float")
				{
					float v = StringUtil::ToFloat(value);
					datasize = sizeof(v);
					writeTo.Write((unsigned char*)&datasize, sizeof(datasize));
					writeTo.Write((unsigned char*)&v, datasize);
				}
				else if (type == L"float2")
				{
					float2 v = StringUtil::ToFloat2(value);
					datasize = sizeof(v);
					writeTo.Write((unsigned char*)&datasize, sizeof(datasize));
					writeTo.Write((unsigned char*)&v, datasize);
				}
				else if (type == L"float3")
				{
					float3 v = StringUtil::ToFloat3(value);
					datasize = sizeof(v);
					writeTo.Write((unsigned char*)&datasize, sizeof(datasize));
					writeTo.Write((unsigned char*)&v, datasize);
				}
				else if (type == L"float4")
				{
					float4 v = StringUtil::ToFloat4(value);
					datasize = sizeof(v);
					writeTo.Write((unsigned char*)&datasize, sizeof(datasize));
					writeTo.Write((unsigned char*)&v, datasize);
				}
				else if (type == L"string")
				{
					std::string v = StringUtil::ToASCII(value);
					datasize = (int)v.size() + 1/*end of string */;
					writeTo.Write((unsigned char*)&datasize, sizeof(datasize));
					writeTo.Write((unsigned char*)v.c_str(), (unsigned int)(datasize * sizeof(std::string::value_type)));
				}
				else if (type == L"wstring")
				{
					datasize = (int)(value.size() + 1) * sizeof(std::wstring::value_type)/*end of string */;
					writeTo.Write((unsigned char*)&datasize, sizeof(datasize));
					writeTo.Write((unsigned char*)value.c_str(), (unsigned int)(datasize));
				}
				else if (type == L"resource")
				{
					datasize = (int)(value.size() + 1) * sizeof(std::wstring::value_type)/*end of string */;
					writeTo.Write((unsigned char*)&datasize, sizeof(datasize));
					writeTo.Write((unsigned char*)value.c_str(), (unsigned int)(datasize/**sizeof(std::wstring::value_type)*/));
				}
				return;
			}

			/* has child element - ISerializable derrived */
			unsigned int position = writeTo.Seek(0, FileIO::File::SeekFrom::Cur);
			writeTo.WriteStruct(position); // reserve place for datasize

			/* write Begin() */
			hash = StringUtil::Hash(type.c_str());
			writeTo.WriteStruct(hash); // type name hash

			float2 v = StringUtil::ToFloat2(element->Attributes[L"version"]);
			/*
			//int32 majorVersionFromFile=0;
				//ALLM_INFO((L"reading majorVersion"));
				//stream->Read(reinterpret_cast<uint8*>(&majorVersionFromFile), sizeof(int32));
				//int32 minorVersionFromFile=0;
				//ALLM_INFO((L"reading minorVersion"));
				//stream->Read(reinterpret_cast<uint8*>(&minorVersionFromFile), sizeof(int32));
			*/
			int32 majorVersion = int32(v[0]);
			int32 minorVersion = int32(v[1]);
			writeTo.WriteStruct(majorVersion); // major version
			writeTo.WriteStruct(minorVersion); // minor version			

			Xml::ElementCollection::iterator end = element->SubElements.end();
			for (Xml::ElementCollection::iterator i = element->SubElements.begin(); i != end; ++i)
			{
				RecursiveWrite((*i), writeTo);
			}

			datasize = writeTo.Seek(0, FileIO::File::SeekFrom::Cur) - position - sizeof(datasize); // calculate data size
			writeTo.Seek(position, FileIO::File::SeekFrom::Top); // move to the place that is reserved for datasize
			writeTo.WriteStruct(datasize); // overwrite datasize
			writeTo.Seek(0, FileIO::File::SeekFrom::End);
		}

		// irreversible converting
		void ExportStructuredBinaryFromXml(const wchar_t* filename, const wchar_t* output)
		{
			Xml::Parser parser;
			parser.ParseFromFile(filename);

			FileIO::File writeTo(output, FileIO::File::Mode::WriteMode);

			// write header 
			StructuredBinaryStreamHeader header;
			writeTo.WriteStruct(header);

			Xml::Element* allm = parser.GetElements().front();
			assert(allm->Name == L"allm");
			for (Xml::ElementCollection::iterator i = allm->SubElements.begin(); i != allm->SubElements.end(); ++i)
				RecursiveWrite((*i), writeTo);

			writeTo.Close();
		}

	}
}