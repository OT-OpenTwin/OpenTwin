//! @file VersionGraphVersionCfg.h
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <list>
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class VersionGraphCfg;

	class OT_GUI_API_EXPORT VersionGraphVersionCfg : public Serializable {
	public:
		VersionGraphVersionCfg();
		//! \brief Assignment constructor.
		//! \ref ot::VersionGraphVersionCfg::getName
		VersionGraphVersionCfg(const std::string& _name, const std::string& _label = std::string(), const std::string& _description = std::string());
		VersionGraphVersionCfg(const VersionGraphVersionCfg& _other);
		VersionGraphVersionCfg(VersionGraphVersionCfg&& _other) noexcept;
		virtual ~VersionGraphVersionCfg();

		VersionGraphVersionCfg& operator = (const VersionGraphVersionCfg& _other);
		VersionGraphVersionCfg& operator = (VersionGraphVersionCfg&& _other) noexcept;
		
		//! @brief Returns true if the version name is equal to the provided name.
		bool operator == (const std::string& _name) const;

		//! @brief Returns true if the version name is not equal to the provided name.
		bool operator != (const std::string& _name) const;

		void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		//! \see getName
		void setName(const std::string& _name) { m_name = _name; };

		//! \brief Returns the version name.
		//! The name is used to identify the version in a graph.
		//! The name will be displayed on the version. <br>
		//! The name must follow the following syntax: <br>
		//! - <Version> <br>
		//! - or <br>
		//! - <Branch>.<Version> <br>
		//! - or
		//! - <Branch>.<Version>.<Branch>.<Version> and so on.
		const std::string& getName(void) const { return m_name; };

		//! @brief Returns the name of this versions branch.
		//! \ref getName
		std::string getBranchName(void) const;

		//! @brief Returns the name of the branch node.
		//! If this version is "2.1.1" the branch node is version "2".
		std::string getBranchNodeName(void) const;

		//! \see getLabel
		void setLabel(const std::string& _title) { m_label = _title; };

		//! \brief Returns the version label.
		//! The label will be displayed on the version if it is not empty.
		const std::string& getLabel(void) const { return m_label; };

		void setDescription(const std::string& _description) { m_description = _description; };
		const std::string& getDescription(void) const { return m_description; };

		void setParentVersion(const std::string& _version) { m_parentVersion = _version; };
		const std::string& getParentVersion(void) const { return m_parentVersion; };

		//! \see getDirectParentIsHidden
		void setDirectParentIsHidden(bool _isHidden) { m_directParentIsHidden = _isHidden; };

		//! \brief If the direct parent is hidden the connection line will be displayed as a dashed line.
		bool getDirectParentIsHidden(void) const { return m_directParentIsHidden; };

	private:
		std::string m_name;
		std::string m_label;
		std::string m_description;
		std::string m_parentVersion;
		bool m_directParentIsHidden;
	};

}