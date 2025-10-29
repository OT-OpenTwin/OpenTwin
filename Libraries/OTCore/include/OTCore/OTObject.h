// @otlicense

//! @file OTObject.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"

// std header
#include <list>
#include <string>

namespace ot {

	class OT_CORE_API_EXPORT OTObject {
		OT_DECL_NOCOPY(OTObject)
		OT_DECL_NOMOVE(OTObject)
	public:
		OTObject(OTObject* _parentObject = (OTObject*)nullptr);
		virtual ~OTObject();

		void setParentOTObject(OTObject* _newParent);
		OTObject* getParentOTObject(void) { return m_parentObject; };
		const OTObject* getParentOTObject(void) const { return m_parentObject; };

		void registerDeleteNotifier(OTObject* _notifier);
		void removeDeleteNotifier(OTObject* _notifier);

		const std::list<OTObject*>& getChildOTObjects(void) const { return m_childObjects; };
		OTObject* findChildOTObject(const std::string& _objectName);
		const OTObject* findChildOTObject(const std::string& _objectName) const;

		void setOTObjectName(const std::string& _name) { m_name = _name; };
		const std::string& getOTObjectName(void) const { return m_name; };

	protected:
		virtual void objectWasDestroyed(OTObject* _object) {};

	private:
		void registerChildObject(OTObject* _object);
		void forgetChildObject(OTObject* _object);
		void forgetObject(OTObject* _object);

		std::string m_name;

		OTObject* m_parentObject;

		std::list<OTObject*> m_deleteNotifier;
		std::list<OTObject*> m_watchedObjects;
		std::list<OTObject*> m_childObjects;
	};

}