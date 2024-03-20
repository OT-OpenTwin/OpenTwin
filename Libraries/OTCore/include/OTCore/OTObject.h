//! @file OTObject.h
//!
//! This file contains the OpenTwin object interface.
//! This interface is mainly used by all OpenTwin classes that are managed in a UserInterface related way
//! 
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTObjectBase.h"
#include "OTCore/OTClassHelper.h"

// std header
#include <list>
#include <string>

namespace ot {

	//! @brief Default OpenTwin object with a uid and a parent child relationship
	//! If the item is destroyed it will destroy all child items where "deleteOtObjectLater" is false
	class OT_CORE_API_EXPORTONLY OTObject : public OTObjectBase {

		OT_DECL_NOCOPY(OTObject)

		OT_PROPERTY_REF(std::string, otuid, setOtUid, otUid)
		OT_PROPERTY_REF(std::list<OTObject*>, childOtObjects, setChildOtObjects, childOtObjects)
		OT_PROPERTY(OTObject*, parentOtObject, setParentOtObject, parentOtObject)
		OT_PROPERTY(bool, deleteOtObjectLater, setDeleteOtObjectLater, isDeleteOtObjectLater)

	public:
		OTObject() : m_parentOtObject(nullptr), m_deleteOtObjectLater(false) {};
		OTObject(OTObject* _parentObject) : m_parentOtObject(_parentObject), m_deleteOtObjectLater(false) {};
		OTObject(const std::string& _otuid, OTObject* _parentObject = (OTObject*)nullptr) : m_otuid(_otuid), m_parentOtObject(_parentObject), m_deleteOtObjectLater(false) {};
		virtual ~OTObject() {
			if (m_parentOtObject) 
			{
				m_parentOtObject->removeChildOtObject(this);
			}
			m_parentOtObject = nullptr;
			for (auto c : m_childOtObjects) {
				c->setParentOtObject(nullptr);
				if (!c->isDeleteOtObjectLater()) {
					delete c;
				}
			}
		}

		//! @brief Stores the child in the child objects list
		void storeChildOtObject(OTObject* _childObj) { m_childOtObjects.push_back(_childObj); };

		//! @brief Removes the child in the child objects list
		void removeChildOtObject(OTObject* _childObj) { m_childOtObjects.remove(_childObj); }

	};
}