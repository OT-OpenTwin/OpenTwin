//! @file Plot1DCurveInfoCfg.h
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTCore/Serializable.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_GUI_API_EXPORT Plot1DCurveInfoCfg : public Serializable {
	public:
		Plot1DCurveInfoCfg();
		Plot1DCurveInfoCfg(UID _id, UID _version, const std::string& _name);
		Plot1DCurveInfoCfg(const Plot1DCurveInfoCfg& _other);
		virtual ~Plot1DCurveInfoCfg();

		Plot1DCurveInfoCfg& operator = (const Plot1DCurveInfoCfg& _other);

		//! \brief Add the object contents to the provided JSON object.
		//! \param _object Json object reference to write the data to.
		//! \param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! \brief Set the object contents from the provided JSON object.
		//! \param _object The JSON object containing the information.
		//! \throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setId(UID _id) { m_id = _id; };
		UID getId(void) const { return m_id; };

		void setVersion(UID _version) { m_version = _version; };
		UID getVersion(void) const { return m_version; };

		void setTreeId(UID _id) { m_treeId = _id; };
		UID getTreeId(void) const { return m_treeId; };

		void setDimmed(bool _isDimmed) { m_dimmed = _isDimmed; };
		bool getDimmed(void) const { return m_dimmed; };

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& getName(void) const { return m_name; };

	private:
		bool m_dimmed;
		UID m_id;
		UID m_version;
		UID m_treeId;
		std::string m_name;
	};

}