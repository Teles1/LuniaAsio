#include <Info/Info/Items/CompressedItemInfoManager.h>
#include <Core/FileIO/FileStream.h>

namespace Lunia {
	namespace XRated {
		namespace Database {
			namespace Info {
				void CompressedItemInfoManager::Init()
				{
					Items.clear();
					CategoryList.clear();
					UnidentifiedItems.clear();
					eventUnidentifiedItems.clear();
					CategoryList.clear();
					SetItemList.clear();
					ComposeItemList.clear();
					ComposeGradeInfos.clear();

					LoadBinaryData();
					LoadData();
				}

				void CompressedItemInfoManager::LoadBinaryData() {
					Resource::SerializerStreamReader reader = Resource::ResourceSystemInstance().CreateDefaultDeserializer(L"Database/CompressedItemInfos.b");
					reader->Begin(L"AllM::XRated::Database::Info::ItemInfoManager");
					reader->Read(L"compressedItems", CompressedItems);
					reader->Read(L"compressedUnidentifiedItems", UnidentifiedItemInfoCompressed); // name doens't match with variable to keep allm's standart ;-; i dont lub it
					reader->Read(L"CategoryList", CategoryList);
					reader->Read(L"SetItemList", SetItemList);
					reader->Read(L"ComposeItemList", ComposeItemList);
					reader->Read(L"ComposeGradeInfos", ComposeGradeInfos);

					compressedItemsCBF = Resource::ResourceSystemInstance().CreateStreamReader(L"./Database/ItemInfos.cbf");
				}

				void CompressedItemInfoManager::LoadData()
				{
					compressedItemsCBF->SetReadCursor(0, Lunia::IStream::Begin);
					/* Items */
					uint8* BufferItems = reinterpret_cast<uint8*>(new char[4]);
					compressedItemsCBF->Read(BufferItems, 4);
					size_t BufferSize = *(int*)BufferItems;
					std::vector<uint8> IBufferItems;
					IBufferItems.resize(BufferSize);
					compressedItemsCBF->Read(&IBufferItems[0], (uint32)BufferSize);
					IndexedItemsCompressed = IBufferItems;

					/* Unidentified */
					uint8* BufferUnidentified = reinterpret_cast<uint8*>(new char[4]);
					compressedItemsCBF->Read(BufferUnidentified, 4);
					size_t BufferUnidentifiedSize = *(int*)BufferUnidentified;
					std::vector<uint8> IBufferUnidentified;
					IBufferUnidentified.resize(BufferUnidentifiedSize);
					compressedItemsCBF->Read(&IBufferUnidentified[0], (uint32)BufferUnidentifiedSize);
					IndexedUnidentifiedCompressed = IBufferUnidentified;
				}

				/* Items */
				bool CompressedItemInfoManager::GetItems(const uint32& templateOffset) {
					compressedItemsCBF = new FileIO::RefCountedMemoryStreamReader(IndexedItemsCompressed.data(), (uint32)IndexedItemsCompressed.size());
					compressedItemsCBF->SetReadCursor(templateOffset, Lunia::IStream::Begin);
					std::vector<uint8> Buffer;
					Buffer.resize(4);
					size_t COMPRESSED_SIZE = 0;
					size_t UNCOMPRESSED_SIZE = 0;

					compressedItemsCBF->Read(&Buffer[0], 4);
					COMPRESSED_SIZE = *(int*)&Buffer[0];
					compressedItemsCBF->Read(&Buffer[0], 4);
					UNCOMPRESSED_SIZE = *(int*)&Buffer[0];

					/* Setting buffer input and output sizes*/
					std::vector<uint8> inBuf;
					inBuf.resize(COMPRESSED_SIZE + LZMA_PROPS_SIZE);
					std::vector<uint8> outBuf;
					outBuf.resize(UNCOMPRESSED_SIZE);

					compressedItemsCBF->Read(inBuf.data(), inBuf.size());

					/*decoding and decrypting the binary owo*/
					SRes res = LzmaUncompress(outBuf.data(), &UNCOMPRESSED_SIZE, inBuf.data() + LZMA_PROPS_SIZE, &COMPRESSED_SIZE, inBuf.data(), LZMA_PROPS_SIZE);

					Resource::SerializerStreamReader BlockDecrypted = Serializer::CreateBinaryStreamReader(new FileIO::RefCountedMemoryStreamReader(&outBuf[0], (uint32)UNCOMPRESSED_SIZE));
					BlockDecrypted->Read(L"ItemInfoManager", Items, false);
					return true;
				}

