// @otlicense

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

	class PushButton;
	class PropertyGrid;
	class PropertyGridItem;
	class PropertyGridGroup;
	class TreeWidgetItem;

	//! @brief The PropertyDialog is used to edit many property groups.
	//! When a property is changed a copy of the property is stored and the propertyChanged signal is emitted.
	//! All changed properties can be accessed after the dialog closed.
	//! The dialog result is "OK" only when the user pressed Confirm and at least one property has changed.
	//! All stored changed properties will be destroyed when the dialog is destroyed.
	//! @see PropertyDialogCfg
	class OT_WIDGETS_API_EXPORT PropertyDialog : public Dialog {
		Q_OBJECT
		OT_DECL_NOCOPY(PropertyDialog)
		OT_DECL_NOMOVE(PropertyDialog)
	public:
		PropertyDialog(const PropertyDialogCfg& _config, QWidget* _parentWidget = (QWidget*)nullptr);
		virtual ~PropertyDialog();

		//! @brief Sets up this dialog from the provided configuration.
		//! Existing data will be removed.
		void setupFromConfiguration(const PropertyDialogCfg& _config);

		const PropertyGrid* getPropertyGrid(void) const { return m_grid; };

		const std::list<const Property*>& getChangedProperties(void) const { return m_changedProperties; };

		void setConfirmButtonEnabled(bool _enabled);

	Q_SIGNALS:
		void propertyChanged(const Property* _property);
		void propertyDeleteRequested(const Property* _property);

		//! @brief Is emitted whenever the property grid was setup from the active configuration.
		//! The active configuration depends on the selected tree item.
		void propertyGridRefreshed(void);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private slots

	private Q_SLOTS:
		void slotConfirm(void);
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

		//! @brief Creates a configuration for the root group that contains the full group structure until the provided group.
		//! Other groups and properties are ignored.
		//! The resulted structure may look like this: "Root/Child/ActualGroup (the group provided)".
		PropertyGroup* createRootGroupConfig(const PropertyGroup* _group);

		QTreeWidgetItem* findTreeItem(QTreeWidgetItem* _parent, QStringList _path) const;

		std::map<QTreeWidgetItem*, PropertyDialogEntry> m_treeMap;

		std::list<const Property*> m_changedProperties;
		PropertyDialogNavigation* m_navigation;
		PushButton* m_confirmButton;
		PropertyGrid* m_grid;
		bool m_changed;
	};

}
