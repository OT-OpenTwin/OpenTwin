//! @file BasicValidator.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtGui/qvalidator.h>

namespace ot {

	//! @file BasicValidator.h
	//! @brief Defines a validator for ASCII-based names.
	//! This class provides a validator that ensures names contain only ASCII letters,
	//! digits, spaces, or underscores. The first character must be a letter.
	class OT_WIDGETS_API_EXPORT BasicValidator : public QValidator {
		OT_DECL_NOCOPY(BasicValidator)
	public:
		//! @brief Valid ranges for the input string.
		enum ValidRange {
			NoFlags    = 0 << 0, //! @brief No flags.
			UpperAscii = 1 << 0, //! @brief Upper case ASCII letters (A-Z).
			LowerAscii = 1 << 1, //! @brief Lower case ASCII letters (a-z).
			Numbers    = 1 << 2, //! @brief Numbers (0-9).
			Underscore = 1 << 3, //! @brief Underscore character ('_').
			Space      = 1 << 4, //! @brief Space character (' ').

			//! @brief All ASCII letters.
			AsciiLetters = UpperAscii | LowerAscii,

			//! @brief All valid characters for a name excluding spaces.
			CompactNameRanges = UpperAscii | LowerAscii | Numbers | Underscore,

			//! @brief All valid characters for a name including spaces.
			NameRanges = CompactNameRanges | Space
		};
		typedef ot::Flags<ValidRange> ValidRanges;

		//! @brief Converts the given ValidRanges to a list of char ranges.
		//! @param _ranges Ranges to convert.
		static std::list<std::pair<char, char>> rangesToList(const ValidRanges& _ranges);

		//! @brief Constructor.
		//! @param _startingRanges Starting ranges used to validate the first character.
		//! @param _otherRanges Other ranges used to validate the middle characters.
		//! @param _endingRanges Ending ranges used to validate the last character.
		BasicValidator(const ValidRanges& _startingRanges = ValidRanges(ValidRange::AsciiLetters), const ValidRanges& _otherRanges = (ValidRange::NameRanges), const ValidRanges& _endingRanges = ValidRanges(ValidRange::CompactNameRanges));

		//! @brief Move constructor.
		BasicValidator(BasicValidator&& _other) noexcept;

		//! @brief Destructor.
		virtual ~BasicValidator() {};

		//! @brief Move assignment operator.
		BasicValidator& operator = (BasicValidator&& _other) noexcept;

		//! @brief Validates the given input string by calling BasicValidator::checkStringValid.
		//! Returns only true if the state is acceptable.
		//! @ref BasicValidator::checkStringValid
		static inline bool checkStringIsValid(const QString& _string, const ValidRanges& _startingRanges = ValidRanges(ValidRange::AsciiLetters), const ValidRanges& _otherRanges = (ValidRange::NameRanges), const ValidRanges& _endingRanges = ValidRanges(ValidRange::CompactNameRanges)) { return BasicValidator::checkStringValid(_string, _startingRanges, _otherRanges) == QValidator::Acceptable; };

		//! @brief Validates the given input string.
		//! The input is acceptable if it starts with a letter and contains only ASCII letters,
		//! digits, spaces, or underscores. The last character cannot be a space or underscore.
		static State checkStringValid(const QString& _string, const ValidRanges& _startingRanges = ValidRanges(ValidRange::AsciiLetters), const ValidRanges& _otherRanges = (ValidRange::NameRanges), const ValidRanges& _endingRanges = ValidRanges(ValidRange::CompactNameRanges));

		//! @brief Validates the given input string by calling BasicValidator::checkStringValid.
		//! @param _input The string to validate. The function does not modify the input.
		//! @param _position The cursor position in the input string. The function does not modify the position.
		//! @return The validation state: Acceptable, Intermediate, or Invalid.
		//! @ref BasicValidator::checkStringValid
		virtual State validate(QString& _input, int& _position) const override { return this->validateString(_input); };

		//! @brief Returns the validation state of the input string.
		//! @param _str String to check.
		State validateString(const QString& _str) const;

		//! @brief Returns true if the input string is valid.
		bool isStringValid(const QString& _string) const { return this->validateString(_string) == QValidator::Acceptable; };

		//! @brief Attempts to fix an invalid input string.
		//! @param _input The input string to be corrected.
		//! Removes any invalid characters from the input string.
		//! @ref BasicValidator::validate
		virtual void fixup(QString& _input) const override;

		void setValidStartingRanges(std::list<std::pair<char, char>>&& _ranges) { m_validStartingRanges = std::move(_ranges); };
		const std::list<std::pair<char, char>>& getValidStartingRanges(void) const { return m_validStartingRanges; };

		void setValidOtherRanges(std::list<std::pair<char, char>>&& _ranges) { m_validOtherRanges = std::move(_ranges); };
		const std::list<std::pair<char, char>>& getValidOtherRanges(void) const { return m_validOtherRanges; };

	private:
		std::list<std::pair<char, char>> m_validStartingRanges;
		std::list<std::pair<char, char>> m_validOtherRanges;
		std::list<std::pair<char, char>> m_validEndingRanges;
		
		inline bool isCharValid(const QChar& _chr, const std::list<std::pair<char, char>>& _ranges) const {
			bool ok = false;

			for (const std::pair<char, char>& range : _ranges) {
				if (_chr >= range.first && _chr <= range.second) {
					ok = true;
					break;
				}
			}

			return ok;
		}

	};

}

OT_ADD_FLAG_FUNCTIONS(ot::BasicValidator::ValidRange)