				ItemInfo* CompressedItemInfoManager::Retrieve(const uint32& hash) {
					if (CompressedItems.dataPosition.find(hash) == CompressedItems.dataPosition.end()) return nullptr;
					if (this->Items.find(hash) != this->Items.end()) return &Items[hash];
					if (GetItems(CompressedItems.dataPosition[hash])) {
						return &Items[hash];
					}
					return nullptr;
				}

				ItemInfo* CompressedItemInfoManager::Retrieve(const wchar_t* name) {
					return Retrieve(StringUtil::Hash(name));
				}

				/* Unidentified Items */
				bool CompressedItemInfoManager::GetUnidentifiedItems(const uint32& templateOffset) {
					compressedItemsCBF = new FileIO::RefCountedMemoryStreamReader(IndexedUnidentifiedCompressed.data(), (uint32)IndexedUnidentifiedCompressed.size());
					compressedItemsCBF->SetReadCursor(templateOffset, Lunia::IStream::Begin);
					std::vector<uint8> Buffer;
					Buffer.resize(4);
					size_t COMPRESSED_SIZE = 0;
					size_t UNCOMPRESSED_SIZE = 0;

					compressedItemsCBF->Read(&Buffer[0], 4);
					COMPRESSED_SIZE = *(int*)&Buffer[0];
					compressedItemsCBF->Read(&Buffer[0], 4);
					UNCOMPRESSED_SIZE = *(int*)&Buffer[0];

					/* Setting buffer input and output sizes*/
					std::vector<uint8> inBuf;
					inBuf.resize(COMPRESSED_SIZE + LZMA_PROPS_SIZE);
					std::vector<uint8> outBuf;
					outBuf.resize(UNCOMPRESSED_SIZE);

					compressedItemsCBF->Read(inBuf.data(), (uint32)inBuf.size());

					/*decoding and decrypting the binary owo*/
					SRes res = LzmaUncompress(outBuf.data(), &UNCOMPRESSED_SIZE, inBuf.data() + LZMA_PROPS_SIZE, &COMPRESSED_SIZE, inBuf.data(), LZMA_PROPS_SIZE);

					Resource::SerializerStreamReader BlockDecrypted = Serializer::CreateBinaryStreamReader(new FileIO::RefCountedMemoryStreamReader(&outBuf[0], (uint32)UNCOMPRESSED_SIZE));
					BlockDecrypted->Read(L"UnidentifiedItemInfoManager", UnidentifiedItems, false);
					return true;
				}

				UnidentifiedItemInfo* CompressedItemInfoManager::RetrieveUnidentifiedItem(const uint32& hash)
				{
					if (UnidentifiedItemInfoCompressed.dataPosition.find(hash) == UnidentifiedItemInfoCompressed.dataPosition.end()) return nullptr;
					if (this->UnidentifiedItems.find(hash) != this->UnidentifiedItems.end()) return &UnidentifiedItems[hash];
					if (GetUnidentifiedItems(UnidentifiedItemInfoCompressed.dataPosition[hash])) {
						return &UnidentifiedItems[hash];
					}
					return nullptr;
				}
				UnidentifiedItemInfo* CompressedItemInfoManager::RetrieveUnidentifiedItem(const wchar_t* id)
				{
					return RetrieveUnidentifiedItem(StringUtil::Hash(id));
				}
			}
		}
	}
}