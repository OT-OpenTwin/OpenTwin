// @otlicense
// File: CreateProjectDialog.h
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
#include "OTGui/ProjectTemplateInformation.h"
#include "OTWidgets/Dialog.h"

// Qt header
#include <QtWidgets/qlistwidget.h>

// std header
#include <map>
#include <list>

namespace ot {

	class Label;
	class LineEdit;
	class PushButton;
	class CreateProjectDialog;

	class OT_WIDGETS_API_EXPORT CreateProjectDialogEntry : public QListWidgetItem {
		OT_DECL_NOCOPY(CreateProjectDialogEntry)
		OT_DECL_NOMOVE(CreateProjectDialogEntry)
		OT_DECL_NODEFAULT(CreateProjectDialogEntry)
	public:
		explicit CreateProjectDialogEntry(const ProjectTemplateInformation& _info, const CreateProjectDialog* _dialog);
		virtual ~CreateProjectDialogEntry() {};

		const ProjectTemplateInformation& getInfo(void) const { return m_info; };

	private:
		ProjectTemplateInformation m_info;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################
	
	// ###########################################################################################################################################################################################################################################################################################################################

	class OT_WIDGETS_API_EXPORT CreateProjectDialog : public Dialog {
		Q_OBJECT
		OT_DECL_NOCOPY(CreateProjectDialog)
		OT_DECL_NOMOVE(CreateProjectDialog)
		OT_DECL_NODEFAULT(CreateProjectDialog)
	public:
		explicit CreateProjectDialog(QWidget* _parentWidget);
		virtual ~CreateProjectDialog();

		void setProjectTemplates(const std::list<ProjectTemplateInformation>& _templates);
		void setCurrentProjectName(const QString& _name);

		void selectProjectType(const std::string& _projectType, const std::string& _templateName);

		std::string getProjectType(void) const;
		std::string getTemplateName(bool _emptyIfDefault) const;
		std::string getProjectName(void) const;

	protected:
		virtual void showEvent(QShowEvent* _event) override;

	Q_SIGNALS:
		void createProject(const QString& _projectName, const std::string& _projectType, const std::string& _projectTemplate);

	private Q_SLOTS:
		void slotFocusName(void);
		void slotShowInfo(void);
		void slotSearch(void);
		void slotCreate(void);
		void slotCheckCreateEnabledState(void);

	private:
		bool isDataValid(void) const;
		void clear(void);
		void addListEntry(const ProjectTemplateInformation& _info);

		QListWidget* m_list;
		Label* m_info;

		LineEdit* m_search;
		LineEdit* m_name;
		PushButton* m_createButton;
	};

}