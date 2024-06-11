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


	Q_SIGNALS:
		void propertyChanged(const std::string& _groupPath, const std::string& _propertyName);
		void propertyDeleteRequested(const std::string& _groupPath, const std::string& _propertyName);
		
		// ###########################################################################################################################################################################################################################################################################################################################

		// Private slots

	private Q_SLOTS:
		void slotConfirm(void);
		void slotCancel(void);
		void slotTreeSelectionChanged(void);
		void slotPropertyChanged(const std::string& _groupName, const std::string& _propertyName);
		void slotPropertyChanged(const std::list<std::string>& _groupPath, const std::string& _propertyName);
		void slotPropertyDeleteRequested(const std::string& _groupName, const std::string& _propertyName);
		void slotPropertyDeleteRequested(const std::list<std::string>& _groupPath, const std::string& _propertyName);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private helper

	private:
		class PropertyDialogEntry;
		class PropertyDialogNavigation;

		void iniData(void);
		void iniGroup(QTreeWidgetItem* _parentTreeItem, const PropertyGroup* _group);
		bool childItemExists(QTreeWidgetItem* _item, const QString& _text);

		//! \brief Returns the first item matching the text.
		//! Returns _item if it matches.
		//! Calls this function for all childs of _item.
		QTreeWidgetItem* findMatchingItem(QTreeWidgetItem* _item, const QString& _text);

		PropertyDialogCfg m_config;
		std::map<QTreeWidgetItem*, PropertyDialogEntry> m_treeMap;

		PropertyDialogNavigation* m_navigation;
		PropertyGrid* m_grid;
		bool m_changed;
	};

}
