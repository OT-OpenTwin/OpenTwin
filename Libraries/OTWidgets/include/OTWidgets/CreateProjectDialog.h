// @otlicense

//! @file CreateProjectDialog.h
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

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
		OT_DECL_NODEFAULT(CreateProjectDialogEntry)
		OT_DECL_NOCOPY(CreateProjectDialogEntry)
	public:
		CreateProjectDialogEntry(const ProjectTemplateInformation& _info, const CreateProjectDialog* _dialog);
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
	public:
		CreateProjectDialog(QWidget* _parentWidget = (QWidget*)nullptr);
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