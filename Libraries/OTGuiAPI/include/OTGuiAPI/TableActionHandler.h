//! @file TableActionHandler.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTCommunication/ActionHandler.h"
#include "OTGui/TableCfg.h"
#include "OTGuiAPI/OTGuiAPIAPIExport.h"

namespace ot {

	class OT_GUIAPI_API_EXPORT TableActionHandler {
		OT_DECL_NOCOPY(TableActionHandler)
		OT_DECL_NOMOVE(TableActionHandler)
	public:
		TableActionHandler(ActionDispatcherBase* _dispatcher = &ot::ActionDispatcher::instance());
		virtual ~TableActionHandler() = default;

	protected:
		virtual void tableSaveRequested(const ot::TableCfg& _config) {};

	private:
		ActionHandler m_actionHandler;

		void handleTableSaveRequested(JsonDocument& _doc);
	};

}