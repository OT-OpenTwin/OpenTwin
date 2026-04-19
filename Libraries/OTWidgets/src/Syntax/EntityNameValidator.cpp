// @otlicense

// OpenTwin header
#include "OTWidgets/Syntax/EntityNameValidator.h"

ot::EntityNameValidator::EntityNameValidator() 
	: m_charBlackList({'/'})
{
	
}

QValidator::State ot::EntityNameValidator::validateString(const QString& _str) const {
	for (const QChar& c : _str) {
		if (isCharInvalid(c)) {
			return QValidator::State::Invalid;
		}
	}

	return QValidator::State::Acceptable;
}

void ot::EntityNameValidator::fixup(QString& _input, int& _cursorPosition) const {
	int ix = 0;
	int len = _input.size();
	
	for (int i = 0; i < len; ) {
		if (isCharInvalid(_input.at(i))) {
			_input.removeAt(i);

			if (i < _cursorPosition) {
				_cursorPosition--;
			}
		}
		else {
			i++;
		}
	}
}

inline bool ot::EntityNameValidator::isCharInvalid(QChar _c) const
{
	return false;
}
