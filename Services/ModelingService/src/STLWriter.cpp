// @otlicense
// File: STLWriter.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include "STLWriter.h"
#include "OTCADEntities/EntityGeometry.h"
#include "OTCADEntities/GeometryOperations.h"
#include "Application.h"

#include "OTCommunication/Msg.h"
#include "OTModelEntities/DataBase.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/uiComponent.h"
#include "OTModelAPI/ModelServiceAPI.h"

#include <iostream>
#include <filesystem>

#include "base64.h"
#include "zlib.h"

void STLWriter::getExportFileContent(std::string& fileContent, unsigned long long& uncompressedDataLength)
{
	std::string data;
	createSTLFileContent(data);

	uncompressedDataLength = data.size();

	// Compress the file data content
	uLong compressedSize = compressBound((uLong)uncompressedDataLength);

	char* compressedData = new char[compressedSize];
	compress((Bytef*)compressedData, &compressedSize, (Bytef*)data.data(), (uLong)uncompressedDataLength);

	// Convert the binary to an encoded string
	int encoded_data_length = Base64encode_len(compressedSize);
	char* base64_string = new char[encoded_data_length];

	Base64encode(base64_string, compressedData, compressedSize); // "base64_string" is a then null terminated string that is an encoding of the binary data pointed to by "data"

	delete[] compressedData;
	compressedData = nullptr;

	fileContent = std::string(base64_string);

	delete[] base64_string;
	base64_string = nullptr;
}

void STLWriter::createSTLFileContent(std::string& data)
{
	data = "Hello World!";
}
