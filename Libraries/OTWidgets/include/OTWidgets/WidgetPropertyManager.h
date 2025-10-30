// @otlicense
// File: WidgetPropertyManager.h
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

#pragma once

// OpenTwin header
#include "OTGui/PropertyManager.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qlist.h>

// std header
#include <list>

namespace ot {

	class ManagedPropertyLink;
	class ManagedWidgetPropertyObject;
	
	//! @class WidgetPropertyManager
	//! @brief The WidgetPropertyManager class is an extended PropertyManager wrapper.
	//! It allows to use frontend component links to the properties enabling build in property handling.
	class OT_WIDGETS_API_EXPORT WidgetPropertyManager : public PropertyManager {
		OT_DECL_NOCOPY(WidgetPropertyManager)
	public:
		//! @brief Constructor.
		//! Creates a default PropertyManager.
		//! @param _object The object that owns this manager. May be null.
		WidgetPropertyManager(ManagedWidgetPropertyObject* _object = (ManagedWidgetPropertyObject*)nullptr);

		//! @brief Move constructor.
		//! @param _other Other object to move data from.
		WidgetPropertyManager(WidgetPropertyManager&& _other) noexcept;

		//! @brief Destructor.
		virtual ~WidgetPropertyManager() {};

		//! @brief Assignment move operator.
		//! @param _other Other object to move data from.
		//! @return Reference to this object.
		WidgetPropertyManager& operator = (WidgetPropertyManager&& _other) noexcept;
	
		// ###########################################################################################################################################################################################################################################################################################################################

		// Event handling

		//! @brief Will be called whenever a property has been changed.
		//! @param _property Property that has been changed.
		virtual void propertyChanged(const Property* _property) override;

		//! @brief Will be called before a property will be read.
		//! @param _propertyGroupName Name of the group where the property should be located at.
		//! @param _propertyName Name of the property. The name should be unique inside a group.
		virtual void readingProperty(const std::string& _propertyGroupName, const std::string& _propertyName) const override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Linking

		//! @brief Add property link to list.
		//! @param _link Link to add.
		void addPropertyLink(ManagedPropertyLink* _link);

		//! @brief Remove the property link from the lists.
		//! @param _link Link to remove.
		void forgetPropertyLink(ManagedPropertyLink* _link);

		//! @brief Return all currently registered propery links.
		const std::list<ManagedPropertyLink*>& getPropertyLinks(void) const { return m_links; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Getter

		//! @brief Set the managed property object that owns this manager.
		void setManagedObject(ManagedWidgetPropertyObject* _object) { m_object = _object; };

		//! @brief Returns the managed property object that owns this manager.
		ManagedWidgetPropertyObject* getManagedObject(void) const { return m_object; };

	private:
		ManagedWidgetPropertyObject* m_object;
		std::list<ManagedPropertyLink*> m_links; //! @brief Links
	};

}