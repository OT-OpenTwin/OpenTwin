//! @file DirectoryBrowser.h
//! @author Alexander Kuester (alexk95)
//! @date February 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/QWidgetInterface.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qwidget.h>

class QTreeView;
class QFileSystemModel;

namespace ot {

	class LineEdit;

	class OT_WIDGETS_API_EXPORT DirectoryBrowser : public QWidget, public QWidgetInterface {
		Q_OBJECT
		OT_DECL_NOCOPY(DirectoryBrowser)
	public:
		DirectoryBrowser();
		virtual ~DirectoryBrowser();

		virtual QWidget* getQWidget(void) override { return this; };

		virtual const QWidget* getQWidget(void) const override { return this; };

	Q_SIGNALS:
		void currentPathChanged(const QString& _path);

	private Q_SLOTS:
		void slotRootChanged(void);
		void slotSelectionChanged(void);

	private:
		QString getItemText(const QModelIndex& _ix);

		LineEdit* m_rootEdit;
		QTreeView* m_treeView;
		QFileSystemModel* m_model;
	};

}