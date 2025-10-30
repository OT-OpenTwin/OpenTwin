// @otlicense

#pragma once

// OpenTwin header
#include "OTGui/Property.h"
#include "OTGui/PropertyBase.h"
#include "OTWidgets/TreeWidgetItem.h"
#include "OTWidgets/WidgetTypes.h"

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
		OT_DECL_NOMOVE(PropertyGridItem)
	public:
		PropertyGridItem();
		virtual ~PropertyGridItem();

		//! @brief Setup the item
		//! Note that the item must be placed in a tree before calling the setup
		virtual bool setupFromConfig(const Property* _config);

		//! @brief Finish the setup (should be called after the item is placed in a tree and after calling setup from config)
		void finishSetup();

		void setParentPropertyGroup(PropertyGridGroup* _group) { m_parentGroup = _group; };
		PropertyGridGroup* getParentPropertyGroup() const { return m_parentGroup; };

		//! @brief Returns the name of the parent group.
		//! A parent group must be set.
		std::string getGroupName() const;

		void setTitle(const QString& _title);
		QString getTitle() const;

		void setInput(PropertyInput* _input);
		PropertyInput* getInput() const { return m_input; };
		
		PropertyBase getPropertyData() const;
		std::string getPropertyType() const;

	Q_SIGNALS:
		void inputValueChanged(const ot::Property* _property);
		void deleteRequested(const ot::Property* _property);

	private Q_SLOTS:
		void slotValueChanged();
		void slotDeleteRequested();
		void slotGlobalStyleChanged();
		
	private:
		Property* createSignalProperty();

		ot::Property* m_signalProperty;
		PropertyGridGroup* m_parentGroup;

		QWidget* m_titleLayoutW;
		Label* m_deleteLabel;
		Label* m_titleLabel;

		PropertyInput* m_input;
	};

}

Q_DECLARE_METATYPE(ot::PropertyGridItem*)