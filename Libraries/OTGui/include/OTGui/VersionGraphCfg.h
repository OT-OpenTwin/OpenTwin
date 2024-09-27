//! @file VersionGraphCfg.h
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/VersionGraphVersionCfg.h"

namespace ot {

	class OT_GUI_API_EXPORT VersionGraphCfg : public Serializable {
	public:
		VersionGraphCfg();
		VersionGraphCfg(const VersionGraphCfg& _other);
		virtual ~VersionGraphCfg();

		VersionGraphCfg& operator = (const VersionGraphCfg& _other);

		//! \brief Add the object contents to the provided JSON object.
		//! \param _object Json object reference to write the data to.
		//! \param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! \brief Set the object contents from the provided JSON object.
		//! \param _object The JSON object containing the information.
		//! \throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		//! \see getActiveVersionName
		void setActiveVersionName(const std::string& _version) { m_activeVersionName = _version; };

		//! \brief Returns the active version name.
		//! The active version is the currently active model version.
		const std::string& getActiveVersionName(void) const { return m_activeVersionName; };

		//! \see getActiveBranchVersionName
		void setActiveBranchVersionName(const std::string& _version) { m_activeBranchVersionName = _version; };

		//! \brief Returns the active branch version name.
		//! The active branch version is the currently active branch.
		//! All parent versions of this version are in the active branch.
		//! The first child a version in an active branch belongs to the active branch (after the active branch version, parent versions are handled by the parent).
		const std::string& getActiveBranchVersionName(void) const { return m_activeBranchVersionName; };

		void addRootVersion(const VersionGraphVersionCfg& _version);
		void setRootVersions(const std::list<VersionGraphVersionCfg>& _versions) { m_rootVersions = _versions; };
		const std::list<VersionGraphVersionCfg>& getRootVersions(void) const { return m_rootVersions; };

	private:
		std::string m_activeVersionName;
		std::string m_activeBranchVersionName;
		std::list<VersionGraphVersionCfg> m_rootVersions;
	};

}