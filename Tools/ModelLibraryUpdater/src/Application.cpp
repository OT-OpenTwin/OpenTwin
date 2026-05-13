// @otlicense
// File: Application.cpp
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

// Service header
#include "Application.h"

// OpenTwin header
#include "OTServiceFoundation/UserCredentials.h"
#include "OTDataStorage/Connection/ConnectionAPI.h"
#include "OTSystem/AppExitCodes.h"
#include "OTSystem/OperatingSystem.h"
#include "OTCore/Logging/Logger.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Dispatch/ActionDispatcher.h"
#include "OTCore/ReturnMessage.h"
#include "OTModelEntities/Lms/LibraryElement.h"
#include "OTSystem/FileSystem/DirectoryIterator.h"
#include "OTSystem/FileSystem/AdvancedDirectoryIterator.h"

// Thirdparty header
#include <bsoncxx/builder/stream/helpers.hpp>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/json.hpp>
#include <QtCore/qcryptographichash.h>


Application& Application::getInstance() {
	static Application instance;
	return instance;
}


std::list<ot::LibraryElement> Application::getLocalModels(const std::string& _modelFolderPath, const std::string& _collectionName)
{
	std::list<ot::LibraryElement> localModels;
    try {
        
        if (!std::filesystem::exists(_modelFolderPath)) {
			OT_LOG_E("Model folder path does not exist: " + _modelFolderPath);
			return {};
        }

        ot::IgnoreRules ignoreRules;
        ignoreRules.addRule("*");
        ignoreRules.addRule("!*.otmeta.json");

		ot::AdvancedDirectoryIterator directoryIterator(_modelFolderPath, ot::DirectoryIterator::Files,ignoreRules);

        // Iterate through all
        while (directoryIterator.hasNext())             
        {
			ot::FileInformation fileInfo = directoryIterator.next();
			const std::string filePath = fileInfo.getPath().string();

            try {
                std::ifstream file(filePath);
                if (!file) {
                    OT_LOG_E("Cannot open file: " + filePath);
                    continue;
                }
                
				std::stringstream buffer;
                buffer << file.rdbuf();
                std::string content = buffer.str();
                file.close();

				// Deserialize the content into a LibraryElement
                ot::LibraryElement element = ot::LibraryElement::fromJson(content);
				element.setCollectionName(_collectionName);
				// Fill library element with hash value calculated from content file
				fillLibraryElementWithHash(element, _modelFolderPath);

				// Add the element to the list of local models
                localModels.push_back(element);
				
            }
            catch (const std::exception& e) {
                OT_LOG_E("Error processing .otmeta.json file '" + filePath + "': " + std::string(e.what()));
                continue;
            }
        } 
    }
    catch (const std::exception& e) {
        OT_LOG_E("Error while reading local models: " + std::string(e.what()));
        return {};
	}

    return localModels;
}

void Application::fillLibraryElementWithHash(ot::LibraryElement& _element, const std::string& _modelFolderPath) {

    // Get content file to calculate hash and set it to LibraryElement
    std::string contentFileName = _element.getFileName();
    std::filesystem::path contentFilePath = std::filesystem::path(_modelFolderPath) / contentFileName;
    if (std::filesystem::exists(contentFilePath)) {
        try {
            std::ifstream contentFile(contentFilePath, std::ios::binary);
            if (!contentFile) {
                OT_LOG_E("Cannot open content file: " + contentFilePath.string());
            }
            else {
                std::stringstream contentBuffer;
                contentBuffer << contentFile.rdbuf();
                std::string fileContent = contentBuffer.str();
                contentFile.close();

                // Calculate hash from content file and set it to LibraryElement
                QCryptographicHash hashCalculator(QCryptographicHash::Algorithm::Md5);
                hashCalculator.addData(QByteArrayView(fileContent.data(), fileContent.size()));
                std::string hashValue = hashCalculator.result().toHex().toStdString();
                _element.setHash(hashValue);
            }
        }
        catch (const std::exception& e) {
            OT_LOG_E("Error reading content file '" + contentFilePath.string() + "': " + std::string(e.what()));
        }
    }
}

std::list<ot::LibraryElement> Application::addDataToLibraryElements(const std::list<ot::LibraryElement>& _elements, const std::string& _modelFolderPath) {
    std::list<ot::LibraryElement> updatedElements = _elements;

    for (ot::LibraryElement& element : updatedElements) {
        std::string contentFileName = element.getFileName();
        std::filesystem::path contentFilePath = std::filesystem::path(_modelFolderPath) / contentFileName;
        if (std::filesystem::exists(contentFilePath)) {
            try {
                std::ifstream contentFile(contentFilePath, std::ios::binary);
                if (!contentFile) {
                    OT_LOG_E("Cannot open content file: " + contentFilePath.string());
                }
                else {
                    std::stringstream contentBuffer;
                    contentBuffer << contentFile.rdbuf();
                    std::string fileContent = contentBuffer.str();
                    contentFile.close();
                    // Set the file content to the LibraryElement
                    element.setData(fileContent);
                }
            }
            catch (const std::exception& e) {
                OT_LOG_E("Error reading content file '" + contentFilePath.string() + "': " + std::string(e.what()));
            }
        }
        else {
            OT_LOG_E("Content file does not exist: " + contentFilePath.string());
        }
	}
	return updatedElements;
}

