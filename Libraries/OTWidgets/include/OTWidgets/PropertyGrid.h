//! @file PropertyGrid.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTWidgets/QWidgetInterface.h"
#include "OTWidgets/TreeWidget.h"

// Qt header
#include <QtWidgets/qwidget.h>

// std header
#include <list>

namespace ot {

	class PropertyGridGroup;
	class PropertyGridItem;

	class OT_WIDGETS_API_EXPORT PropertyGrid : public QObject, public QWidgetInterface {
		Q_OBJECT
		OT_DECL_NOCOPY(PropertyGrid)
	public:
		PropertyGrid(QObject* _parentObject = (QObject*)nullptr);
		virtual ~PropertyGrid();

		virtual QWidget* getQWidget(void) override { return m_tree; };

		void addGroup(PropertyGridGroup* _group);

	private slots:
		void slotPropertyChanged();

	private:
		ot::TreeWidget* m_tree;

		PropertyGridGroup* m_defaultGroup;
		std::list<PropertyGridGroup*> m_groups;
	};

}
