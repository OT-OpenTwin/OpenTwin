//! @file PropertyGridGroup.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/PropertyGroup.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtGui/qbrush.h>
#include <QtWidgets/qtreewidget.h>

namespace ot {

	class PropertyGridItem;

	class OT_WIDGETS_API_EXPORT PropertyGridGroup : public QTreeWidgetItem {
		OT_DECL_NOCOPY(PropertyGridGroup)
	public:
		PropertyGridGroup();
		virtual ~PropertyGridGroup();

		void setupFromConfig(PropertyGroup* _group);

		//! @brief Finish the setup (should be called after the item is placed in a tree and after calling setup from config)
		void finishSetup(void);

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& getName(void) const { return m_name; };

		void setTitle(const QString& _title);
		QString getTitle(void) const;

		void addItem(PropertyGridItem* _item);

		void setGroupBrush(const QBrush& _brush) { m_groupBrush = _brush; };
		const QBrush& groupBrush(void) const { return m_groupBrush; };

	private:
		std::string m_name;
		QBrush m_groupBrush;
	};

}