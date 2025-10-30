// @otlicense
// File: ModelLibraryDialog.h
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
#include "OTGui/ModelLibraryDialogCfg.h"
#include "OTWidgets/Dialog.h"

// std header
#include <list>
#include <string>

#pragma warning (disable:4251)

class QWidget;
class QGroupBox;
class QGridLayout;

namespace ot {

	class Label;
	class LineEdit;
	class ComboBox;

	class OT_WIDGETS_API_EXPORT ModelLibraryDialog : public Dialog {
		Q_OBJECT
		OT_DECL_NODEFAULT(ModelLibraryDialog)
		OT_DECL_NOCOPY(ModelLibraryDialog)
		OT_DECL_NOMOVE(ModelLibraryDialog)
	public:
		ModelLibraryDialog(ModelLibraryDialogCfg&& _config);
		virtual ~ModelLibraryDialog();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		const std::string& getSelectedName() const { return m_selectedName; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Slots

	private Q_SLOTS:
		void slotConfirm();
		void slotFilterChanged();
		void slotModelChanged();

	private:
		struct FilterInputEntry {
			std::string name;
			Label* label = nullptr;
			LineEdit* edit = nullptr;
		};

		ModelLibraryDialogCfg m_config;

		ComboBox* m_nameEdit;
		std::string m_selectedName;
		std::list<FilterInputEntry> m_filterEntries;

		QGroupBox* m_infoGroup;
		QGridLayout* m_infoLayout;
		std::list<QWidget*> m_infoWidgets;
	};

}
