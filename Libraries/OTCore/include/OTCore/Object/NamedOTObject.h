// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Object/OTObject.h"

namespace ot
{

	//! @brief The NamedOTObject class is a subclass of OTObject that adds a name property to the object.
	class OT_CORE_API_EXPORT NamedOTObject : public OTObject
	{
		OT_DECL_NOCOPY(NamedOTObject)
		OT_DECL_NOMOVE(NamedOTObject)
	public:
		explicit NamedOTObject(OTObject* _parentObject = (OTObject*)nullptr);
		explicit NamedOTObject(const std::string& _name, OTObject* _parentObject = (OTObject*)nullptr);
		virtual ~NamedOTObject() = default;

		void setOTObjectName(const std::string& _name);
		const std::string& getOTObjectName() const { return m_name; };

		NamedOTObject* findNamedChildObject(const std::string& _objectName, bool _recursive = true);
		const NamedOTObject* findNamedChildObject(const std::string& _objectName, bool _recursive = true) const;

	private:
		std::string m_name;

	};

}