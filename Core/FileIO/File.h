#pragma once
#include "FileBase.h"

namespace Lunia {
	namespace FileIO {

		/**
		@brief Provides static methods for management of files, and open a file

		@author ALKEE
		*/
		class File
		{
		private:
			HANDLE handle;
			String filename;

		private:
			File& operator=(const File&) { return *this; }

		public:

			enum SeekFrom { Top = FILE_BEGIN, End = FILE_END, Cur = FILE_CURRENT };
			enum Mode {
				ReadMode = 0x1,
				WriteMode = 0x2,
				AppendMode = 0x6
			};

			/**
			@brief Delete a file

			@param filename	a file name to be deleted
			*/
			static void Delete(const wchar_t* filename);
			static void inline Delete(const String& filename) { return Delete(filename.c_str()); }

			static void DeleteDirectory(const wchar_t* fileName);
			static void inline DeleteDirectory(const String& fileName) { return DeleteDirectory(fileName.c_str()); }


			static void Copy(const wchar_t* existFilename, const wchar_t* newFilename, bool failExist);
			static void inline Copy(const String& existFilename, const String& newFilename, bool failExist) { return Copy(existFilename.c_str(), newFilename.c_str(), failExist); }


			/**
			@brief Retrieves attributes for a specified file or directory

			@param filename	a file name that specifies the name of a file or directory
			*/
			static FileInfo::FileAttributes GetAttributes(const wchar_t* filename);
			static inline FileInfo::FileAttributes GetAttributes(const String& filename) { return GetAttributes(filename.c_str()); }

			/**
			@brief ReSetting attributes for a specified file or directory

			@param fileName a file name that specifies the name of a file or directory
			@parma attribute a changed set attribe.
			*/
			static bool SetAttributes(const wchar_t* fileName, FileInfo::FileAttributes attribute);
			static bool inline SetAttributes(const String& filename, FileInfo::FileAttributes attribute) { return SetAttributes(filename.c_str(), attribute); }


			/**
			@brief Determines whether the specified file exists.

			@param filename	a file name which is confirmed
			@return @b true if the file exists on the specified path; otherwise, @b false
			*/
			static bool Exists(const wchar_t* filename);
			static bool inline Exists(const String& filename) { return Exists(filename.c_str()); }


			/**
			@brief Retrieve information of the given file.

			@param filename	a file name which is retrieved
			@param fileInfo	@a (out) #FileInfo structure
			*/
			static void GetFileInfo(const wchar_t* filename, FileInfo* fileInfo /* out */);
			static void inline GetFileInfo(const String& filename, FileInfo* fileInfo /* out */) { GetFileInfo(filename.c_str(), fileInfo); }

			/**
			@brief Constructor
			make instance with opening a file

			@param filename	a filename to be opened
			@param mode		open modes
			*/
			File(const String& filename, Mode mode);
			File(const wchar_t* filename, Mode mode);
			File();

			virtual ~File();

			/**
			@brief Creates or opens a file.

			@param	filename	a file name to be opened
			@param mode		open modes
			*/
			virtual void Open(const wchar_t* filename, Mode mode);
			virtual void inline Open(const String& filename, Mode mode) { Open(filename.c_str(), mode); }


			/**
			@brief Moves a file pointer a number of bytes from a given position (origin)

			@param offset		offset from position
			@param origin		basis of offset
			@return				current offset of the file
			*/
			virtual long Seek(long offset, SeekFrom origin);


			long inline GetSize() { long cur = Seek(0, Cur); long end = Seek(0, End); Seek(cur, Top); return end; }


			/**
			@brief Read data block from current file pointer. after read, file pointer is moved to end of data

			@param buffer		buffer that is written
			@param size			size of data in bytes
			@return				read byte, and returns 0 when the file pointer is on end of file
			*/
			virtual int Read(unsigned char* buffer, long size);

			/**
			@brief Reads a container to file
			*/
			template <typename fc> int inline Read(fc& c, int count)
			{
				unsigned char buffer[sizeof(fc::value_type)]; //hope this works..

				int lastReturn(0);
				for (int i = 0; i < count; ++i) {
					lastReturn = Read(buffer, sizeof(typename fc::value_type));
					c.push_back(*reinterpret_cast<typename fc::value_type*>(buffer));
				}
				return lastReturn;
			}

			/**
			@brief Reads a vector to the file
			*/
			template <typename fc> int inline Read(std::vector<fc>& v, int count)
			{
				int size = int(v.size());
				v.resize(size + count);
				return Read(reinterpret_cast<unsigned char*>(&v[size]), sizeof(std::vector<fc>::value_type) * long(count)) != 0;
			}

			/**
			@brief Write data block to current file pointer. after write, file pointer is moved to end of data

			@param buffer		buffer that is read
			@param buffer		size of buffer in bytes
			*/
			void Write(const unsigned char* buffer, long size);

			template <typename fc> void inline WriteStruct(fc& s) {
				Write(reinterpret_cast<unsigned char*>(&s), sizeof(fc));
			}
			/**
			@brief  Writes a container to file
			*/
			template <typename fc> void inline Write(const fc& c)
			{
				auto iend = c.end();
				for (auto i = c.begin(); i != iend; ++i)
				{
					Write(reinterpret_cast<const unsigned char*>(&(*i)), sizeof(fc::value_type));
				}
			}

			/**
			@brief  Writes a vector to the file
			
			template <typename fc> void Write(const std::vector<fc>& v)
			{
				//Write(reinterpret_cast<unsigned char*>(&v[0]), sizeof(std::vector<fc>::value_type) * long(vertices.size()));
			}
			*/

			/**
			@brief Flush internal buffer to file if the file is under cache-control
			*/
			virtual void Flush();


			/**
			@brief Close a file.
			*/
			virtual void Close();


			/**
			@brief Retrieve the file handle to be able to access directly

			@return handle value, if a file is not opened NULL will be returned.
			*/
			HANDLE GetHandle();
		};
	}
}