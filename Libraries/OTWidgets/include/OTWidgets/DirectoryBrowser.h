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
#include <QtCore/qobject.h>

class QWidget;
class QTreeWidgetItem;

namespace ot {

	class LineEdit;
	class TreeWidgetFilter;

	class OT_WIDGETS_API_EXPORT DirectoryBrowser : public QObject, public QWidgetInterface {
		Q_OBJECT
		OT_DECL_NOCOPY(DirectoryBrowser)
	public:
		DirectoryBrowser();
		virtual ~DirectoryBrowser();

		virtual QWidget* getQWidget(void) override { return m_rootWidget; };
		virtual const QWidget* getQWidget(void) const override { return m_rootWidget; };

		void goTo(const QString& _path);
		void goTo(const QStringList& _path);
		void expandCurrent(void);
		void goToAndExpand(const QStringList& _path);

	Q_SIGNALS:
		void currentPathChanged(const QString& _path);

	private Q_SLOTS:
		void slotSearchChanged(void);
		void slotSelectionChanged(void);

	private:
		QTreeWidgetItem* findItemFromPath(const QString& _tidyPath, bool _returnSubset);
		QTreeWidgetItem* findItemFromPath(const QStringList& _tidyPath, bool _returnSubset);
		QTreeWidgetItem* findItemFromPath(QTreeWidgetItem* _parentItem, QStringList& _tidyPath, bool _returnSubset);

		QTreeWidgetItem* buildPath(QTreeWidgetItem* _parentItem, QStringList& _tidyPath);
		void getItemPath(QTreeWidgetItem* _childItem, QStringList& _resultPath);
		QString getItemPathString(QTreeWidgetItem* _itm);

		QWidget* m_rootWidget;
		LineEdit* m_search;
		TreeWidgetFilter* m_tree;

	};

}