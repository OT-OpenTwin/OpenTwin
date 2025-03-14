//! @file PathBrowseEdit.h
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/GuiTypes.h"
#include "OTWidgets/QWidgetInterface.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qobject.h>

class QWidget;

namespace ot {

	class LineEdit;
	class PushButton;

	class OT_WIDGETS_API_EXPORT PathBrowseEdit : public QObject, public QWidgetInterface {
		Q_OBJECT
		OT_DECL_NODEFAULT(PathBrowseEdit)
		OT_DECL_NOCOPY(PathBrowseEdit)
	public:
		PathBrowseEdit(PathBrowseMode _mode, QWidget* _parent = (QWidget*)nullptr);
		PathBrowseEdit(const QString& _path, PathBrowseMode _mode, QWidget* _parent = (QWidget*)nullptr);
		virtual ~PathBrowseEdit();

		virtual QWidget* getQWidget(void) override { return m_root; };
		virtual const QWidget* getQWidget(void) const override { return m_root; };

		void setPath(const QString& _path);
		const QString& getPath(void) const { return m_path; };

		void setBrowseMode(PathBrowseMode _mode);
		PathBrowseMode getBrowseMode(void) const { return m_mode; };

		void setBrowseTitle(const QString& _title) { m_browseTitle = _title; };
		const QString& getBrowseTitle(void) const { return m_browseTitle; };

		void setFileFilter(const QString& _filter) { m_fileFilter = _filter; };
		const QString& getFileFilter(void) const { return m_fileFilter; };

		LineEdit* getLineEdit(void) { return m_edit; };
		PushButton* getPushButton(void) { return m_button; };

	Q_SIGNALS:
		void pathChanged(void);

	public Q_SLOTS:
		void slotBrowse(void);

	private Q_SLOTS:
		void slotEditChanged(void);

	private:
		void ini(QWidget* _parent);

		QString m_path;

		PathBrowseMode m_mode;
		QWidget* m_root;
		LineEdit* m_edit;
		PushButton* m_button;

		QString m_browseTitle;
		QString m_fileFilter;
	};

}