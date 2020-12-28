#include <Info/Info/Items/CompressedItemInfoManager.h>
#include <Core/FileIO/FileStream.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {

				void CompressedItemInfoManager::Init(bool bForceXmlData)
				{
					Items.clear();
					CategoryList.clear();
					UnidentifiedItems.clear();
					eventUnidentifiedItems.clear();
					CategoryList.clear();
					SetItemList.clear();
					ComposeItemList.clear();
					ComposeGradeInfos.clear();

					if (bForceXmlData) {//Xml loading
						LoadXmlData();
					}
					else { //Binary loading
					 /* std::wstring fullLoading;
					 Lunia::Config::ConfigInstance().Get(L"database", L"fullLoading", fullLoading, L"1");
					 if (fullLoading == L"1")
						 ItemInfoManager::LoadBinaryData();
					 else */
						LoadCompressedBinary();
						//cbfreader = Lunia::Resource::ResourceSystemInstance().CreateStreamReader(L"./Database/ItemInfos.cbf");
						//cbfreader->SetReadCursor(0, Lunia::IStream::Begin);
						//Read(L"ItemInfos", Items);
						//Read(L"UnidentifiedItemInfo", UnidentifiedItems);
					}
				}

				void CompressedItemInfoManager::LoadCompressedBinary()
				{
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/CompressedItemInfos.b");
					reader->Begin(L"AllM::XRated::Database::Info::ItemInfoManager");
					reader->Read(L"compressedItems", CompressedItems);
					reader->Read(L"compressedUnidentifiedItems", UnidentifiedItemInfoCompressed); // name doens't match with variable to keep allm's standart ;-; i dont lub it
					reader->Read(L"CategoryList", CategoryList);
					reader->Read(L"SetItemList", SetItemList);
					reader->Read(L"ComposeItemList", ComposeItemList);
					reader->Read(L"ComposeGradeInfos", ComposeGradeInfos);
				}

				void CompressedItemInfoManager::SaveCompressedXml() {
					Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateSerializerXmlStreamWriter(L"./Database/CompressedItemInfos.xml");
					writer->Begin(L"AllM::XRated::Database::Info::ItemInfoManager");
					writer->Write(L"compressedItems", CompressedItems);
					writer->Write(L"compressedUnidentifiedItems", UnidentifiedItemInfoCompressed);
					writer->Write(L"CategoryList", CategoryList);
					writer->Write(L"SetItemList", SetItemList);
					writer->Write(L"ComposeItemList", ComposeItemList);
					writer->Write(L"ComposeGradeInfos", ComposeGradeInfos);
				}

				void CompressedItemInfoManager::SaveCompressedB() {

					Resource::SerializerStreamWriter writer = Resource::ResourceSystemInstance().CreateDefaultSerializer(L"./Database/CompressedItemInfos.b");
					writer->Begin(L"AllM::XRated::Database::Info::ItemInfoManager");
					writer->Write(L"compressedItems", CompressedItems);
					writer->Write(L"compressedUnidentifiedItems", UnidentifiedItemInfoCompressed);
					writer->Write(L"CategoryList", CategoryList);
					writer->Write(L"SetItemList", SetItemList);
					writer->Write(L"ComposeItemList", ComposeItemList);
					writer->Write(L"ComposeGradeInfos", ComposeGradeInfos);
				}

				void CompressedItemInfoManager::SaveItemsCbf() {
					uint32 bufSize(123845181);
					uint8* inBuf = (uint8*)malloc(bufSize);
					Lunia::Resource::StreamWriter temp = new Lunia::FileIO::RefCountedMemoryStreamWriter(inBuf, bufSize);
					Lunia::Resource::SerializerStreamWriter binary =
						Serializer::CreateBinaryStreamWriter(temp);
					std::cout << "Writing ItemInfo to inBuf ..." << std::endl;
					binary->Write(L"ItemInfo", Items);
					std::cout << "Done Writing ItemInfo to inBuf ..." << std::endl;
					//hexdump(binary->GetStream(), 1024);
					std::cout << "bufSize[" << bufSize << "]" << std::endl;
					/* std::wstring filename(L"./Database/inFile.cbf");
					FileIO::FileStreamWriter writer(filename.c_str());
					writer.Write(binary->GetStream(), bufSize); */

					std::cout << "Compressing data" << std::endl;
					//uint8* outBuf;
					//lzmaCompress(inBuf, bufSize, outBuf);
					delete[] inBuf;
				}

				Lunia::XRated::Database::Info::ItemInfo* CompressedItemInfoManager::Retrieve(uint32 hash)
				{
					ItemInfoMap::iterator ita = Items.find(hash);
					if (ita != Items.end())
						return &ita->second;
					else
					{
						return NULL;
					}
				}

				Lunia::XRated::Database::Info::ItemInfo* CompressedItemInfoManager::Retrieve(const wchar_t* id)
				{
					return Retrieve(Lunia::StringUtil::Hash(id));
				}

				Lunia::XRated::UnidentifiedItemInfo* CompressedItemInfoManager::RetrieveUnidentifiedItem(uint32 hash)
				{
					return NULL;
				}

				Lunia::XRated::UnidentifiedItemInfo* CompressedItemInfoManager::RetrieveUnidentifiedItem(const wchar_t* id)
				{
					return RetrieveUnidentifiedItem(Lunia::StringUtil::Hash(id));
				}

				bool CompressedItemInfoManager::Remove(Lunia::uint32 hash)
				{
					ItemInfoMap::iterator it = Items.find(hash);
					if (it != Items.end())
					{
						Items.erase(it);
						return true;
					}

					/*UnidentifiedItemInfoMap::iterator ita=UnidentifiedItems.find(hash);
					if (ita!=UnidentifiedItems.end())
					{
						UnidentifiedItems.erase(ita);
						return true;
					}*/

					return false;
				}

				bool CompressedItemInfoManager::Remove(const wchar_t* id)
				{
					return Remove(Lunia::StringUtil::Hash(id));
				}

			}
		}
	}
}