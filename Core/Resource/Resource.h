#pragma once

#include <Core/Serializer/Serializer.h>

#include "BinaryLoader.h"
#include "ITextLoaderListener.h"
#include "IChunkLoaderListener.h"
#include "ICacheController.h"

#include "../IStream.h"
#include <Core/Utils/Locator.h>
#include "../com_ptr.h"

namespace Lunia {
	namespace Resource {


		typedef com_ptr<IRefCountedStreamWriter> StreamWriter;
		typedef com_ptr<IRefCountedStreamReader> StreamReader;

		typedef com_ptr<Serializer::IRefCountedStreamWriter> SerializerStreamWriter;
		typedef com_ptr<Serializer::IRefCountedStreamReader> SerializerStreamReader;

		typedef IRefCountedStreamReader IStream;

		class ResourceException : public Exception
		{
		public:
			ResourceException(const wchar_t* desc) : Exception(desc) {}
		};

		class ResourceNotFoundException : public ResourceException
		{
			static_wstring<> filename;
		public:
			inline const wchar_t* GetFileName() { return filename.c_str(); }
		public:
			ResourceNotFoundException(const wchar_t* desc, const wchar_t* pFilename)
				: ResourceException(desc), filename(pFilename) {}
		};

		/**
		Mostly Thread Safe. AddPath function is not thread safe so just call it in the beginning once, before anything else.

		Resource System

		@author juhnu
		@date   2003.12.01

		Game should use this API to loader resources, do NOT use FileIO, Zip, Xml directly! This API will
		make it transparent for user, if the file is in normal directory tree, in the zip or some other location. It searchers
		files along the paths given, Yuu can set path to a .zip file (AddPath(some.zip)), then files inside the zip can be accessed
		as they were along normal path.


		DONE

		[date fixed]  [date reported]  [description]

		2005.11.17 2004.03.22 Added support for the zipped directories. (you can zip a directory and it's used automatically) - juhnu
		2005.06.01    Added support for the custom stream factories, added support for the multiple event listeners and a method to signal resource updates.
		2005.xx.xx    Added methods for the creating serializers
		2004.04.13    support $path, $file constants. Implemented as / and |,  if you start value with /, its replaced with the xml file path, | is replaced by xml file path and file name
		2004.03.22    2004.03.31 wrapper for deprecated binary loader (not really needed, I changed sources to use new one -juhnu)
		2004.03.26    2004.03.22 Iteration control with enum return value is not working Normal, SkipChildren etc, fix. -juhnu
		2004.05.22    2004.03.22       wrapper for deprecated ascii loader -juhnu

		*/

		class IResource {

		public:
			class IEventListener
			{
			public:
				virtual ~IEventListener() {}

				//@deprecated  you may increase progress variables here and render loading prgress
				inline virtual void Completed(const wchar_t*) {}

				inline virtual void OnResourceChanged(const wchar_t*) {}
				inline virtual void OnResourceOpened(const wchar_t*) {}
			};

			virtual ICacheController& GetCacheController() = 0;

			/**
			You can implement this interface in case you want to provide a custom stream factory for some protocol type
			*/
			class IStreamFactory {

			public:
				virtual ~IStreamFactory() {}
				virtual StreamReader CreateStreamReader(const wchar_t* name) = 0;
				virtual StreamWriter CreateStreamWriter(const wchar_t* name) = 0;
				virtual bool         Exists(const wchar_t* name) const = 0;

			};

			//lets remove these, do not use
			int CurrentProgress;
			//lets remove these, do not use	
			int MaximumProgress;

			//@deprecated, do not use
			virtual void RaiseCompleteEvent(const wchar_t* message) = 0;

			virtual void RaiseResourceChangedEvent(const wchar_t* resourceName) = 0;
			virtual void RaiseResourceOpenedEvent(const wchar_t* resourceName) = 0;


			/**
			Adds an event listener. Remember to call RemoveEventListener() when you destroy the class

			@param listener pointer to the user specified listener
			*/
			virtual void AddEventListener(IEventListener* listener) = 0;


			/**
			Removes an event listener, which was registered to the resource system in prior call to AddEventListener()

			@param listener pointer to the user specificed listener
			*/
			virtual void RemoveEventListener(IEventListener* listener) = 0;


			/**
			Returns the full path for the resource. This method doesn't throw but returns the given filename in case the path couldn't be found.

			@todo.. should this throw an exception instead?
			@return full path of the resource. If the path couldn't be found buffer will contain given name
			*/
			virtual void GetFullName(const wchar_t* name, wchar_t* buffer, size_t maximumSize) const = 0;

			/**
			See above.
			*/
			inline std::wstring GetFullName(const wchar_t* name) const {
				wchar_t buffer[4096];
				GetFullName(name, buffer, 4096);
				return buffer;
			}

			virtual void GetConsolidatedName(const wchar_t* name, wchar_t* buffer, size_t maximumSize) const = 0;

			/**
			See above.
			*/
			inline std::wstring GetConsolidatedName(const wchar_t* name) const {
				wchar_t buffer[4096];
				GetConsolidatedName(name, buffer, 4096);
				return buffer;
			}

			/**
			@return the primary data path, c:/program files/lunia/data/ for example.
			*/
			virtual void GetPrimaryDataPath(wchar_t* buffer, size_t size) const = 0;

			/**
			See above.
			*/
			inline std::wstring GetPrimaryDataPath() const {
				wchar_t buffer[4096];
				GetPrimaryDataPath(buffer, 4096);
				return buffer;
			}

