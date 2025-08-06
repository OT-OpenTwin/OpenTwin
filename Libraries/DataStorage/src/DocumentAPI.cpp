#pragma once
#include "stdafx.h"
#include <iostream>
#include <fstream>
#include <string>
#include <cassert>

#include "..\include\DocumentAPI.h"
#include "..\include\Document\DocumentManager.h"
#include "..\include\Document\DocumentAccessBase.h"
#include "..\include\Enums\InsertTypeEnum.h"
#include "..\include\Connection\ConnectionAPI.h"

#include <mongocxx/client.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>
#include <bsoncxx/types/value.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/json.hpp>

using bsoncxx::types::value;
using bsoncxx::builder::basic::kvp;

std::int64_t DefaultChunkSize = 1024 * 255;

// Test class used during performance testing. This is not the actual implementation class.
namespace DataStorageAPI
{
	void DocumentAPI::GetDocumentUsingGridFs(value id, std::ostream* destination)
	{
		try
		{
			auto db = DataStorageAPI::ConnectionAPI::getInstance().getDatabase("ProjectsLargeData");
			auto bucket = db.gridfs_bucket();
	
			std::ofstream dest;
			std::ostream& destination1 = dest;

			bucket.download_to_stream(id, destination);
		}
		catch (std::exception& e)
		{
			std::cout << e.what();
		}
	}

	value DocumentAPI::InsertDocumentUsingGridFs(std::istream* source, const std::string &fileName)
	{
		try
		{
			auto db = DataStorageAPI::ConnectionAPI::getInstance().getDatabase("ProjectsLargeData");
			auto bucket = db.gridfs_bucket();

			mongocxx::options::gridfs::upload options{};

			//by default chuck size is 255kb = 255 * 1024, this can be changed
			//options.chunk_size_bytes(1024);

			// To add addition metadata information.
			/*bsoncxx::builder::basic::document fileMetadata{};
			fileMetadata.append(kvp("version", 2));
			options.metadata(fileMetadata.view());*/

			auto upload = bucket.upload_from_stream(fileName, source, options);
			return upload.id();
		}
		catch (std::exception& e)
		{
			std::cout << e.what();
			throw e;
		}
	}

	void DocumentAPI::GetDocumentUsingGridFs(value id, uint8_t *&buffer, size_t &length)
	{
		try
		{
			auto db = DataStorageAPI::ConnectionAPI::getInstance().getDatabase("ProjectsLargeData");
			auto bucket = db.gridfs_bucket();

			auto downloader = bucket.open_download_stream(id);
			length = downloader.file_length();

			buffer = new std::uint8_t[length];
			downloader.read(buffer, length);
			downloader.close();
		}
		catch (std::exception& e)
		{
			std::cout << e.what();
		}
	}

	void DocumentAPI::GetDocumentUsingGridFs(value id, uint8_t *&buffer, size_t &length, const std::string &fileName)
	{
		try
		{
			auto db = DataStorageAPI::ConnectionAPI::getInstance().getDatabase("Projects");
			mongocxx::options::gridfs::bucket bucketOptions = mongocxx::options::gridfs::bucket();
			bucketOptions.bucket_name(fileName);
			auto bucket = db.gridfs_bucket(bucketOptions);

			auto downloader = bucket.open_download_stream(id);
			length = downloader.file_length();

			buffer = new std::uint8_t[length];
			downloader.read(buffer, length);
			downloader.close();
		}
		catch (std::exception& e)
		{
			std::cout << e.what();
		}
	}

