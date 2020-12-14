#pragma once
#include <Core/Serializer/Serializer.h>

namespace Lunia {
	namespace Serializer {
		com_ptr<IRefCountedStreamWriter> CreateXmlStreamWriter(IStreamWriter& stream);
		com_ptr<IRefCountedStreamReader>  CreateXmlStreamReader(IStreamReader& stream);

		com_ptr<IRefCountedStreamWriter> CreateXmlStreamWriter(com_ptr<IRefCountedStreamWriter> stream);
		com_ptr<IRefCountedStreamReader>  CreateXmlStreamReader(com_ptr<IRefCountedStreamReader> stream);

		void SaveXml(const String& filename, const ISerializable& instance, const String& instanceName);
	}
}