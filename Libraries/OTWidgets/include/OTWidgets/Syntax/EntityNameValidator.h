// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/Syntax/CustomValidator.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT EntityNameValidator : public CustomValidator {
		OT_DECL_DEFCOPY(EntityNameValidator)
		OT_DECL_DEFMOVE(EntityNameValidator)
	public:
		EntityNameValidator();
		virtual ~EntityNameValidator() = default;

		virtual QValidator::State validate(QString& _input, int& _position) const override { return this->validateString(_input); };

		//! @brief Returns the validation state of the input string.
		//! @param _str String to check.
		QValidator::State validateString(const QString& _str) const;

		//! @brief Returns true if the input string is valid.
		bool isStringValid(const QString& _string) const { return this->validateString(_string) == QValidator::Acceptable; };

		virtual void fixup(QString& _input, int& _cursorPosition) const override;
	private:
		inline bool isCharInvalid(QChar _c) const;

		std::vector<QChar> m_charBlackList;
	};

}