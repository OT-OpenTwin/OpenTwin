// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtCore/qabstractitemmodel.h>
#include <QtWidgets/qwidget.h>

class QTreeView;
class QFileSystemModel;

namespace ot {

	class LineEdit;

	class OT_WIDGETS_API_EXPORT DirectoryBrowser : public QWidget, public WidgetBase {
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
		QModelIndex m_rootIx;
	};

}