	value DocumentAPI::InsertDocumentUsingGridFs(bsoncxx::document::view docView, const std::string &fileName, Document &metaDoc)
	{
		try
		{
			auto db = DataStorageAPI::ConnectionAPI::getInstance().getDatabase("Projects");
			mongocxx::options::gridfs::bucket bucketOptions = mongocxx::options::gridfs::bucket();
			bucketOptions.bucket_name(fileName);
			auto bucket = db.gridfs_bucket(bucketOptions);

			mongocxx::options::gridfs::upload options{};

			//LOGIC TO FIT THE ELEMENTS IN METADATA DOCUMENT IN ASCENDING ORDER
			std::vector<std::string_view> insertedKeys = this->insertElementsToMetadata(docView, metaDoc);

			const uint8_t *buffer = reinterpret_cast<const uint8_t *>(docView.data());
			size_t size = docView.length();

			auto uploader = bucket.open_upload_stream(fileName, options);
			uploader.write(buffer, size);

			auto upload = uploader.close();

			//FILLING REFERENCE OF KEYS STORED IN GIRDFS IN METADATA DOCUMENT
			for (auto key = std::begin(insertedKeys); key != std::end(insertedKeys); ++key) {
				metaDoc.append(kvp(*key, upload.id()));
			}
			return upload.id();
		}
		catch (std::exception& e)
		{
			std::cout << e.what();
			throw e;
		}
	}
	value DocumentAPI::InsertDocumentUsingGridFs(bsoncxx::document::view docView, const std::string &fileName)
	{
		try
		{
			auto db = DataStorageAPI::ConnectionAPI::getInstance().getDatabase("ProjectsLargeData");
			auto bucket = db.gridfs_bucket();

			mongocxx::options::gridfs::upload options{};

			//by default chuck size is 255kb = 255 * 1024, this can be changed
			//options.chunk_size_bytes(1024);

			// To add addition metadata information.
			/*bsoncxx::builder::basic::document fileMetadata{};
			fileMetadata.append(kvp("version", 2));
			options.metadata(fileMetadata.view());*/

			const uint8_t *buffer = reinterpret_cast<const uint8_t *>(docView.data());
			size_t size = docView.length();

			auto uploader = bucket.open_upload_stream(fileName, options);
			uploader.write(buffer, size);

			auto upload = uploader.close();
			return upload.id();
		}
		catch (std::exception& e)
		{
			std::cout << e.what();
			throw e;
		}
	}
	value DocumentAPI::InsertBinaryDataUsingGridFs(const uint8_t* dataBuffer, size_t dataSize, const std::string &fileName)
	{
		try
		{
			auto db = DataStorageAPI::ConnectionAPI::getInstance().getDatabase("Projects");
			mongocxx::options::gridfs::bucket bucketOptions = mongocxx::options::gridfs::bucket();
			bucketOptions.bucket_name(fileName);
			auto bucket = db.gridfs_bucket(bucketOptions);

			mongocxx::options::gridfs::upload options{};

			auto uploader = bucket.open_upload_stream(fileName, options);
			uploader.write(dataBuffer, dataSize);

			auto upload = uploader.close();

			return upload.id();
		}
		catch (std::exception& e)
		{
			std::cout << e.what();
			throw e;
		}
	}
	void DocumentAPI::DeleteGridFSData(value id, const std::string &fileName)
	{
		try
		{
			auto db = DataStorageAPI::ConnectionAPI::getInstance().getDatabase("Projects");
			mongocxx::options::gridfs::bucket bucketOptions = mongocxx::options::gridfs::bucket();
			bucketOptions.bucket_name(fileName);
			auto bucket = db.gridfs_bucket(bucketOptions);

			bucket.delete_file(id);
		}
		catch (std::exception& e)
		{
			std::cout << e.what();
		}
	}
	std::vector<std::string_view> DocumentAPI::insertElementsToMetadata(bsoncxx::document::view docView, Document & metaDoc)
	{
		std::set<std::pair<size_t, std::string_view>> elemKeyValues;
		std::vector<std::string_view> insertedKeys;
		size_t sizeOfMetadata = 0;
		for (bsoncxx::document::element element : docView) {
				sizeOfMetadata += (element.keylen() + 16 + 7);
				elemKeyValues.emplace(this->getSizeOfElement(element), element.key());
		}

		std::string_view lastInsertedKey;
		for (auto const &keyValue : elemKeyValues) {
			bsoncxx::document::element tempElem = docView.find(keyValue.second).operator*();
			if ((metaDoc.view().length() + keyValue.first - 16 + sizeOfMetadata) < DocumentManager::MaxDocumentLength) {
				sizeOfMetadata -= (tempElem.keylen() + 16 + 7);
				metaDoc.append(kvp(tempElem.key(), tempElem.get_value()));
				lastInsertedKey = keyValue.second;
			}
			else {
				insertedKeys.push_back(keyValue.second);
			}
		}
		//CHECK TO MAKE SURE THE DOCUMENT IS NOT LARGER THAN MAXIMUM SET SIZE
		if (metaDoc.view().length() >= DocumentManager::MaxDocumentLength) {
			metaDoc.clear();
			for (auto const &keyValue : elemKeyValues) {
				if (keyValue.second.compare(lastInsertedKey)) {
					bsoncxx::document::element tempElem = docView.find(keyValue.second).operator*();
					metaDoc.append(kvp(tempElem.key(), tempElem.get_value()));
				}
				else {
					insertedKeys.push_back(keyValue.second);
					break;
				}
			}
		}
		return insertedKeys;
	}
	size_t DocumentAPI::getSizeOfElement(bsoncxx::document::element ele)
	{
		
		size_t elemSize{ 0 };
		switch (ele.type()) {
			case bsoncxx::type::k_array:
				elemSize = ele.get_array().value.length();
				break;
			case bsoncxx::type::k_binary:
				elemSize = ele.get_binary().size;
				break;
			case bsoncxx::type::k_bool:
				elemSize = sizeof(ele.get_bool().value);
				break;
			case bsoncxx::type::k_decimal128:
				elemSize = sizeof(ele.get_decimal128());
				break;
			case bsoncxx::type::k_utf8:
				elemSize = ele.get_utf8().value.length();
				break;
			case bsoncxx::type::k_oid:
				elemSize = ele.get_oid().value.k_oid_length;
				break;
			case bsoncxx::type::k_code:
				elemSize = ele.get_code().code.length();
				break;
			case bsoncxx::type::k_codewscope:
				elemSize = ele.get_codewscope().code.length();
			case bsoncxx::type::k_date:
				elemSize = sizeof(ele.get_date().to_int64());
				break;
			case bsoncxx::type::k_dbpointer:
				elemSize = ele.get_dbpointer().value.size();
				break;
			case bsoncxx::type::k_document:
				elemSize = ele.get_document().value.length();
				break;
			case bsoncxx::type::k_double:
				elemSize = sizeof(ele.get_double().value);
				break;
			case bsoncxx::type::k_int32:
				elemSize = sizeof(ele.get_int32().value);
				break;
			case bsoncxx::type::k_int64:
				elemSize = sizeof(ele.get_int64().value);
				break;
			case bsoncxx::type::k_maxkey:
				elemSize = sizeof(ele.get_maxkey());
				break;
			case bsoncxx::type::k_minkey:
				elemSize = sizeof(ele.get_minkey());
				break;
			case bsoncxx::type::k_null:
				elemSize = sizeof(ele.get_null());
				break;
			case bsoncxx::type::k_regex:
				elemSize = ele.get_regex().regex.length();
				break;
			case bsoncxx::type::k_timestamp:
				elemSize = sizeof(ele.get_timestamp().timestamp);
				break;
			case bsoncxx::type::k_undefined:
				elemSize = sizeof(ele.get_undefined());
				break;
			default:
				assert(0);
		}
		return elemSize;
	}
	//void DocumentAPI::GetDocumentFromFileStorage(value id, std::ifstream* destination)
	//{
	//	try
	//	{
	//		auto queryDoc = bsoncxx::builder::stream::document{} << "_id" << id << bsoncxx::builder::stream::finalize;
	//		DocumentAccessBase getDoc("Tutorial", "Documents");
	//		auto fileData = getDoc.GetDocument(bsoncxx::to_json(queryDoc), "{}");
	//		if (fileData)
	//		{
	//			std::string path = fileData.get().view()["Path"].get_utf8().value.to_string();
	//			destination->open(path);

