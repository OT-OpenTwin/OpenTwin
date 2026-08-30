// @otlicense

// OpenTwin header
#include "OTCore/Debugging/CoreDebug.h"
#include "OTCore/Object/NamedOTObject.h"

ot::NamedOTObject::NamedOTObject(OTObject* _parentObject)
	: OTObject(_parentObject)
{}

ot::NamedOTObject::NamedOTObject(const std::string& _name, OTObject* _parentObject)
	: OTObject(_parentObject), m_name(_name)
{}

void ot::NamedOTObject::setOTObjectName(const std::string& _name)
{
	OT_CORE_OTOBJECT_DBG_PTR(this, "Setting object name to: " + _name);
}

ot::NamedOTObject* ot::NamedOTObject::findNamedChildObject(const std::string& _objectName, bool _recursive)
{
	return const_cast<NamedOTObject*>(
		static_cast<const NamedOTObject*>(this)->findNamedChildObject(_objectName, _recursive)
		);
}

const ot::NamedOTObject* ot::NamedOTObject::findNamedChildObject(const std::string& _objectName, bool _recursive) const
{
	const NamedOTObject* result = nullptr;
	for (OTObject* obj : this->getChildOTObjects())
	{
		const NamedOTObject* namedObj = dynamic_cast<const NamedOTObject*>(obj);
		if (namedObj == nullptr)
		{
			continue;
		}

		if (namedObj->getOTObjectName() == _objectName)
		{
			result = namedObj;
			break;
		}
		else if (_recursive)
		{
			result = namedObj->findNamedChildObject(_objectName, _recursive);
			if (result)
			{
				break;
			}
		}
	}

	return result;
}
