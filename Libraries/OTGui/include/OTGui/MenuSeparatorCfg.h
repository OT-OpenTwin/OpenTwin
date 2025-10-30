// @otlicense
// File: MenuSeparatorCfg.h
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