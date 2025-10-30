// @otlicense

#pragma once

// OpenTwin header
#include <OTCore/Serializable.h>

// std header
#include <list>
#include <string>

namespace ot {

	class OT_CORE_API_EXPORT ProjectInformation : public Serializable {
		OT_DECL_DEFCOPY(ProjectInformation)
		OT_DECL_DEFMOVE(ProjectInformation)
	public:
		ProjectInformation();
		ProjectInformation(const ConstJsonObject& _jsonObject);
		virtual ~ProjectInformation() = default;

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		void setVersion(int _version) { m_version = _version; };
		int getVersion() const { return m_version; };

		void setProjectName(const std::string& _name) { m_name = _name; };
		const std::string& getProjectName() const { return m_name; };

		void setProjectType(const std::string& _type) { m_type = _type; };
		const std::string& getProjectType() const { return m_type; };

		void setUserName(const std::string& _name) { m_user = _name; };
		const std::string& getUserName() const { return m_user; };

		void setCollectionName(const std::string& _collection) { m_collection = _collection; };
		const std::string& getCollectionName() const { return m_collection; };

		void setCreationTime(int64_t _msSinceEpoch) { m_creationTime = _msSinceEpoch; };
		int64_t getCreationTime() const { return m_creationTime; };

		void setLastAccessTime(int64_t _msSinceEpoch) { m_lastAccessTime = _msSinceEpoch; };
		int64_t getLastAccessTime() const { return m_lastAccessTime; };

		void addUserGroup(const std::string& _group) { m_userGroups.push_back(_group); };
		void setUserGroups(const std::list<std::string>& _groups) { m_userGroups = _groups; };
		const std::list<std::string>& getUserGroups() const { return m_userGroups; };

		void addTag(const std::string& _tag) { m_tags.push_back(_tag); };
		void setTags(const std::list<std::string>& _tags) { m_tags = _tags; };
		const std::list<std::string>& getTags() const { return m_tags; };

		void setProjectGroup(const std::string& _group) { m_projectGroup = _group; };
		const std::string& getProjectGroup() const { return m_projectGroup; };

	private:
		int m_version;
		std::string m_name;
		std::string m_type;
		std::string m_user;
		std::string m_collection;
		int64_t m_creationTime;
		int64_t m_lastAccessTime;
		std::list<std::string> m_userGroups;
		std::list<std::string> m_tags;
		std::string m_projectGroup;
	};
}