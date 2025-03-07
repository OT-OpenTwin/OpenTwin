//! @file BasicAsciiNameValidator.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/BasicAsciiNameValidator.h"

ot::BasicAsciiNameValidator::BasicAsciiNameValidator() {

}

ot::BasicAsciiNameValidator::State ot::BasicAsciiNameValidator::checkStringValid(const QString& _string) {
	QValidator::State result = QValidator::State::Intermediate;

	if (!_string.isEmpty()) {
		int ix = 0;

		QChar chr = _string.at(ix);
		if ((chr >= 'A' && chr <= 'Z') || (chr >= 'a' && chr <= 'z')) {
			result = QValidator::State::Acceptable;
			ix++;
		}

		for (; ix < _string.size(); ix++) {
			if (!(chr >= 'A' && chr <= 'Z') && !(chr >= 'a' && chr <= 'z') && !(chr >= '0' && chr <= '9')) {
				if (ix < _string.size() - 1) {
					if (chr != ' ' && chr != '_') {
						result = QValidator::State::Invalid;
						break;
					}
				}
				else {
					result = QValidator::State::Invalid;
					break;
				}

			}
		}
	}

	return result;
}

void ot::BasicAsciiNameValidator::fixup(QString& _input) const {
	for (auto it = _input.begin(); it != _input.end(); it++) {
		QChar chr = *it;
		if (!(chr >= 'A' && chr <= 'Z') && !(chr >= 'a' && chr <= 'z') &&
			!(chr >= '0' && chr <= '9') && chr != ' ' && chr != '_') {
			_input.erase(it);
		}
	}
}
