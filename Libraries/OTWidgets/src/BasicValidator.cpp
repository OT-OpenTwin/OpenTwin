//! @file BasicValidator.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTSystem/OTAssert.h"
#include "OTWidgets/BasicValidator.h"

namespace ot {
	namespace intern {
		inline bool isCharValid(const QChar& _chr, const std::list<std::pair<char, char>>& _ranges) {
			bool ok = false;

			for (const std::pair<char, char>& range : _ranges) {
				if (_chr >= range.first && _chr <= range.second) {
					ok = true;
					break;
				}
			}

			return ok;
		}
	}
}

std::list<std::pair<char, char>> ot::BasicValidator::rangesToList(const ValidRanges& _ranges) {
	std::list<std::pair<char, char>> result;
	
	if (_ranges & ValidRange::UpperAscii) { result.push_back(std::pair<char, char>('A', 'Z')); }
	if (_ranges & ValidRange::LowerAscii) { result.push_back(std::pair<char, char>('a', 'z')); }
	if (_ranges & ValidRange::Numbers) { result.push_back(std::pair<char, char>('0', '9')); }
	if (_ranges & ValidRange::Underscore) { result.push_back(std::pair<char, char>('_', '_')); }
	if (_ranges & ValidRange::Space) { result.push_back(std::pair<char, char>(' ', ' ')); }
	if (_ranges & ValidRange::Tabulator) { result.push_back(std::pair<char, char>('\t', '\t')); }
	if (_ranges & ValidRange::NewLine) { result.push_back(std::pair<char, char>('\n', '\n')); }
	if (_ranges & ValidRange::NewLine) { result.push_back(std::pair<char, char>('\r', '\r')); }

	return result;
}

ot::BasicValidator::BasicValidator(const ValidRanges& _startingRanges, const ValidRanges& _otherRanges, const ValidRanges& _endingRanges) :
	m_validStartingRanges(rangesToList(_startingRanges)), m_validOtherRanges(rangesToList(_otherRanges)), m_validEndingRanges(rangesToList(_endingRanges))
{
	OTAssert(!m_validStartingRanges.empty(), "No starting ranges provided");
	OTAssert(!m_validOtherRanges.empty(), "No other ranges provided");
	OTAssert(!m_validEndingRanges.empty(), "No ending ranges provided");
}

ot::BasicValidator::BasicValidator(BasicValidator&& _other) noexcept :
	m_validStartingRanges(std::move(_other.m_validStartingRanges)),
	m_validOtherRanges(std::move(_other.m_validOtherRanges)),
	m_validEndingRanges(std::move(_other.m_validEndingRanges)) 
{

}

ot::BasicValidator& ot::BasicValidator::operator=(BasicValidator&& _other) noexcept {
	if (this != &_other) {
		m_validStartingRanges = std::move(_other.m_validStartingRanges);
		m_validOtherRanges = std::move(_other.m_validOtherRanges);
		m_validEndingRanges = std::move(_other.m_validEndingRanges);
	}

	return *this;
}

ot::BasicValidator::State ot::BasicValidator::checkStringValid(const QString& _string, const ValidRanges& _startingRanges, const ValidRanges& _otherRanges, const ValidRanges& _endingRanges) {
	BasicValidator tmp(_startingRanges, _otherRanges, _endingRanges);
	return tmp.validateString(_string);
}

ot::BasicValidator::State ot::BasicValidator::validateString(const QString& _str) const {
	if (_str.isEmpty()) {
		return QValidator::State::Intermediate;
	}

	int ix = 0;
	int lastIx = _str.length() - 1;

	// Check starting character
	if (!intern::isCharValid(_str.at(ix), m_validStartingRanges)) {
		return QValidator::State::Invalid;
	}

	// Longer than one character (Check middle characters)
	if (ix < lastIx) {
		ix++;

		// Check mid section
		for (; ix < lastIx; ix++) {
			if (!intern::isCharValid(_str.at(ix), m_validOtherRanges)) {
				return QValidator::State::Invalid;
			}
		}
	}

	// Check last character
	if (intern::isCharValid(_str.at(ix), m_validEndingRanges)) {
		return QValidator::State::Acceptable;
	}
	else {
		return QValidator::State::Invalid;
	}
}

void ot::BasicValidator::fixup(QString& _input, int& _cursorPosition) const {
	int ix = 0;
	int lastIx = _input.length() - 1;
	auto it = _input.begin();

	// Check starting character
	if (!intern::isCharValid(*it, m_validStartingRanges)) {
		it = _input.erase(it);
		lastIx--;
	}
	else {
		ix++;
		it++;
	}

	while (ix < lastIx) {
		if (!intern::isCharValid(*it, m_validOtherRanges)) {
			it = _input.erase(it);
			lastIx--;
			if (ix < _cursorPosition) {
				_cursorPosition--;
			}
		}
		else {
			ix++;
			it++;
		}
	}

	// Check last character
	if (!intern::isCharValid(*it, m_validEndingRanges)) {
		it = _input.erase(it);
		lastIx--;
		if (ix < _cursorPosition && ix > 0) {
			_cursorPosition--;
		}
	}
}
