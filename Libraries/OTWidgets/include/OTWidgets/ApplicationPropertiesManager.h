//! @file ApplicationPropertiesManager.h
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/PropertyGridCfg.h"
#include "OTWidgets/Dialog.h"

// Qt header
#include <QtCore/qobject.h>

// std header
#include <map>
#include <string>

namespace ot {

	class PropertyDialog;

	class OT_WIDGETS_API_EXPORT ApplicationPropertiesManager : public QObject {
		Q_OBJECT
		OT_DECL_NOCOPY(ApplicationPropertiesManager)
		OT_DECL_NOMOVE(ApplicationPropertiesManager)
	public:
		static ApplicationPropertiesManager& instance(void);

		ot::Dialog::DialogResult showDialog(void);

		void clear(void);

		std::list<const Property*> getChangedProperties(void) const { return m_changedProperties; };

	Q_SIGNALS:
		void propertyChanged(const std::string& _owner, const Property* _property);
		void propertyDeleteRequested(const std::string& _owner, const Property* _property);

	public:
		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void add(const std::string& _owner, const PropertyGridCfg& _config);

		//! @brief Changes the merge behaviour.
		//! @see ot::PropertyGroup::mergeWith()
		void setReplaceExistingPropertiesOnMerge(bool _replace) { m_propertyReplaceOnMerge = _replace; };

		//! @brief The current property merge behaviour.
		//! @see ot::PropertyGroup::mergeWith()
		bool getReplaceExistingPropertiesOnMerge(void) const { return m_propertyReplaceOnMerge; };

		const PropertyDialog* getDialog(void) const { return m_dialog; };

		void setDialogTitle(const QString& _title);
		const QString getDialogTitle(void) const { return m_dialogTitle; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private

	private Q_SLOTS:
		void slotPropertyChanged(const Property* _property);
		void slotPropertyDeleteRequested(const Property* _property);

	private:
		bool m_propertyReplaceOnMerge;

		std::list<const Property*> m_changedProperties;
		std::map<std::string, PropertyGridCfg> m_data;
		PropertyDialog* m_dialog;
		QString m_dialogTitle;

		PropertyGridCfg findData(const std::string& _owner);

		//! @brief Removes the root group and sets the owner.
		//! The owner will be set to the root group name before the root is removed.
		Property* createCleanedSlotProperty(const Property* _property, std::string& _owner);
		
		void updateCurrentDialog(void);

		PropertyGridCfg buildDialogConfiguration(void);

		ApplicationPropertiesManager();
		~ApplicationPropertiesManager();
	};

}