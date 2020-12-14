#pragma once

#include "Database.h"

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {

				struct FilePosData : public Serializer::ISerializable
				{
					uint32 StartPos;
					uint32 EndPos;

					virtual void Serialize(Serializer::IStreamWriter& out) const;
					virtual void Deserialize(Serializer::IStreamReader& in);
				};

				class Loader : public ReferenceCounted
				{
					com_ptr<IRefCountedStreamReader> fileReader;
					uint32 fileOfEnd;
				public:
					//template <typename T> void MakeIndex(std::map<uint32, FilePosData>& table, T& data);
					//template <typename T> void MakeIndex(FilePosData& filePositon, const std::wstring& element, T& data);
					//template <typename T> void Read(const FilePosData& filePosition, T& result);
					//template <typename T> void Read(const FilePosData& filePosition, const std::wstring& element, T& result);
				public:
					Loader(const wchar_t* file);
					//~Loader();
				};

				//com_ptr<Loader> CreateLoader(const wchar_t* file);
			}
		}
	}
}