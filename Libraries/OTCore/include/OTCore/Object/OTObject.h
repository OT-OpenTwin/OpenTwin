// @otlicense
// File: OTObject.h
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
#include "OTCore/Object/ObjectWithDestroyNotifier.h"

namespace ot {

	class OT_CORE_API_EXPORT OTObject : public ObjectWithDestroyNotifier
	{
		OT_DECL_NOCOPY(OTObject)
		OT_DECL_NOMOVE(OTObject)
	public:
		explicit OTObject(OTObject* _parentObject = (OTObject*)nullptr);
		virtual ~OTObject();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Parent / Child management

		//! @brief Sets the parent object of this object.
		//! This method will register this object as a child of the new parent and unregister it from the old parent if exists.
		void setParentOTObject(OTObject* _newParent);
		OTObject* getParentOTObject() { return m_parentObject; };
		const OTObject* getParentOTObject() const { return m_parentObject; };

		const std::list<OTObject*>& getChildOTObjects() const { return m_childObjects; };

		//! @brief Sets whether this object should be destroyed when its parent is destroyed.
		//! Default is true.
		void setDestroyByParent(bool _destroyByParent) { m_destroyByParent = _destroyByParent; };
		bool getDestroyByParent() const { return m_destroyByParent; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Child management

	private:
		inline void registerChildObject(OTObject* _object);
		inline void forgetChildObject(OTObject* _object);

		OTObject* m_parentObject;
		bool m_destroyByParent;
		std::list<OTObject*> m_childObjects;
	};

}
