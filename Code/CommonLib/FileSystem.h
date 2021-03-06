#pragma once

namespace fastbird
{
	typedef char* BinaryData;
	class FileSystem
	{	
		// working directory.
		static std::string mRootAbs; 
	
	public:

		static void Initialize();
		static void Finalize();
		static int CompareLastFileWrite(const char* filepath1, const char* filepath2);
		static BinaryData ReadBinaryFile(const char* filepath, std::streamoff& outLength);
		static void FinishBinaryFile(BinaryData data);
		static void SaveBinaryFile(const char* filepath, BinaryData data, size_t length);
		static bool SecurityOK(const char* filepath);

		static bool IsFileExisting(const char* filepath);
		static void Rename(const char* prev, const char* newname);
		static void DelFile(const char* file);
		static void NewFile(const char* filepath);
		static void CreateFolder(const char* folderPath);
		static const char* GetCWD();
		static std::string OpenFile(HWND hwnd, char* filter);
	};
}