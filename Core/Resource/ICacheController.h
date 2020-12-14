#pragma once

#include "ICache.h"

namespace Lunia {
	namespace Resource {

		/**
		Interface for controlling resource caches.

		@author juhnu
		@date 2006.11.24
		*/
		class ICacheController
		{
		public:
			virtual void AddCache(ICache* cache) = 0;
			virtual void RemoveCache(ICache* cache) = 0;

			struct Tag
			{
				enum type
				{
					Global = 1,
					Touched = 1 << 1, //this tag is set automatically set, when the asset is retrieved from cache.
					InUse = 1 << 2  //this tag is automatically set
				};
			};

			/**
			it make csv files.
			one file one chache...
			and make some directory
			*/
			virtual void PrintStatsFile() = 0;
			virtual void PrintStats() = 0;

			virtual int GetCacheCount() = 0;
			virtual const wchar_t* GetCacheName(int index) = 0;

			/**
			@param cacheName cache name or part of it.
			@return used memory of a given cache
			*/
			virtual uint32 GetMemoryUsage(const wchar_t* cacheName) = 0;

			/**
			tag ^= ((tag & readTagMask) == referenceRead & readTagMask) ? (tag ^ referenceWrite) & writeTagMask : 0;

			@param readTagMask comparison mask.
			@param referenceRead value used with the comparison.
			@param writeTagMask write mask.
			@param referenceWrite value to write.
			Frees untouched and not global assets: Free(Touched|Global, 0)
			*/
			virtual void Tag(uint32 readTagMask, uint32 referenceRead, uint32 writeTagMask, uint32 referenceWrite) = 0;

			/**
			Frees all asset with given tag pattern.

			(tag & tagMask) == (reference & tagMask) ? delete : nop;

			@param tagMask comparison mask.
			@param reference value used with the comparison.
			*/
			virtual void Free(uint32 tagMask, uint32 reference) = 0;

			/*const wchar_t* GetName() const=0;
			uint32 GetBytesAllocated() const=0;
			uint32 GetAssetCount() const=0;*/

			//helper methods
			void inline FreeNonUsed()
			{
				Free(Tag::InUse, 0);
			}

			void inline FreeEverything()
			{
				Free(0, 0);
			}

			void inline FreeNonGlobal()
			{
				Free(Tag::Global, 0);
			}

			void inline FreeNonGlobalAndNonUsed()
			{
				Free(Tag::Global | Tag::InUse, 0);
			}

			void inline FreeUntouchedAndNonGlobal()
			{
				Free(Tag::Touched | Tag::Global, 0);
			}

			void inline TagAllAsUntouched()
			{
				Tag(Tag::Touched, Tag::Touched, Tag::Touched, 0);
			}

			void inline TagTouchedAsGlobal()
			{
				Tag(Tag::Touched, Tag::Touched, Tag::Global, Tag::Global);
			}



		};

	}
}