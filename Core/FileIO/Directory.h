#pragma once
#include "FileBase.h"

namespace Lunia {
	namespace FileIO {

		class Directory
		{
		public:
			/**
			retrieves number of files in the directory

			@param path		a physical path(directory) of the directory
			@return				number of files in the directory
			*/
			static int Count(const wchar_t* path);
			static int Count(String& path) { return Count(path.c_str()); }


			/**
			Retrieves the current directory for the current process.
			*/
			static String GetCurrentDirectory();

			/**
			Changes the current directory for the current process.

			@param path		pointer to a string that specifies the path to the new current directory.
								This parameter may be a relative path or a full path. In either case, the full path of the specified directory is calculated and stored as the current directory.
			*/
			static void SetCurrentDirectory(const wchar_t* path);
			static void SetCurrentDirectory(String& path) { SetCurrentDirectory(path.c_str()); }

			/**
			Creates a new directory. the function applies a inherited security descriptor to the new directory.

			@param path		a string that specifies the path to create the new directory.
			@remarks this function creates directories with recursive way.
			*/
			static void Create(const wchar_t* path);
			static void Create(String& path) { Create(path.c_str()); }

			/**
			Determines whether the specified directory exists.

			@param	path		a name of the directory to search for.
			@return @b true if the directory exists on the specified path; otherwise, @b false
			*/
			static bool Exists(const wchar_t* path);
			static bool Exists(String& path) { return Exists(path.c_str()); }

			/**
			Constructor

			@param path		path of the directory.
			*/
			Directory(String& path);
			Directory(const wchar_t* path);
			~Directory();

			/**
			Open the specified directory to list files recursively

			@param path		path to be opened
			*/
			void Open(const wchar_t* path);
			void Open(String& path) { Open(path.c_str()); }

			/**
			Open zip file and treat as directory

			@param path		zip file to open
			*/
			void OpenZip(const wchar_t* path);

			/**
			Close and release all directory information
			*/
			void Close();

		private:
			String path;
			FileInfoCollection files;

		public:

			/**
			Retrieve all files in the directory

			@return				list of files
			*/
			virtual FileInfoCollection& GetFiles();
		};


	}
}