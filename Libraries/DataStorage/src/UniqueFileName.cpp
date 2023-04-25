#pragma once
#include <iostream>
#include <Windows.h>
#include "stdafx.h"
#include <ctime>
#include "../include/Helper/UniqueFileName.h"
#include <sstream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace DataStorageAPI
{
	string UniqueFileName::GetUniqueFilePath(string directory, string fileName, string fileExtension)
	{
		if (directory.empty()) throw std::invalid_argument("Directory cannot be null");
		if (fileName.empty()) throw std::invalid_argument("Filename cannout be null");
		if (fileExtension.empty()) throw std::invalid_argument("FileExtension cannot be null");

		fileExtension = fileExtension.rfind(".", 0) == 0 ? fileExtension : "." + fileExtension;
		fs::path uniqueFilePath = directory;
		uniqueFilePath /= (fileName + fileExtension);

		return GetUniqueFilePath(uniqueFilePath.string());
	}


	bool UniqueFileName::DirectoryExists(string directory)
	{
		auto fileAtr = GetFileAttributesA(directory.c_str());
		if (fileAtr == INVALID_FILE_ATTRIBUTES)
			return false;

		return (fileAtr & FILE_ATTRIBUTE_DIRECTORY);
	}

	string UniqueFileName::GetUniqueFilePath(string filePath)
	{
		fs::path uniqueFilePath{ filePath };
		if (!DirectoryExists(uniqueFilePath.parent_path().string())) {
			std::cout << "Directory doesn't exists!!" << std::endl;
			return "Directory doesn't exists!!";
			//throw std::invalid_argument("Directory doesn't exists!!");
		}

		auto fileName = uniqueFilePath.stem().string();
		auto fileExtension = uniqueFilePath.extension().string();
		for (int i = 1; exists(uniqueFilePath); ++i) {
			std::ostringstream fn;
			fn << fileName << "(" << i << ")" << fileExtension;
			uniqueFilePath.replace_filename(fn.str());
		}

		// create the file so there won't exist two files with same name.
		auto uniquePath = uniqueFilePath.string();
		std::ofstream outfile(uniquePath);
		outfile.close();

		return uniquePath;
	}

	string UniqueFileName::GetUniqueFilePathUsingDirectory(string directory, string fileExtension)
	{
		if (directory.empty()) throw std::invalid_argument("Directory cannot be null");

		std::time_t currentTime = std::time(nullptr);
		struct tm currentDate;
		localtime_s(&currentDate, &currentTime);
		std::ostringstream fileName;
		fileName << "Documents-" << 1900 + currentDate.tm_year << currentDate.tm_mon + 1 << currentDate.tm_mday << "-"
			<< currentDate.tm_hour << currentDate.tm_min << currentDate.tm_sec;

		fs::path uniqueFilePath = directory;
		fileExtension = fileExtension.rfind(".", 0) == 0 ? fileExtension : "." + fileExtension;
		uniqueFilePath /= (fileName.str() + fileExtension);

		return GetUniqueFilePath(uniqueFilePath.string());
	}

	string UniqueFileName::GetRelativeFilePath(string absolutePath)
	{
		fs::path path{ absolutePath };
		return path.relative_path().string();
	}

	bool UniqueFileName::CheckFilePathExists(string filePath)
	{
		return fs::exists(filePath);
	}
}