	//			// For performance testing, just read the entire data into buffer.
	//			// No need to perform other operation on buffer
	//			const int BUFFER_SIZE = 500 * 1024 * 1024;
	//			std::vector<char> buffer(BUFFER_SIZE + 1, 0);
	//			while (*destination)
	//			{
	//				destination->read(buffer.data(), BUFFER_SIZE);
	//				std::streamsize s = ((*destination) ? BUFFER_SIZE : destination->gcount());

	//				buffer[s] = 0;
	//			}
	//		}
	//		else {
	//			std::cout << "Document couldn't be found";
	//		}
	//	}
	//	catch (std::exception& e)
	//	{
	//		std::cout << e.what();
	//	}
	//}

	//value DocumentAPI::InsertDocumentToFileStorage(std::ifstream* source, std::string fileName)
	//{
	//	try
	//	{
	//		auto path = "C:\\HIS\\4th sem\\data\\" + fileName;

	//		std::ofstream myfile(path);
	//		const int BUFFER_SIZE = 500 * 1024 * 1024;
	//		std::vector<char> buffer(BUFFER_SIZE + 1, 0);
	//		while (*source)
	//		{
	//			source->read(buffer.data(), BUFFER_SIZE);
	//			std::streamsize s = ((*source) ? BUFFER_SIZE : source->gcount());

