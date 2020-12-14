#pragma once
#include <Core/FileIO/File.h>
#include "ICacheController.h"
#include <Core/Utils/DateTime.h>

namespace Lunia {
	namespace Resource {

		/**
		Cache controller implementation.

		@author juhnu
		@date 2006.11.24
		*/
		class CacheController : public ICacheController
		{
			std::vector<ICache*> caches;

			float updateTime;

			struct Visitor :ICache::IAssetVisitor
			{
				uint32 total;
				Visitor() : total(0)
				{
				}
				~Visitor()
				{
					float totalmb = total / 1024.0f / 1024.0f;
					Logger::GetInstance().Warn(L"------------------------------------------------------------------------ Total Memory Used={0} MB", totalmb);
				}
				virtual void Visit(const ICache::IAsset& asset)
				{
					std::wstring tags = L"";

					if (asset.GetTag() & Tag::Global) tags += L"GLOBAL  ";
					if (asset.GetTag() & Tag::Touched) tags += L"TOUCHED  ";
					if (asset.GetTag() & Tag::InUse) tags += L"INUSE";

					tags += StringUtil::ToUnicode(asset.GetMemoryUsage() / 1024.0f);
					tags += L"KB";
					Logger::GetInstance().Warn(L"{0} [{1}] {2}", asset.GetName(), asset.GetDescription(), tags.c_str());
					total += asset.GetMemoryUsage();
				}
			};

			struct VisitorSize :ICache::IAssetVisitor
			{
				uint32 total;
				VisitorSize() : total(0)
				{
				}
				~VisitorSize()
				{
				}
				virtual void Visit(const ICache::IAsset& asset)
				{
					total += asset.GetMemoryUsage();
				}
			};

			struct VisitorForFile :ICache::IAssetVisitor
			{
				uint32 total;
				uint32 count;
				FileIO::File& file;
				VisitorForFile(FileIO::File& pFile) : file(pFile), total(0), count(0)
				{

				}
				~VisitorForFile()
				{
					float totalmb = total / 1024.0f;
					Write(fmt::format(L"Total Memory Used, {0}", totalmb));
					Write(fmt::format(L"Average Memory Used, {0}", (count == 0) ? 0 : static_cast<float>(totalmb) / static_cast<float>(count)));
				}
				virtual void Visit(const ICache::IAsset& asset)
				{
					std::wstring str = asset.GetName();

					str += L",";
					str += fmt::format(L"{0}", static_cast<float>(asset.GetMemoryUsage()) / 1024.0f);


					if (asset.GetTag() & Tag::Global) str += L",GLOBAL";
					if (asset.GetTag() & Tag::Touched) str += L",TOUCHED";
					if (asset.GetTag() & Tag::InUse) str += L",INUSE";

					Write(str);
					total += asset.GetMemoryUsage();
					++count;
				}
				void Write(const std::wstring& str)
				{
					std::string astring = StringUtil::ToASCII(str);
					astring += "\r\n";
					file.Write(reinterpret_cast<const unsigned char*>(astring.c_str()), astring.size());
				}

			};
		public:
			CacheController()
				:updateTime(0.0f)
			{
			}
			virtual void PrintStats()
			{
				for (size_t i = 0; i < caches.size(); ++i)
				{

					Logger::GetInstance().Warn(L"Cache={0} [{1}]", caches[i]->GetName(), caches[i]->GetDescription());
					Visitor visitor;
					caches[i]->VisitAssets(visitor);
				}
			}
			virtual void PrintStatsFile()
			{
				std::wstring directory = FileIO::Directory::GetCurrentDirectory() + L"/Statistics/";
				std::wstring statisticsDirectory = FileIO::Directory::GetCurrentDirectory() + L"/Statistics";
				if (!FileIO::Directory::Exists(statisticsDirectory))
				{

					FileIO::Directory::Create(statisticsDirectory);
				}

				directory += DateTime::Now().ToString(DateTime::Format::Unseparated);

				if (!FileIO::Directory::Exists(directory))
				{
					FileIO::Directory::Create(directory);
				}

				for (size_t i = 0; i < caches.size(); ++i)
				{
					std::wstring filename = directory + L"/" + caches[i]->GetName() + L".csv";

					FileIO::File file(filename, FileIO::File::Mode::WriteMode);
					std::wstring title = fmt::format(L"Cache={0} [{1}]", caches[i]->GetName(), caches[i]->GetDescription());
					std::wstring title2(L"Filename,memory,tags");
					VisitorForFile visitor(file);;
					visitor.Write(title);
					visitor.Write(title2);
					caches[i]->VisitAssets(visitor);
				}
			}
			virtual int GetCacheCount()
			{
				return static_cast<int>(caches.size());
			}
			virtual const wchar_t* GetCacheName(int index)
			{
				return caches[index]->GetName();
			}

			virtual uint32 GetMemoryUsage(const wchar_t* cacheName)
			{

				uint32 total = 0;
				for (size_t i = 0; i < caches.size(); ++i)
				{
					std::wstring name = caches[i]->GetName();
					if (name == cacheName)
					{
						VisitorSize visitor;
						caches[i]->VisitAssets(visitor);
						total += visitor.total;
					}
				}

				return total;
			}

			virtual void AddCache(ICache* cache)
			{
				caches.push_back(cache);
			}

			virtual void RemoveCache(ICache* cache)
			{
				for (size_t i = 0; i < caches.size(); ++i)
				{
					if (caches[i] == cache)
					{
						caches[i] = caches.back();
						caches.pop_back();
						return;
					}
				}
			}

			virtual void Update(float dt)
			{
				/*updateTime-=dt;
				if(updateTime > 0) return;

				updateTime = 1.0f;*/

				for (size_t i = 0; i < caches.size(); ++i)
				{
					caches[i]->Update(dt);
				}
			}

			virtual void Tag(uint32 readTagMask, uint32 referenceRead, uint32 writeTagMask, uint32 referenceWrite)
			{
				for (size_t i = 0; i < caches.size(); ++i)
				{
					caches[i]->Tag(readTagMask, referenceRead, writeTagMask, referenceWrite);
				}
			}

			virtual void Free(uint32 tagMask, uint32 reference)
			{
				//first pass
				for (size_t i = 0; i < caches.size(); ++i)
				{
					caches[i]->Free(tagMask, reference);
				}

				//we do the 2nd and 2rd pass only if the mask uses InUse tag and needs it to be zero.
				if (((tagMask & Tag::InUse) != 0) && ((reference & Tag::InUse) == 0))
				{

					//second pass to process assets that became unused after the first pass.
					for (size_t i = 0; i < caches.size(); ++i)
					{
						caches[i]->Free(tagMask, reference);
					}

					//third pass to process assets that became unused after the second pass.
					for (size_t i = 0; i < caches.size(); ++i)
					{
						caches[i]->Free(tagMask, reference);
					}
					//we assume we don't have third order inter-dependencies in the resources so we are done here.
				}
			}
		};

	}
}