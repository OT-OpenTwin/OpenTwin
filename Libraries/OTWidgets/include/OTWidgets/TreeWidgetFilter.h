//! @file TreeWidget.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTWidgets/QWidgetInterface.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qstring.h>

class QWidget;
class QVBoxLayout;
class QTreeWidgetItem;

namespace ot {

	class TreeWidget;
	class LineEdit;

	class OT_WIDGETS_API_EXPORT TreeWidgetFilter : public QObject, public ot::QWidgetInterface {
		Q_OBJECT
	public:
		TreeWidgetFilter(ot::TreeWidget * _tree = (ot::TreeWidget *)nullptr, int _filterColumn = 0);
		virtual ~TreeWidgetFilter();

		virtual void applyFilter(const QString& _filterText);
		virtual void clearFilter(void);

		//! @brief Returns a pointer to the root widget of this object
		virtual QWidget* getQWidget(void) override { return m_layoutW; };
		ot::LineEdit* filterLineEdit(void) { return m_lineEdit; };
		ot::TreeWidget* treeWidget(void) { return m_tree; };

	signals:
		void filterChanged(const QString& _filterText);
		void filterCleared(void);

	public slots:
		void slotReturnPressed(void);
		void slotTextChanged(void);

	private:
		void dispatchFilter(const QString& _text);
		void setAllVisible(QTreeWidgetItem* _item);

		//! @return True if this item or any of its childs match the filter
		bool filterChilds(QTreeWidgetItem* _item, const QString& _filter);

		void expandAllParents(QTreeWidgetItem* _item);

		int             m_filterColumn;
		QString         m_lastFilter;
		QWidget*        m_layoutW;
		QVBoxLayout*    m_layout;

		ot::LineEdit*   m_lineEdit;
		ot::TreeWidget* m_tree;
	};
}
