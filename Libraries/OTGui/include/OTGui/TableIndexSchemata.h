// @otlicense

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