			/**
			Returns the relative path for the resource. This method doesn't throw but returns the given filename in case the path couldn't be found.

			@todo.. should this throw an exception instead?
			@return relative path of the resource. If the path couldn't be found buffer will contain given name
			*/
			virtual void GetRelativeName(const wchar_t* name, wchar_t* buffer, size_t maximumSize) const = 0;

			/**
			See above.
			*/
			inline std::wstring GetRelativeName(const wchar_t* name) const {
				wchar_t buffer[4096];
				GetRelativeName(name, buffer, 4096);
				return buffer;
			}

			/**
			Adds a stream factory to use with specified protocol-prefix.

			prefix for the disc based files is "file" if filename doesn't have prefix, it's assumed to be "file"
			prefix for the editor could be "editor" for example

			Example:

			class MemoryBased: public IResource::IStreamFactory;
			MemoryBased myFactory;
			AddStreamFactory(L"editor",&myFactory);

			now all the paths starting with the editor:// will be directed to this custom factory.

			CreateStreamWriter(L"editor://blabla");

			@param prefix protocol prefix that is used to identify path names, which should created with the factory being registered.
			@param listener pointer to a stream factory.
			*/
			virtual void AddStreamFactory(const wchar_t* prefix, IStreamFactory* listener) = 0;

			/**
			Remove stream factory listener.

			@param listener pointer to a stream factory.
			*/
			virtual void RemoveStreamFactory(IStreamFactory* listener) = 0;

			virtual com_ptr<IRefCountedStreamWriter> CreateStreamWriter(const wchar_t* target) = 0;
			virtual com_ptr<IRefCountedStreamReader> CreateStreamReader(const wchar_t* source) = 0;

			virtual SerializerStreamWriter CreateSerializerXmlStreamWriter(const wchar_t* target) = 0;
			virtual SerializerStreamReader CreateSerializerXmlStreamReader(const wchar_t* target) = 0;

			virtual SerializerStreamWriter CreateSerializerBinaryStreamWriter(const wchar_t* target) = 0;
			virtual SerializerStreamReader CreateSerializerBinaryStreamReader(const wchar_t* target) = 0;

			virtual SerializerStreamWriter CreateSerializerStructuredBinaryStreamWriter(const wchar_t* target) = 0;
			virtual SerializerStreamReader CreateSerializerStructuredBinaryStreamReader(const wchar_t* target) = 0;

			virtual SerializerStreamWriter CreateDefaultSerializer(const wchar_t* target) = 0;
			virtual SerializerStreamReader CreateDefaultDeserializer(const wchar_t* target) = 0;

			/**
			Returns the file extension for binary serialized files
			@author kim
			@date 2006.08.01

			@return file extension of the binary serialized files
			*/
			virtual const wchar_t* GetSerializerBinaryExtension() const = 0;

			/**
			Clear Paths and Set new Primary Data Path
			@author caspian
			@date 2008.03.03

			*/
			virtual void Reset(const wchar_t* startUpPath = L"") = 0;

			/**
			Adds path where files can be looked to the resource system, can be a directory or an archived file

			GetResourceSystemInstance().AddPath(L"data")'
			GetResourceSystemInstance().AddPath(L"assets.zip")'
			GetResourceSystemInstance().AddPath(L"images.zip")'

			@param path name of the directory or archived file (.zip), u can use / or \ as directory separator, but please use /
			*/
			virtual void AddPath(const wchar_t* path) = 0;

			/**
			See above.
			*/
			inline void AddPath(const std::wstring& path) { this->AddPath(path.c_str()); }

			/**
			Checks whether the resource exists or not.

			@return true if resource exists, false otherwise.
			*/
			virtual bool Exists(const wchar_t* target) const = 0;


			/**
			parses standard (now xml based) file and provides you callbacks on the elements(tags) you wanted, ignores
			unknown elements.

			@param name name of the file
			@param elements list of the wanted elements
			@param elementCount count of the elements
			@param listener pointer to the listener interface

			*/
			virtual void Load(const wchar_t* name, const wchar_t** elements, int elementCount, ITextLoaderListener* listener) = 0;

			/**
			See above.
			*/
			inline void Load(const std::wstring& name, const wchar_t** elements, int elementCount, ITextLoaderListener* listener) { Load(name.c_str(), elements, elementCount, listener); }

			/**
			Parses standard chunk based file and provides you convenient callbacks on the chunks you wanted, ignores
			unknown chunks

			@param name name of the file
			@param chunkIDs list of the wanted chunk ids
			@param chunkIDCount count of the chunk ids
			@param listener pointer to the listener interface
			*/
			virtual void Load(const wchar_t* name, unsigned int* chunkIDs, unsigned int chunkIDCount, IChunkLoaderListener* listener) = 0;

			/**
			See above.
			*/
			inline void Load(const std::wstring& name, unsigned int* chunkIDs, unsigned int chunkIDCount, IChunkLoaderListener* listener) { Load(name.c_str(), chunkIDs, chunkIDCount, listener); }

			/**
			Provides raw binary interface to handle the file
			*/
			virtual com_ptr<IStream>  OpenStream(const wchar_t* name) = 0;

			/**
			See above.
			*/
			inline com_ptr<IStream>  OpenStream(const std::wstring& name) { return OpenStream(name.c_str()); }

			/**
			Must be called by the client.
			*/
			virtual void Update(float dt) = 0;

		};
		IResource& ResourceSystemInstance();
	}
}

