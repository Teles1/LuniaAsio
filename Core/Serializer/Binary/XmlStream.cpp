#include "XmlStreamWriter.h"
#include "XmlStreamReader.h"
#include "XmlStream.h"
#include <Core/FileIO/FileStream.h>

namespace Lunia {
	namespace Serializer {

		com_ptr<IRefCountedStreamWriter> CreateXmlStreamWriter(Lunia::IStreamWriter& stream) {
		   return new XmlStreamWriter(stream);
		}

		com_ptr<IRefCountedStreamReader>  CreateXmlStreamReader(Lunia::IStreamReader& stream) {
		   return new XmlStreamReader(stream);
		}

		com_ptr<IRefCountedStreamWriter> CreateXmlStreamWriter(com_ptr<Lunia::IRefCountedStreamWriter> stream) {
		  return new XmlStreamWriter(stream);
		}
		
		com_ptr<IRefCountedStreamReader>  CreateXmlStreamReader(com_ptr<Lunia::IRefCountedStreamReader> stream) {
		  return new XmlStreamReader(stream);
		}

		void SaveXml(const String& filename, const ISerializable& instance, const String& instanceName)
		{
			FileIO::FileStreamWriter file(filename.c_str());
			com_ptr<Serializer::IRefCountedStreamWriter> writer=CreateXmlStreamWriter(file);
			writer->Write(instanceName.c_str(), instance);
		}
		
	}
}