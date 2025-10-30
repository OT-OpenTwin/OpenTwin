// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/VersionGraphVersionCfg.h"

// std header
#include <list>

namespace ot {

	class OT_GUI_API_EXPORT VersionGraphCfg : public Serializable {
	public:
		typedef std::list<VersionGraphVersionCfg> VersionsList;
		typedef std::list<VersionsList> BranchesList;

		VersionGraphCfg();
		VersionGraphCfg(const VersionGraphCfg& _other) = delete;
		VersionGraphCfg(VersionGraphCfg&& _other) noexcept;
		virtual ~VersionGraphCfg();

		VersionGraphCfg& operator = (const VersionGraphCfg& _other) = delete;
		VersionGraphCfg& operator = (VersionGraphCfg&& _other) noexcept;

		//! @brief Add the object contents to the provided JSON object.
		//! @param _object Json object reference to write the data to.
		//! @param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		//! @see getActiveVersionName
		void setActiveVersionName(const std::string& _version) { m_activeVersionName = _version; };

		//! @brief Returns the active version name.
		//! The active version is the currently active model version.
		const std::string& getActiveVersionName(void) const { return m_activeVersionName; };

		void incrementActiveVersion(void);

		static std::string incrementVersion(const std::string& _version);

		//! @see getActiveBranchName
		void setActiveBranchName(const std::string& _version) { m_activeBranchName = _version; };

		//! @brief Returns the active branch name.
		//! The active branch is the currently active branch.
		//! All parent versions of this version are in the active branch.
		const std::string& getActiveBranchName(void) const { return m_activeBranchName; };

		VersionGraphVersionCfg& insertVersion(const std::string& _version, const std::string& _parentVersion, const std::string& _label = std::string(), const std::string& _description = std::string());

		VersionGraphVersionCfg& insertVersion(VersionGraphVersionCfg&& _version);

		VersionsList& insertBranch(VersionsList&& _branch);

		static VersionsList& insertBranch(VersionsList&& _branch, std::list<VersionsList>& _branchesList);

		//! @brief Returns the version with the given name.
		VersionGraphVersionCfg* findVersion(const std::string& _version);

		//! @brief Returns the version with the given name.
		const VersionGraphVersionCfg* findVersion(const std::string& _version) const;

		//! @brief Returns the last version in the active branch.
		const VersionGraphVersionCfg* findLastVersion(void);

		//! @brief Returns the last version in the specified branch.
		const VersionGraphVersionCfg* findLastVersion(const std::string& _branchName);

		//! @brief Returns the previous version to the specified version.
		const VersionGraphVersionCfg* findPreviousVersion(const std::string& _version) const;

		//! @brief Returns the next version to the specified version by taking the current branch into account.
		const VersionGraphVersionCfg* findNextVersion(const std::string& _version) const;
		const VersionGraphVersionCfg* findNextVersion(const std::string& _version, const std::string& _activeBranch) const;

		//! @brief Returns a list containing all next versions.
		//! The resulting list will not contain the specified _version.
		//! @param _version The version to start searching from.
		//! @param _lastVersion The last version to add to the list. Versions after that will be ignored.
		std::list<const VersionGraphVersionCfg*> findNextVersions(const std::string& _version, const std::string& _lastVersion);
		std::list<const VersionGraphVersionCfg*> findNextVersions(const std::string& _version, const std::string& _activeBranch, const std::string& _lastVersion);

		//! @brief Find all child versions of the specified versions. 
		std::list<const VersionGraphVersionCfg*> findAllNextVersions(const std::string& _version);

		//! @brief Returns true if the version is the last one in its branch.
		//! Will return false if the version does not exist or is not the last of its branch.
		bool versionIsEndOfBranch(const std::string& _version) const;

		//! @brief Returns true if a version with the given name prefix exists in any of the versions (Expensive).
		bool versionStartingWithNameExists(const std::string& _prefix);

		//! @brief Removes the version and all of its childs if the version exists (Expensive).
		void removeVersion(const std::string& _version);	

		bool getBranchExists(const std::string& _branchName) const;

		const std::list<std::list<VersionGraphVersionCfg>>& getBranches(void) const { return m_branches; };

		//! @brief Returns the number of branches that have the specified version as a branch node.
		int getBranchesCountFromNode(const std::string& _version) const;

		//! @brief Returns a list of direct branches that have the specified version as a branch node.
		std::list<std::list<VersionGraphVersionCfg>> getBranchesFromNode(const std::string& _version) const;

		//! @breif Clear the version graph.
		void clear(void);

	private:
		//! @brief Finds the next version.
		//! This method is designed to improve performance when "iterating" trough the current branch.
		//! @param _version Version to find the next version of.
		//! @param _versionList The list the current version is located at.
		//! If the list is not set it will be searched by calling findVersionIterator.
		//! On success the list will be set to the corresponding list of the result or null if there is no next version.
		//! @param _currentVersionListIterator List iterator of the _versionList pointing to the current version.
		//! On success the iterator will be set to the corresponding list iterator of the result.
		const ot::VersionGraphVersionCfg* findNextVersion(const std::string& _version, const std::string& _activeBranch, const VersionsList*& _versionList, VersionsList::const_iterator& _currentVersionListIterator) const;
		
		void findAllNextVersions(std::list<const VersionGraphVersionCfg*>& _result, const VersionsList* _list, VersionsList::const_iterator _iterator) const;

		bool findVersionIterator(const std::string& _versionName, const VersionsList*& _list, VersionsList::const_iterator& _iterator) const;

		typedef VersionsList* (FindBranchFun(const std::string& _branchName));
		VersionsList* findBranch(const std::string& _branchName);

		typedef const VersionsList* (ConstFindBranchFun(const std::string& _branchName));
		const VersionsList* findBranch(const std::string& _branchName) const;

		std::string m_activeVersionName;
		std::string m_activeBranchName;
		BranchesList m_branches;
	};

}