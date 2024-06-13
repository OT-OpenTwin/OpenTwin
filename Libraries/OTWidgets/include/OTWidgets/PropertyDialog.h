//! @file PropertyDialog.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/PropertyGridCfg.h"
#include "OTGui/PropertyDialogCfg.h"
#include "OTWidgets/Dialog.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// std header
#include <map>

class QTreeWidgetItem;

namespace ot {

	class PropertyGrid;
	class PropertyGridItem;
	class PropertyGridGroup;
	class TreeWidgetItem;

	class OT_WIDGETS_API_EXPORT PropertyDialog : public Dialog {
		Q_OBJECT
		OT_DECL_NOCOPY(PropertyDialog)
	public:
		PropertyDialog(const PropertyDialogCfg& _config, QWidget* _parentWidget = (QWidget*)nullptr);
		virtual ~PropertyDialog();

		const PropertyGrid* const getPropertyGrid(void) const { return m_grid; };

	Q_SIGNALS:
		void propertyChanged(const Property* _property);
		void propertyDeleteRequested(const Property* _property);
		
		// ###########################################################################################################################################################################################################################################################################################################################

		// Private slots

	private Q_SLOTS:
		void slotConfirm(void);
		void slotCancel(void);
		void slotTreeSelectionChanged(void);
		void slotPropertyChanged(const Property* _property);
		void slotPropertyDeleteRequested(const Property* _property);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private helper

	private:
		class PropertyDialogEntry;
		class PropertyDialogNavigation;

		void iniData(const PropertyDialogCfg& _config);
		void iniGroup(QTreeWidgetItem* _parentTreeItem, const PropertyGroup* _group);
		bool childItemExists(QTreeWidgetItem* _item, const QString& _text);

		//! \brief Creates a configuration for the root group that contains the full group structure until the provided group.
		//! Other groups and properties are ignored.
		//! The resulted structure may look like this: "Root/Child/ActualGroup (the group provided)".
		PropertyGroup* createRootGroupConfig(const PropertyGroup* _group);

		//! \brief Returns the first item matching the text.
		//! Returns _item if it matches.
		//! Calls this function for all childs of _item.
		QTreeWidgetItem* findMatchingItem(QTreeWidgetItem* _item, const QString& _text);

		std::map<QTreeWidgetItem*, PropertyDialogEntry> m_treeMap;

		PropertyDialogNavigation* m_navigation;
		PropertyGrid* m_grid;
		bool m_changed;
	};

}
