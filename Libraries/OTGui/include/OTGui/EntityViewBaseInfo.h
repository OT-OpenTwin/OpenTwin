//! @file EntityViewBaseInfo.h
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTCore/Serializable.h"
#include "OTCore/OTClassHelper.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_GUI_API_EXPORT EntityViewBaseInfo : public Serializable {
	public:
		EntityViewBaseInfo();
		EntityViewBaseInfo(const std::string& _name);
		EntityViewBaseInfo(const std::string& _name, UID _entityId, UID _entityVersion);
		EntityViewBaseInfo(const std::string& _name, const std::string& _title, UID _entityId, UID _entityVersion);
		EntityViewBaseInfo(const EntityViewBaseInfo& _other);
		virtual ~EntityViewBaseInfo();

		EntityViewBaseInfo& operator = (const EntityViewBaseInfo& _other);

		//! \brief Add the object contents to the provided JSON object.
		//! \param _object Json object reference to write the data to.
		//! \param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! \brief Set the object contents from the provided JSON object.
		//! \param _object The JSON object containing the information.
		//! \throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& getName(void) const { return m_name; };

		void setTitle(const std::string& _title) { m_title = _title; };
		const std::string& getTitle(void) const { return (m_title.empty() ? m_name : m_title); };

		void setEntityId(UID _id) { m_entityId = _id; };
		UID getEntityId(void) const { return m_entityId; };

		void setEntityVersion(UID _version) { m_entityVersion = _version; };
		UID getEntityVersion(void) const { return m_entityVersion; };

	private:
		std::string m_name;
		std::string m_title;
		UID m_entityId;
		UID m_entityVersion;

	};

}