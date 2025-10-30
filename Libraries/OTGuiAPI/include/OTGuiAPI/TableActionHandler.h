// @otlicense
// File: TableActionHandler.h
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

		// ###########################################################################################################################################################################################################################################################################################################################

		// Public: Action document helper

		static ot::JsonDocument createTableSaveRequestDocument(const ot::TableCfg& _config);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected: Callbacks

	protected:
		//! @brief Callback that is triggered when a table save is requested.
		//! @param _config Configuration of the table to be saved.
		//! @return ReturnMessage indicating success or failure of the save operation.
		//! A return status of type OK will result in the table modified flag being cleared.
		virtual ReturnMessage tableSaveRequested(const ot::TableCfg& _config) { return ot::ReturnMessage(ot::ReturnMessage::Failed, "Unhandled save request"); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Action handling

	private:
		ActionHandler m_actionHandler;

		ReturnMessage handleTableSaveRequested(JsonDocument& _doc);
	};

}