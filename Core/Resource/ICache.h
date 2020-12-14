#pragma once

namespace Lunia {
	namespace Resource {

		/**
		Common interface for all kind of data caches.

		@author juhnu
		@author 2006.11.22
		*/
		class ICache
		{
		public:

			class IAsset
			{
			public:
				/**
				@return name of the asset
				*/
				virtual const wchar_t* GetName() const = 0;

				/**
				@return memory usage of the asset in bytes.
				*/
				virtual uint32 GetMemoryUsage() const = 0;

				/**
				@param buffer buffer for description
				@param size size of the buffer
				@return true if has description, false otherwise. If false the buffer is not touched.
				*/
				virtual const wchar_t* GetDescription() const = 0;

				/**
				@return tag bit mask
				*/
				virtual uint32 GetTag() const = 0;
			};

			class IAssetVisitor
			{
			public:
				virtual void Visit(const IAsset& asset) = 0;
			};

			virtual void VisitAssets(IAssetVisitor& visitor) = 0;


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
			@return name of the cache
			*/
			virtual const wchar_t* GetName() const = 0;

			/**
			@return description of the cache
			*/
			virtual const wchar_t* GetDescription() const = 0;

			/**
			Called by Resource::CacheController periodically.

			@param dt elapsed timestep in seconds.
			*/
			virtual void Update(float dt) = 0;

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

		};
	}
}