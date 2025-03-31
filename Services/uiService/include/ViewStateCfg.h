#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"

// std header
#include <string>

class ViewStateCfg {
	OT_DECL_NOCOPY(ViewStateCfg)
public:
	ViewStateCfg();
	ViewStateCfg(ViewStateCfg&& _other) noexcept;
	virtual ~ViewStateCfg();

	ViewStateCfg& operator = (ViewStateCfg&& _other) noexcept = delete;

	std::string toJson(void) const;
	static ViewStateCfg fromJson(const std::string& _json);

	void setViewConfig(const std::string& _cfgString) { m_viewConfig = _cfgString; };
	const std::string& getViewConfig(void) const { return m_viewConfig; };

private:
	std::string m_viewConfig;

};