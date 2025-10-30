// @otlicense
// File: TableIndexSchemata.h
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

#include "OTGui/TableCfg.h"
#include "OTGui/TableRange.h"

namespace ot {

	class OT_GUI_API_EXPORT TableIndexSchemata {
		OT_DECL_NOCOPY(TableIndexSchemata)
		OT_DECL_NOMOVE(TableIndexSchemata)
		OT_DECL_NODEFAULT(TableIndexSchemata)
	public:
		static ot::TableRange userRangeToMatrixRange(const ot::TableRange& _range, ot::TableCfg::TableHeaderMode _headerOrientation);

		static ot::TableRange selectionRangeToMatrixRange(const ot::TableRange& _range, ot::TableCfg::TableHeaderMode _headerOrientation);

		static ot::TableRange selectionRangeToUserRange(const ot::TableRange& _range);

		static ot::TableRange userRangeToSelectionRange(const ot::TableRange& _range);

		static ot::TableRange matrixToUserRange(const ot::TableRange& _range, ot::TableCfg::TableHeaderMode _headerOrientation);
	};

}