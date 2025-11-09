// @otlicense
// File: FMNewProjectDialog.h
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
#include "OTWidgets/Dialog.h"
#include "OTFMC/FMCTypes.h"
#include "OTFMC/FMNewProjectInfo.h"

// std header
#include <map>

namespace ot {

	class ComboBox;
	class PlainTextEdit;
	class PathBrowseEdit;

	class OT_FMC_API_EXPORT FMNewProjectDialog : public ot::Dialog {
		OT_DECL_NOCOPY(FMNewProjectDialog)
		OT_DECL_NOMOVE(FMNewProjectDialog)
		OT_DECL_NODEFAULT(FMNewProjectDialog)
	public:
		FMNewProjectDialog(QWidget* _parent);
		virtual ~FMNewProjectDialog();

		FMNewProjectInfo getNewProjectInfo() const;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Slots

	private Q_SLOTS:
		void slotConfirm();
		void slotShowHelp();
		void slotApplyIgnorePatternPreset();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Helper

	private:
		enum class DefaultIgnorePattern {
			Unknown,
			Programming
		};

		void applyProgrammingIgnorePattern();

		bool m_replaceIgnoreFile;

		PathBrowseEdit* m_directory;
		ComboBox* m_defaultIgnorePatterns;
		PlainTextEdit* m_ignoreText;
	};
}