// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/Event/ContextRequestData.h"

namespace ot {

	class NavigationContextRequestData : public ContextRequestData
	{
		OT_DECL_DEFCOPY(NavigationContextRequestData)
		OT_DECL_DEFMOVE(NavigationContextRequestData)
	public:
		NavigationContextRequestData() = default;
		~NavigationContextRequestData() = default;

		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		static constexpr const char* className() noexcept { return "NavigationContextRequestData"; };
		virtual std::string getClassName() const { return NavigationContextRequestData::className(); };

		void setNavigationItemPath(const std::string& _navigationItemPath) { m_navigationItemPath = _navigationItemPath; };
		void setNavigationItemPath(std::string&& _navigationItemPath) { m_navigationItemPath = std::move(_navigationItemPath); };
		const std::string& getNavigationItemPath() const { return m_navigationItemPath; };

	private:
		std::string m_navigationItemPath;
	};


}