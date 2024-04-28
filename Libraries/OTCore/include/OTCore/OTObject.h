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

	//! @class OTObject
	//! @brief Default OpenTwin object with a uid and a parent child relationship
	//! If the item is destroyed it will destroy all child items where "deleteOtObjectLater" is false
	class OT_CORE_API_EXPORTONLY OTObject : public OTObjectBase {

		OT_DECL_NOCOPY(OTObject)

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

		void setDeleteOtObjectLater(bool _deleteLater = true) { m_deleteOtObjectLater = _deleteLater; };
		bool isDeleteOtObjectLater(void) const { return m_deleteOtObjectLater; };

		void setOtUid(const std::string& _uid) { m_otuid = _uid; };
		const std::string& otUid(void) const { return m_otuid; };

		void setChildOtObjects(const std::list<OTObject*>& _objects) { m_childOtObjects = _objects; };
		const std::list<OTObject*>& childOtObjects(void) const { return m_childOtObjects; };

		void setParentOtObject(OTObject* _object) { m_parentOtObject = _object; };
		OTObject* parentOtObject(void) const { return m_parentOtObject; };

	private:
		bool m_deleteOtObjectLater;
		std::string m_otuid;
		std::list<OTObject*> m_childOtObjects;
		OTObject* m_parentOtObject;
	};
}