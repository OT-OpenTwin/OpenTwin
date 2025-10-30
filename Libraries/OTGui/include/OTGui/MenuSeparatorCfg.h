// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/MenuEntryCfg.h"

namespace ot {

	class OT_GUI_API_EXPORT MenuSeparatorCfg : public MenuEntryCfg {
	public:
		MenuSeparatorCfg();
		MenuSeparatorCfg(const MenuSeparatorCfg& _other);
		virtual ~MenuSeparatorCfg();

		MenuSeparatorCfg& operator = (const MenuSeparatorCfg&) = delete;

		virtual MenuEntryCfg* createCopy(void) const override;
		virtual EntryType getMenuEntryType(void) const override { return MenuEntryCfg::Separator; };
	};

}