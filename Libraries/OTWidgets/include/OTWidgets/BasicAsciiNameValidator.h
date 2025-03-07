//! @file BasicAsciiNameValidator.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtGui/qvalidator.h>

namespace ot {

	//! @file BasicAsciiNameValidator.h
	//! @brief Defines a validator for ASCII-based names.
	//! This class provides a validator that ensures names contain only ASCII letters,
	//! digits, spaces, or underscores. The first character must be a letter.
	class OT_WIDGETS_API_EXPORT BasicAsciiNameValidator : public QValidator {
	public:
		BasicAsciiNameValidator();

		//! @brief Validates the given input string by calling BasicAsciiNameValidator::checkStringValid.
		//! Returns only true if the state is acceptable.
		//! @ref BasicAsciiNameValidator::checkStringValid
		static inline bool isStringValid(const QString& _string) { return BasicAsciiNameValidator::checkStringValid(_string) == QValidator::Acceptable; };

		//! @brief Validates the given input string.
		//! The input is acceptable if it starts with a letter and contains only ASCII letters,
		//! digits, spaces, or underscores. The last character cannot be a space or underscore.
		static State checkStringValid(const QString& _string);

		//! @brief Validates the given input string by calling BasicAsciiNameValidator::checkStringValid.
		//! @param _input The string to validate. The function does not modify the input.
		//! @param _position The cursor position in the input string. The function does not modify the position.
		//! @return The validation state: Acceptable, Intermediate, or Invalid.
		//! @ref BasicAsciiNameValidator::checkStringValid
		virtual State validate(QString& _input, int& _position) const override { return BasicAsciiNameValidator::checkStringValid(_input); };

		//! @brief Attempts to fix an invalid input string.
		//! @param _input The input string to be corrected.
		//! Removes any invalid characters from the input string.
		//! @ref BasicAsciiNameValidator::validate
		virtual void fixup(QString& _input) const override;

	};

}