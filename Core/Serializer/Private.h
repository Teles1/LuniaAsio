#pragma once
#include "Core.h"
namespace Serializer{
	template <typename T>
	class ArrayDeserialize {
		T* buffer;
		unsigned int size;
	public:
		ArrayDeserialize(T* pBuffer, unsigned int pSize)
			: buffer(pBuffer), size(pSize) {}

		Deserialize(IStreamReader& in) {
			in.Begin(L"array");
			size_t fsize = in.GetCount(L"item");
			if (fsize >= size) {
				if (fsize != size)
					WARN("static array being deserialized has bigger size on file");
			}
			else {
				ERROR("static array being deserialized has smaller size on file");
				throw;
			}

			for (unsigned int i = 0; i < size; ++i) {
				T value;
				in.Read(L"item", value);
				buffer[i] = value;
			}
		}

	};
}