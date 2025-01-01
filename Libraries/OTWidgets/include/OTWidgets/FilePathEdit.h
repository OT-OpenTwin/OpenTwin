//! @file FilePathEdit.h
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/QWidgetInterface.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qobject.h>

class QWidget;

namespace ot {

	class LineEdit;
	class PushButton;

	class OT_WIDGETS_API_EXPORT FilePathEdit : public QObject, public QWidgetInterface {
		Q_OBJECT
		OT_DECL_NODEFAULT(FilePathEdit)
		OT_DECL_NOCOPY(FilePathEdit)
	public:
		enum FileMode {
			OpenFileMode,
			SaveFileMode
		};

		FilePathEdit(FileMode _mode, QWidget* _parent = (QWidget*)nullptr);
		FilePathEdit(const QString& _filePath, FileMode _mode, QWidget* _parent = (QWidget*)nullptr);
		virtual ~FilePathEdit();

		virtual QWidget* getQWidget(void) override { return m_root; };
		virtual const QWidget* getQWidget(void) const override { return m_root; };

		void setFilePath(const QString& _file);
		const QString& getFilePath(void) const { return m_file; };

		void setBrowseTitle(const QString& _title) { m_browseTitle = _title; };
		const QString& getBrowseTitle(void) const { return m_browseTitle; };

		void setFileFilter(const QString& _filter) { m_fileFilter = _filter; };
		const QString& getFileFilter(void) const { return m_fileFilter; };

		LineEdit* getLineEdit(void) { return m_edit; };
		PushButton* getPushButton(void) { return m_button; };

	Q_SIGNALS:
		void fileChanged(void);

	public Q_SLOTS:
		void slotBrowse(void);

	private Q_SLOTS:
		void slotEditChanged(void);

	private:
		void ini(QWidget* _parent);

		QString m_file;

		FileMode m_mode;
		QWidget* m_root;
		LineEdit* m_edit;
		PushButton* m_button;

		QString m_browseTitle;
		QString m_fileFilter;
	};

}