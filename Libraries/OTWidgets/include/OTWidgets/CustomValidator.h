// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtGui/qvalidator.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT CustomValidator : public QValidator {
		OT_DECL_NOCOPY(CustomValidator)
		OT_DECL_NOMOVE(CustomValidator)
	public:
		CustomValidator() = default;
		virtual ~CustomValidator() = default;

		//! @brief Fixes the given string.
		//! Will call the fixup function with cursor position ignored.
		//! @ref fixup(QString&, int&)
		//! @param _input Input string to fix.
		virtual void fixup(QString& _input) const override;

		//! @brief Fixes the given string.
		//! @param _input Input string to fix.
		//! @param _cursorPosition Current cursor position in the string. This position should be updated if characters are added/removed before this position.
		virtual void fixup(QString& _input, int& _cursorPosition) const = 0;
	};

}