void Application::createJsonDocumentFromLibraryElement(std::list<ot::LibraryElement> _elements, ot::JsonDocument& _doc) {

    // Pack the list of LibraryElements
    ot::JsonArray elementsArray;
    for (const auto& element : _elements) {
        ot::JsonObject elementObj;
        element.addToJsonObject(elementObj, _doc.GetAllocator());
        elementsArray.PushBack(elementObj, _doc.GetAllocator());
    }
    _doc.AddMember(OT_ACTION_PARAM_Config, elementsArray, _doc.GetAllocator());

    return;
}

std::list<ot::LibraryElement> Application::createLibraryElementsFromJsonDocument(const std::string& _lmsResponse) {
    std::list<ot::LibraryElement> receivedModels;
    try {
		// Parse the LMS response and check if it is successful
        ot::ReturnMessage returnMessage = ot::ReturnMessage::fromJson(_lmsResponse);

        if (!returnMessage.isOk()) {
            OT_LOG_E("LMS returned error: " + returnMessage.getWhat());
            return {};
        }

        ot::JsonDocument responseDoc;
        if (responseDoc.fromJson(returnMessage.getWhat().c_str())) {

			// Get the list of library elements from the response JSON
            std::list<ot::ConstJsonObject> elementObjects = ot::json::getObjectList(responseDoc, OT_ACTION_PARAM_Config);

			// Deserialise each library element and add it to the list of received models
            for (const ot::ConstJsonObject& elementObj : elementObjects) {
                ot::LibraryElement element;
                element.setFromJsonObject(elementObj);
                receivedModels.push_back(element);
            }

            return receivedModels;
        }
        else {
            OT_LOG_E("Failed to parse LMS response JSON");
            return {};
        }
    }
    catch (const std::exception& e) {
        OT_LOG_E("Error deserializing LMS response: " + std::string(e.what()));
        return {};
    }
}

std::string Application::sendToLms(const ot::JsonDocument& _doc, std::string _lmsUrl) {
	std::string lmsResponse;

    const int maxCt = 30;
    int ct = 1;
    bool ok = false;

    do {
        lmsResponse.clear();
        if (!(ok = ot::msg::send("", _lmsUrl, ot::MessageType::EXECUTE, _doc.toJson(), lmsResponse, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit))) {
            OT_LOG_E("Request failed [Attempt " + std::to_string(ct) + " / " + std::to_string(maxCt) + "]");
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(500ms);

        }
    } while (!ok && ct++ <= maxCt);

    if (!ok) {
        OT_LOG_E("Request failed");
        return "";
    }
	return lmsResponse;
}

std::string Application::sendAsyncToLms(const ot::JsonDocument& _doc, std::string _lmsUrl) {
    ot::msg::sendAsync("", _lmsUrl, ot::MessageType::EXECUTE, _doc.toJson(), ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit);
    return"";
}


void Application::start(ot::StartArgumentParser _argumentParser) {
    

	std::string dataPath = _argumentParser.getDataPath().toStdString();
    std::string collectionName = _argumentParser.getCollectionName().toStdString();
    std::string _lmsUrl = _argumentParser.getLmsUrl().toStdString();
	std::string databasePsw = _argumentParser.getDatabasePsw().toStdString();

    // First iterate through all local models and create a list of LibraryElements
    std::list<ot::LibraryElement> localModels = getLocalModels(dataPath, collectionName);

    if (localModels.empty()) {
        return;
    }
    
	// Pack the list of LibraryElements into a json document to send it to LMS
	ot::JsonDocument doc;
    doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_LMS_UpdateOrCreateLirbaryElement, doc.GetAllocator()), doc.GetAllocator());
    if(!databasePsw.empty()) {
        doc.AddMember(OT_ACTION_PARAM_Value, ot::JsonString(databasePsw, doc.GetAllocator()), doc.GetAllocator());
    }
	doc.AddMember(OT_ACTION_PARAM_COLLECTION_NAME, ot::JsonString(collectionName, doc.GetAllocator()), doc.GetAllocator());
	
	createJsonDocumentFromLibraryElement(localModels, doc);
    localModels.clear();

    // Send to Lms
    std::string lmsResponse;
    lmsResponse = sendToLms(doc, _lmsUrl);

	// Deserialize the LMS response and create a list of LibraryElements which need to be updated or initiall added
	localModels = createLibraryElementsFromJsonDocument(lmsResponse);

    if(localModels.empty()) {
        return;
	}
    
	// Add the content data to the LibraryElements based on the file names and folder path
	localModels = addDataToLibraryElements(localModels, dataPath);

    // Pack the list of LibraryElements into a json document to send it to LMS
	ot::JsonDocument updateDoc;
	updateDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_LMS_AddNewLibraryElement, updateDoc.GetAllocator()), updateDoc.GetAllocator());
    if (!databasePsw.empty()) {
        updateDoc.AddMember(OT_ACTION_PARAM_Value, ot::JsonString(databasePsw, doc.GetAllocator()), doc.GetAllocator());
    }

	updateDoc.AddMember(OT_ACTION_PARAM_COLLECTION_NAME, ot::JsonString(collectionName, updateDoc.GetAllocator()), updateDoc.GetAllocator());
	createJsonDocumentFromLibraryElement(localModels, updateDoc);

    // Send to Lms
	lmsResponse = sendToLms(updateDoc, _lmsUrl);

    return;
}


Application::Application() {

}

Application::~Application() {

}
