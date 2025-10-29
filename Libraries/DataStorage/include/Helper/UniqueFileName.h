// @otlicense

#pragma once
#ifndef INCLUDE_UNIQUEFILENAME_H_
#define INCLUDE_UNIQUEFILENAME_H_

#include <iostream>
#include <string>

using string = std::string;

namespace DataStorageAPI
{
	class __declspec(dllexport) UniqueFileName
	{

	public:
		 string GetUniqueFilePath(string directory, string fileName, string fileExtension);
		 string GetUniqueFilePath(string filePath);
		 string GetUniqueFilePathUsingDirectory(string directory, string fileExtension = ".dat");
		 string GetRelativeFilePath(string absolutePath);
		 bool CheckFilePathExists(string filePath);

	private:
		bool DirectoryExists(std::string directory);
	};
};

#endif
