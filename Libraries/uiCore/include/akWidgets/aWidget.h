// @otlicense

#pragma once

// AK header
#include <akCore/globalDataTypes.h>
#include <akCore/akCore.h>
#include <akCore/aObject.h>

// Forward declaration
class QWidget;

namespace ak {

	//! This class provides a interface that represents a widget.
	//! Every class derived from this class must be able to provide a QWidget that is representing it
	class UICORE_API_EXPORT aWidget : public aObject {
	public:
		//! @brief Constructor
		//! @param _type Object type
		//! @param _UID The initial UID of this object
		aWidget(
			objectType			_type = otNone,
			UID					_UID = invalidUID
		);

		//! @brief Deconstructor
		virtual ~aWidget();

		//! @brief Will return the widgets widget to display it
		virtual QWidget * widget(void) = 0;

		//! @brief Returns true the object is derived from aWidget
		virtual bool isWidgetType(void) const override;

	private:
		// Block copy constructor
		aWidget(const aWidget & _other) = delete;

		// Block assignment operator
		aWidget & operator = (const aWidget & _other) = delete;
	};
} // namespace ak