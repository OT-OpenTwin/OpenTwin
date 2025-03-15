//! @file BasicValidator.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/OTAssert.h"
#include "OTWidgets/BasicValidator.h"

std::list<std::pair<char, char>> ot::BasicValidator::rangesToList(const ValidRanges& _ranges) {
	std::list<std::pair<char, char>> result;
	
	if (_ranges & ValidRange::UpperAscii) { result.push_back(std::pair<char, char>('A', 'Z')); }
	if (_ranges & ValidRange::LowerAscii) { result.push_back(std::pair<char, char>('a', 'z')); }
	if (_ranges & ValidRange::Numbers) { result.push_back(std::pair<char, char>('0', '9')); }
	if (_ranges & ValidRange::Underscore) { result.push_back(std::pair<char, char>('_', '_')); }
	if (_ranges & ValidRange::Space) { result.push_back(std::pair<char, char>(' ', ' ')); }

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
	if (!this->isCharValid(_str.at(ix), m_validStartingRanges)) {
		return QValidator::State::Invalid;
	}

	// Longer than one character (Check middle characters)
	if (ix < lastIx) {
		ix++;

		// Check mid section
		for (; ix < lastIx; ix++) {
			if (!this->isCharValid(_str.at(ix), m_validOtherRanges)) {
				return QValidator::State::Invalid;
			}
		}
	}

	// Check last character
	if (this->isCharValid(_str.at(ix), m_validEndingRanges)) {
		return QValidator::State::Acceptable;
	}
	else {
		return QValidator::State::Invalid;
	}
}

void ot::BasicValidator::fixup(QString& _input) const {
	int ix = 0;
	int lastIx = _input.length() - 1;
	auto it = _input.begin();

	// Check starting character
	if (!this->isCharValid(_input.at(ix), m_validStartingRanges)) {
		auto tmp = it;
		it++;
		_input.erase(tmp);
		lastIx--;
	}

	// Longer than one character (Check middle characters)
	if (ix < lastIx) {
		ix++;

		// Check mid section
		for (; ix < lastIx; ix++) {
			if (!this->isCharValid(_input.at(ix), m_validOtherRanges)) {
				auto tmp = it;
				it++;
				_input.erase(tmp);
				lastIx--;
			}
		}
	}

	// Check last character
	if (!this->isCharValid(_input.at(ix), m_validEndingRanges)) {
		auto tmp = it;
		it++;
		_input.erase(tmp);
	}
}
