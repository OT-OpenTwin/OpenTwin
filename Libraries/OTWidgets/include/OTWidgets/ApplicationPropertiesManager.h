//! @file ApplicationPropertiesManager.h
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/PropertyGridCfg.h"
#include "OTWidgets/Dialog.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qobject.h>

// std header
#include <map>
#include <string>

namespace ot {

	class OT_WIDGETS_API_EXPORT ApplicationPropertiesManager : public QObject {
		Q_OBJECT
		OT_DECL_NOCOPY(ApplicationPropertiesManager)
	public:
		static ApplicationPropertiesManager& instance(void);

		ot::Dialog::DialogResult showDialog(void);

	Q_SIGNALS:
		void propertyChanged(const std::string& _groupPath, const std::string& _propertyName);
		void propertyDeleteRequested(const std::string& _groupPath, const std::string& _propertyName);

	public:
		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void add(const std::string& _owner, const PropertyGridCfg& _config);

		//! \brief Changes the merge behaviour.
		//! \see ot::PropertyGroup::mergeWith()
		void setReplaceExistingPropertiesOnMerge(bool _replace) { m_propertyReplaceOnMerge = _replace; };

		//! \brief The current property merge behaviour.
		//! \see ot::PropertyGroup::mergeWith()
		bool getReplaceExistingPropertiesOnMerge(void) const { return m_propertyReplaceOnMerge; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private

	private Q_SLOTS:
		void slotPropertyChanged(const std::string& _groupPath, const std::string& _propertyName);
		void slotPropertyDeleteRequested(const std::string& _groupPath, const std::string& _propertyName);

	private:
		bool m_propertyReplaceOnMerge;
		std::map<std::string, PropertyGridCfg> m_data;

		PropertyGridCfg& findOrCreateData(const std::string& _owner);

		ApplicationPropertiesManager();
		~ApplicationPropertiesManager();
	};

}