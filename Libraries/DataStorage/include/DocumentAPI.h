// @otlicense

#pragma once
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <set>
#include <bsoncxx/types/value.hpp>
#include "bsoncxx/builder/basic/document.hpp"
#include "bsoncxx/stdx/string_view.hpp"
#include <string>

using bsoncxx::types::value;
using Document = bsoncxx::builder::basic::document;

namespace DataStorageAPI
{
	class DocumentAPI
	{
	public:
		__declspec(dllexport) value InsertDocumentUsingGridFs(std::istream* source, const std::string &fileName);
		__declspec(dllexport) void GetDocumentUsingGridFs(value id, std::ostream* destination);
		__declspec(dllexport) value InsertDocumentUsingGridFs(bsoncxx::document::view docView, const std::string &fileName);
		__declspec(dllexport) void GetDocumentUsingGridFs(value id, uint8_t *&buffer, size_t &length);
		__declspec(dllexport) value InsertDocumentUsingGridFs(bsoncxx::document::view docView, const std::string &fileName, Document &metaDoc);
		__declspec(dllexport) void GetDocumentUsingGridFs(value id, uint8_t *&buffer, size_t &length, const std::string &fileName);
		__declspec(dllexport) value InsertBinaryDataUsingGridFs(const uint8_t* dataBuffer, size_t dataSize, const std::string &fileName);
		__declspec(dllexport) void DeleteGridFSData(value id, const std::string& fileName);


		/*
		__declspec(dllexport) value InsertDocumentToFileStorage(std::ifstream* source, std::string fileName);
		__declspec(dllexport) void GetDocumentFromFileStorage(value id, std::ifstream* destination);
		__declspec(dllexport) std::string GetDocumentPathFromFileStorage(value id);

		__declspec(dllexport) value InsertDocumentToDatabase(std::string fileName, std::string content);
		__declspec(dllexport) std::string GetDocumentFromDatabase(value id);

		__declspec(dllexport) value InsertDocumentPathToDatabase(std::string filePath, std::string fileName);
		__declspec(dllexport) std::string GetDocumentPathFromDatabase(value id);*/
	private:
		__declspec(dllexport) std::vector<std::string_view> insertElementsToMetadata(bsoncxx::document::view docView, Document &metaDoc);
		__declspec(dllexport) size_t getSizeOfElement(bsoncxx::document::element element);
	};
};
