// @otlicense
// File: ProjectFilterData.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"

// std header
#include <list>
#include <string>

namespace ot {

	class OT_CORE_API_EXPORT ProjectFilterData : public Serializable {
		OT_DECL_DEFCOPY(ProjectFilterData)
		OT_DECL_DEFMOVE(ProjectFilterData)
	public:
		explicit ProjectFilterData() = default;
		explicit ProjectFilterData(const ConstJsonObject& _jsonObject);
		virtual ~ProjectFilterData() = default;

		void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		void addProjectGroup(const std::string& _projectGroup) { m_projectGroups.push_back(_projectGroup); };
		void addProjectGroup(std::string&& _projectGroup) { m_projectGroups.push_back(std::move(_projectGroup)); };
		void setProjectGroups(const std::list<std::string>& _projectGroups) { m_projectGroups = _projectGroups; };
		void setProjectGroups(std::list<std::string>&& _projectGroups) { m_projectGroups = std::move(_projectGroups); };
		const std::list<std::string>& getProjectGroups() const { return m_projectGroups; };

		void addProjectType(const std::string& _projectType) { m_projectTypes.push_back(_projectType); };
		void addProjectType(std::string&& _projectType) { m_projectTypes.push_back(std::move(_projectType)); };
		void setProjectTypes(const std::list<std::string>& _projectTypes) { m_projectTypes = _projectTypes; };
		void setProjectTypes(std::list<std::string>&& _projectTypes) { m_projectTypes = std::move(_projectTypes); };
		const std::list<std::string>& getProjectTypes() const { return m_projectTypes; };

		void addProjectName(const std::string& _projectName) { m_projectNames.push_back(_projectName); };
		void addProjectName(std::string&& _projectName) { m_projectNames.push_back(std::move(_projectName)); };
		void setProjectNames(const std::list<std::string>& _projectNames) { m_projectNames = _projectNames; };
		void setProjectNames(std::list<std::string>&& _projectNames) { m_projectNames = std::move(_projectNames); };
		const std::list<std::string>& getProjectNames() const { return m_projectNames; };

		void addTag(const std::string& _tag) { m_tags.push_back(_tag); };
		void addTag(std::string&& _tag) { m_tags.push_back(std::move(_tag)); };
		void setTags(const std::list<std::string>& _tags) { m_tags = _tags; };
		void setTags(std::list<std::string>&& _tags) { m_tags = std::move(_tags); };
		const std::list<std::string>& getTags() const { return m_tags; };

		void addOwner(const std::string& _owner) { m_owners.push_back(_owner); };
		void addOwner(std::string&& _owner) { m_owners.push_back(std::move(_owner)); };
		void setOwners(const std::list<std::string>& _owners) { m_owners = _owners; };
		void setOwners(std::list<std::string>&& _owners) { m_owners = std::move(_owners); };
		const std::list<std::string>& getOwners() const { return m_owners; };

		void addUserGroup(const std::string& _userGroup) { m_userGroups.push_back(_userGroup); };
		void addUserGroup(std::string&& _userGroup) { m_userGroups.push_back(std::move(_userGroup)); };
		void setUserGroups(const std::list<std::string>& _userGroups) { m_userGroups = _userGroups; };
		void setUserGroups(std::list<std::string>&& _userGroups) { m_userGroups = std::move(_userGroups); };
		const std::list<std::string>& getUserGroups() const { return m_userGroups; };

	private:
		std::list<std::string> m_projectGroups;
		std::list<std::string> m_projectTypes;
		std::list<std::string> m_projectNames;
		std::list<std::string> m_tags;
		std::list<std::string> m_owners;
		std::list<std::string> m_userGroups;
	};

}