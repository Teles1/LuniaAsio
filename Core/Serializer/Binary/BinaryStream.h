#pragma once

#include <Core/Serializer/Serializer.h>
#include <Core/IStream.h>
#include <Core/com_ptr.h>

namespace Lunia {
	namespace Serializer {

		com_ptr<IRefCountedStreamWriter> CreateBinaryStreamWriter(Lunia::IStreamWriter& stream);
		com_ptr<IRefCountedStreamReader> CreateBinaryStreamReader(Lunia::IStreamReader& stream);

		com_ptr<IRefCountedStreamWriter> CreateBinaryStreamWriter(com_ptr<Lunia::IRefCountedStreamWriter> stream);
		com_ptr<IRefCountedStreamReader> CreateBinaryStreamReader(com_ptr<Lunia::IRefCountedStreamReader> stream);

		com_ptr<IRefCountedStreamWriter> CreateStructuredBinaryStreamWriter(Lunia::IStreamWriter& stream);
		com_ptr<IRefCountedStreamReader> CreateStructuredBinaryStreamReader(Lunia::IStreamReader& stream);

		com_ptr<IRefCountedStreamWriter> CreateStructuredBinaryStreamWriter(com_ptr<Lunia::IRefCountedStreamWriter> stream);
		com_ptr<IRefCountedStreamReader> CreateStructuredBinaryStreamReader(com_ptr<Lunia::IRefCountedStreamReader> stream);

		/** Xml exporter (export means irreversible converting) */
		void ExportStructuredBinaryFromXml(const wchar_t* filename, const wchar_t* output);
	}
}