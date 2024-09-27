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

	class OT_GUI_API_EXPORT VersionGraphVersionCfg : public Serializable {
	public:
		VersionGraphVersionCfg();
		VersionGraphVersionCfg(const std::string& _name, const std::string& _label = std::string(), const std::string& _description = std::string());
		VersionGraphVersionCfg(const VersionGraphVersionCfg& _other);
		virtual ~VersionGraphVersionCfg();

		VersionGraphVersionCfg& operator = (const VersionGraphVersionCfg& _other);

		//! \brief Add the object contents to the provided JSON object.
		//! \param _object Json object reference to write the data to.
		//! \param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! \brief Set the object contents from the provided JSON object.
		//! \param _object The JSON object containing the information.
		//! \throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		//! \see getName
		void setName(const std::string& _name) { m_name = _name; };

		//! \brief Returns the version name.
		//! The name is used to identify the version in a graph.
		//! The name will be displayed on the version.
		const std::string& getName(void) const { return m_name; };

		//! \see getLabel
		void setLabel(const std::string& _title) { m_label = _title; };

		//! \brief Returns the version label.
		//! The label will be displayed on the version if it is not empty.
		const std::string& getLabel(void) const { return m_label; };

		void setDescription(const std::string& _description) { m_description = _description; };
		const std::string& getDescription(void) const { return m_description; };

		VersionGraphVersionCfg* addChildVersion(const std::string& _name, const std::string& _label = std::string(), const std::string& _description = std::string());

		//! \brief Adds the provided version as a child.
		//! This version takes ownership of the provided child version.
		void addChildVersion(VersionGraphVersionCfg* _child);
		void setChildVersions(const std::list<VersionGraphVersionCfg*>& _versions);

		//! \brief Returns the child versions.
		//! This version keeps ownership of the versions.
		const std::list<VersionGraphVersionCfg*>& getChildVersions(void) const { return m_childVersions; };

		//! \brief Returns the version with the given name.
		//! Thie version may be this version.
		VersionGraphVersionCfg* findVersion(const std::string& _versionName);

		//! \brief Returns the last version in this branch (first child versions).
		VersionGraphVersionCfg* getLastBranchVersion(void);

		//! \see getDirectParentIsHidden
		void setDirectParentIsHidden(bool _isHidden) { m_directParentIsHidden = _isHidden; };

		//! \brief If the direct parent is hidden the connection line will be displayed as a dashed line.
		bool getDirectParentIsHidden(void) const { return m_directParentIsHidden; };

		//! \brief Copy data excluding child versions.
		void applyConfigOnly(const VersionGraphVersionCfg& _other);

	private:
		void clear(void);
		void clearChilds(void);

		std::string m_name;
		std::string m_label;
		std::string m_description;
		std::list<VersionGraphVersionCfg*> m_childVersions;
		bool m_directParentIsHidden;
	};

}