	//			buffer[s] = 0;
	//			myfile << buffer.data();
	//		}

	//		auto builder = bsoncxx::builder::stream::document{};
	//		auto doc_value = builder
	//			<< "FileName" << fileName
	//			<< "Path" << path
	//			<< "InsertType" << InsertType::FileStorage
	//			<< bsoncxx::builder::stream::finalize;

	//		auto view = doc_value.view();
	//		DocumentAccessBase insertDoc("Tutorial", "Documents");
	//		auto res = insertDoc.InsertDocument(bsoncxx::to_json(view));
	//		return res.get().inserted_id();
	//	}
	//	catch (std::exception& e)
	//	{
	//		std::cout << e.what();
	//	}
	//}

	//std::string DocumentAPI::GetDocumentPathFromFileStorage(value id)
	//{
	//	try
	//	{
	//		auto queryDoc = bsoncxx::builder::stream::document{} << "_id" << id << bsoncxx::builder::stream::finalize;
	//		DocumentAccessBase getDoc("Tutorial", "Documents");
	//		auto fileData = getDoc.GetDocument(bsoncxx::to_json(queryDoc), "{}");

	//		if (fileData)
	//		{
	//			std::string path = fileData.get().view()["Path"].get_utf8().value.to_string();
	//			return path;
	//		}
	//		else {
	//			std::cout << "Document couldn't be found";
	//		}
	//	}
	//	catch (std::exception& e)
	//	{
	//		std::cout << e.what();
	//	}
	//}

	//value DocumentAPI::InsertDocumentToDatabase(std::string fileName, std::string content)
	//{
	//	try
	//	{
	//		rapidjson::Document queryDoc;
	//		queryDoc.SetObject();

	//		rapidjson::Value jsonContent(rapidjson::kObjectType);
	//		rapidjson::Value jsonFileName(rapidjson::kObjectType);

	//		auto& allocator = queryDoc.GetAllocator();
	//		jsonContent.SetString(content.c_str(), static_cast<rapidjson::SizeType>(content.length()), allocator);
	//		jsonFileName.SetString(fileName.c_str(), static_cast<rapidjson::SizeType>(fileName.length()), allocator);

	//		queryDoc.AddMember("FileName-Json", jsonFileName, allocator);
	//		queryDoc.AddMember("Content", jsonContent, allocator);
	//		queryDoc.AddMember("InsertType", InsertType::Database, allocator);

