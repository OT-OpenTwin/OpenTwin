//! @file PropertyGridEntry.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/Property.h"
#include "OTGui/PropertyBase.h"
#include "OTWidgets/TreeWidgetItem.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qobject.h>

// std header
#include <string>

class QWidget;

namespace ot {

	class Label;
	class Property;
	class PropertyInput;
	class PropertyGridGroup;

	class OT_WIDGETS_API_EXPORT PropertyGridItem : public QObject, public TreeWidgetItem {
		Q_OBJECT
		OT_DECL_NOCOPY(PropertyGridItem)
	public:
		PropertyGridItem();
		virtual ~PropertyGridItem();

		//! @brief Setup the item
		//! Note that the item must be placed in a tree before calling the setup
		virtual bool setupFromConfig(const Property* _config);

		//! @brief Finish the setup (should be called after the item is placed in a tree and after calling setup from config)
		void finishSetup(void);

		void setParentPropertyGroup(PropertyGridGroup* _group) { m_parentGroup = _group; };
		PropertyGridGroup* getParentPropertyGroup(void) const { return m_parentGroup; };

		//! \brief Returns the name of the parent group.
		//! A parent group must be set.
		std::string getGroupName(void) const;

		void setTitle(const QString& _title);
		QString getTitle(void) const;

		void setInput(PropertyInput* _input);
		PropertyInput* getInput(void) const { return m_input; };
		
		PropertyBase getPropertyData(void) const;
		std::string getPropertyType(void) const;

	Q_SIGNALS:
		void inputValueChanged(const ot::Property* const _property);
		void deleteRequested(const ot::Property* const _property);

	private Q_SLOTS:
		void slotValueChanged(void);
		void slotDeleteRequested(void);
		void slotGlobalStyleChanged(void);
		
	private:
		void clear(void);
		Property* createSignalProperty(void);

		PropertyGridGroup* m_parentGroup;

		QWidget* m_titleLayoutW;
		Label* m_deleteLabel;
		Label* m_titleLabel;

		PropertyInput* m_input;
	};

}

Q_DECLARE_METATYPE(ot::PropertyGridItem*)