// @otlicense
// File: LibraryElementRequest.cpp
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


#include "OTModelEntities/Lms/LibraryElementRequest.h"

void ot::LibraryElementRequest::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("RequestingEntityID", m_requestingEntityID, _allocator);
	_object.AddMember("CollectionName", ot::JsonString(m_collectionName, _allocator), _allocator);
	_object.AddMember("CallBackAction", ot::JsonString(m_callBackAction, _allocator), _allocator);
	_object.AddMember("EntityType", ot::JsonString(m_entityType, _allocator), _allocator);
	_object.AddMember("NewEntityFolder", ot::JsonString(m_newEntityFolder, _allocator), _allocator);
	_object.AddMember("PropertyName", ot::JsonString(m_propertyName, _allocator), _allocator);
	_object.AddMember("CallbackService", ot::JsonString(m_callBackService, _allocator), _allocator);
	_object.AddMember("Value", ot::JsonString(m_value, _allocator), _allocator);
}

void ot::LibraryElementRequest::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_requestingEntityID = ot::json::getUInt64(_object, "RequestingEntityID");
	m_collectionName = ot::json::getString(_object, "CollectionName");
	m_callBackAction = ot::json::getString(_object, "CallBackAction");
	m_entityType = ot::json::getString(_object, "EntityType");
	m_newEntityFolder = ot::json::getString(_object, "NewEntityFolder");
	m_propertyName = ot::json::getString(_object, "PropertyName");
	m_callBackService = ot::json::getString(_object, "CallbackService");
	m_value = ot::json::getString(_object, "Value");
}