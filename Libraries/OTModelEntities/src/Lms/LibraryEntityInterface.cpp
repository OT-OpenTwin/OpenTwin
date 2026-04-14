// @otlicense
// File: LibraryEntityInterface.cpp
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

//	OpenTwin header
#include "OTModelEntities/Lms/LibraryEntityInterface.h"

ot::LibraryEntityInterface::LibraryEntityInterface() {
	
}

void ot::LibraryEntityInterface::setLibraryElementID(ot::UID _libraryElementID, EntityBase* _entity) {
	EntityPropertiesBase* property = _entity->getProperties().getProperty("LibraryElementID");
	if (property) {
		EntityPropertiesString* libraryElementIDProperty = dynamic_cast<EntityPropertiesString*>(property);
		if (!libraryElementIDProperty) {
			OT_LOG_E("LibraryElementID property is not of type string");
			return;
		}
		libraryElementIDProperty->setValue(std::to_string(_libraryElementID));
	}
	else {
		EntityPropertiesString* prop = EntityPropertiesString::createProperty("LibraryElement", "LibraryElementID", std::to_string(_libraryElementID), "", _entity->getProperties());
		prop->setVisible(false);
		_entity->setModified();
	}
}

std::string ot::LibraryEntityInterface::getLibraryElementID(EntityBase* _entity) {
	EntityPropertiesBase* property = _entity->getProperties().getProperty("LibraryElementID");
	if (property) {
		EntityPropertiesString* libraryElementIDProperty = dynamic_cast<EntityPropertiesString*>(property);
		if (!libraryElementIDProperty) {
			OT_LOG_E("LibraryElementID property is not of type string");
			return "";
		}
		return libraryElementIDProperty->getValue();
	}
	else {
		return std::to_string(ot::invalidUID);
	}
}