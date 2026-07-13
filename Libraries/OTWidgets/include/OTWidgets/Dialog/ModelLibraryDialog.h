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
#include "OTGui/Dialog/ModelLibraryDialogCfg.h"
#include "OTWidgets/Dialog/Dialog.h"

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
	class ComboButton;

	class OT_WIDGETS_API_EXPORT ModelLibraryDialog : public Dialog {
		Q_OBJECT
		OT_DECL_NOCOPY(ModelLibraryDialog)
		OT_DECL_NOMOVE(ModelLibraryDialog)
		OT_DECL_NODEFAULT(ModelLibraryDialog)
	public:
		explicit ModelLibraryDialog(ModelLibraryDialogCfg&& _config, QWidget* _parent);
		virtual ~ModelLibraryDialog();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		const std::string& getSelectedName() const { return m_selectedName; };
		const std::string& getSelectedOwner() const { return m_selectedOwner; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Slots

	private Q_SLOTS:
		void slotConfirm();
		void slotFilterChanged();
		void slotModelChanged();
		void slotSourceSelectionChanged();

	private:
		struct FilterInputEntry {
			std::string name;
			Label* label = nullptr;
			LineEdit* edit = nullptr;
		};

		void updateNameEdit();
		const LibraryModel* getSelectedModel() const;

		ModelLibraryDialogCfg m_config;

		ComboButton* m_sourceSelection;
		ComboBox* m_nameEdit;
		std::string m_selectedName;
		std::string m_selectedOwner;
		std::list<FilterInputEntry> m_filterEntries;
		std::list<LibraryModel> m_sourceFilteredModels;

		QGroupBox* m_infoGroup;
		QGridLayout* m_infoLayout;
		std::list<QWidget*> m_infoWidgets;
	};

}