	//		rapidjson::StringBuffer buffer;
	//		buffer.Clear();
	//		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	//		queryDoc.Accept(writer);

	//		/*auto builder = bsoncxx::builder::stream::document{};
	//		auto doc_value = builder
	//			<< "FileName" << fileName
	//			<< "Content" << content
	//			<< "InsertType" << InsertType::DatabaseWithContent
	//			<< bsoncxx::builder::stream::finalize;

	//		auto view = doc_value.view();*/

	//		DocumentAccessBase insertDoc("Tutorial", "Documents");
	//		auto res = insertDoc.InsertDocument(buffer.GetString());
	//		return res.get().inserted_id();
	//	}
	//	catch (std::exception& e)
	//	{
	//		std::cout << e.what();
	//	}
	//}

	//std::string DocumentAPI::GetDocumentFromDatabase(value id)
	//{
	//	try
	//	{
	//		rapidjson::Document queryDoc;
	//		queryDoc.SetObject();
	//		auto& allocator = queryDoc.GetAllocator();
	//		rapidjson::Value jsonId(rapidjson::kObjectType);
	//		rapidjson::Value jsonOId(rapidjson::kObjectType);
	//		rapidjson::Value obj(rapidjson::kObjectType);

	//		auto oid = id.get_oid().value.to_string();
	//		jsonOId.SetString(oid.c_str(), static_cast<rapidjson::SizeType>(oid.length()), allocator);
	//		obj.AddMember("$oid", jsonOId, allocator);

	//		//jsonId.PushBack(obj, allocator);
	//		queryDoc.AddMember("_id", obj, allocator);
	//		rapidjson::StringBuffer buffer;
	//		buffer.Clear();

	//		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
	//		queryDoc.Accept(writer);
	//		//auto queryDoc = bsoncxx::builder::stream::document{} << "_id" << id << bsoncxx::builder::stream::finalize;
	//		DocumentAccessBase getDoc("Tutorial", "Documents");
	//		auto fileData = getDoc.GetDocument(buffer.GetString(), "{}");
	//		if (fileData)
	//		{
	//			std::string content = fileData.get().view()["Content"].get_utf8().value.to_string();
	//			return content;
	//		}
	//		else {
	//			return "Data couldn't be found";
	//		}
	//	}
	//	catch (std::exception& e)
	//	{
	//		std::cout << e.what();
	//	}
	//}

	//value DocumentAPI::InsertDocumentPathToDatabase(std::string filePath, std::string fileName)
	//{
	//	try
	//	{
	//		auto builder = bsoncxx::builder::stream::document{};
	//		auto doc_value = builder
	//			<< "FileName" << fileName
	//			<< "FilePath" << filePath
	//			<< "InsertType" << InsertType::Database
	//			<< bsoncxx::builder::stream::finalize;

	//		auto view = doc_value.view();

	//		DocumentAccessBase insertDoc("Tutorial", "Documents");
	//		auto res = insertDoc.InsertDocument(bsoncxx::to_json(view));
	//		return res.get().inserted_id();
	//	}
	//	catch (std::exception& e)
	//	{
	//		std::cout << e.what();
	//	}
	//}

	//std::string DocumentAPI::GetDocumentPathFromDatabase(value id)
	//{
	//	try
	//	{
	//		auto queryDoc = bsoncxx::builder::stream::document{} << "_id" << id << bsoncxx::builder::stream::finalize;
	//		DocumentAccessBase getDoc("Tutorial", "Documents");
	//		auto fileData = getDoc.GetDocument(bsoncxx::to_json(queryDoc), "{}");
	//		if (fileData)
	//		{
	//			std::string filePath = fileData.get().view()["FilePath"].get_utf8().value.to_string();
	//			return filePath;
	//		}
	//		else {
	//			return "Data couldn't be found";
	//		}
	//	}
	//	catch (std::exception& e)
	//	{
	//		std::cout << e.what();
	//	}
	//}
}
