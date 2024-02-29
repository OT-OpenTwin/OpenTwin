//! @file PropertyGridItemDouble.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTGui/Property.h"
#include "OTWidgets/QWidgetInterface.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qvariant.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT PropertyInput : public QObject, public QWidgetInterface {
		Q_OBJECT
		OT_DECL_NOCOPY(PropertyInput)
	public:
		PropertyInput() {};
		virtual ~PropertyInput() {};

		virtual void addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberName, ot::JsonAllocator& _allocator) = 0;
		virtual QVariant getCurrentValue(void) const = 0;
		virtual QWidget* getQWidget(void) override = 0;

	Q_SIGNALS:
		void inputValueChanged(void);

	public Q_SLOTS:
		void slotValueChanged(void);
		void slotValueChanged(bool);
		void slotValueChanged(int);
	};
}