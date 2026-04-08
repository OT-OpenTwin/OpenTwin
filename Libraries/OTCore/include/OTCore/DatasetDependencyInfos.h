// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/DatasetDependencyInfo.h"

// std header
#include <vector>
#include <optional>

namespace ot {

	class OT_CORE_API_EXPORT DatasetDependencyInfos : public Serializable
	{
		OT_DECL_DEFCOPY(DatasetDependencyInfos)
		OT_DECL_DEFMOVE(DatasetDependencyInfos)
	public:
		DatasetDependencyInfos() = default;
		DatasetDependencyInfos(const ConstJsonObject& _jsonObject);
		virtual ~DatasetDependencyInfos() = default;

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		void addDependency(const DatasetDependencyInfo& _info) { m_infos.push_back(_info); };
		void addDependency(DatasetDependencyInfo&& _info) { m_infos.push_back(std::move(_info)); };
		const std::vector<DatasetDependencyInfo>& getDependencies() const { return m_infos; };

		DatasetDependencyInfos findMatchingDependencies(const DatasetDependencyInfos& _otherDependencies);
		//! @brief Find dependencies that do not match with the provided dependencies.
		//! A dependency is considered matching if it has the same label, unit and value as a dependency in the provided list.
		//! @param _otherDependencies List of dependencies to compare with.
		//! @param _onlyExisting If true, only dependencies that exist in the current list will be considered for non-matching. If false, all dependencies in the provided list will be considered, and those that do not have a match in the current list will also be included in the result.
		DatasetDependencyInfos findNonMatchingDependencies(const DatasetDependencyInfos& _otherDependencies, bool _onlyExisting = true);

		bool operator==(const DatasetDependencyInfos& _other) const;
		bool operator!=(const DatasetDependencyInfos& _other) const { return !(*this == _other); };

		//! @brief Return true if there are no dependencies in the list.
		bool hasDependencies() const { return !m_infos.empty(); };

		size_t getDependencyCount() const { return m_infos.size(); };

		DatasetDependencyInfo& getFirstDependency() { return m_infos.front(); };
		const DatasetDependencyInfo& getFirstDependency() const { return m_infos.front(); };

		std::optional<DatasetDependencyInfo> getDependency(const std::string& _label);
		const std::optional<DatasetDependencyInfo> getDependency(const std::string& _label) const;

	private:
		std::vector<DatasetDependencyInfo> m_infos;
		
	};

}