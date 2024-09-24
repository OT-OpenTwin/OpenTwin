//! @file CreateProjectDialog.h
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/ProjectTemplateInformation.h"
#include "OTWidgets/Dialog.h"

// std header
#include <list>

class QListWidget;

namespace ot {

	class Label;
	class LineEdit;
	class PushButton;

	class OT_WIDGETS_API_EXPORT CreateProjectDialog : public Dialog {
		Q_OBJECT
		OT_DECL_NOCOPY(CreateProjectDialog)
	public:
		CreateProjectDialog(QWidget* _parentWidget = (QWidget*)nullptr);
		virtual ~CreateProjectDialog();

		void setProjectTemplates(const std::list<ProjectTemplateInformation>& _templates);
		void setCurrentProjectName(const QString& _name);

		QString getProjectType(void) const;
		QString getProjectName(void) const;

	protected:
		virtual void showEvent(QShowEvent* _event) override;

	Q_SIGNALS:
		void createProject(const QString& _projectName, const QString& _projectTemplate);

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