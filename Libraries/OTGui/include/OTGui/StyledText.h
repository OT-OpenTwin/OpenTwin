//! @file StyledText.h
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	//! @brief The StyledText is used to manipulate the style when working with the StyledTextBuilder.
	class StyledText {
	public:
		//! \brief Controls color style references.
		enum ColorReference {
			Default, //! \brief Default text color.
			Highlight, //! \brief Current styled text highlight color.
			LightHighlight, //! \brief Current styled text light highlight color.
			Comment, //! \brief Current styled text comment color.
			Warning, //! \brief Current styled text warning color.
			Error //! \brief Current styled text error color.
		};

		//! \brief Controls text character formatting.
		enum TextControl {
			//! \brief Resets all text style settings (use default style from now on).
			//! Will also reset the current ColorReference back to Default.
			ClearStyle,
			Bold, //! \brief Enable bold text.
			NotBold, //! \brief Disable bold text.
			Italic, //! \brief Enable italic text.
			NotItalic, //! \brief Disable italic text.
			Underline, //! \brief Enable underline text.
			NotUnderline, //! \brief Disable underline text.
			LineTrough, //! \brief Enable line trough text.
			NotLineTrough, //! \brief Disable line trough text.

			//! \brief Big header sized text (disable Text and other header setting).
			//! \note Note that the header text will be displayed on a separate line. <br>
			//! If the text is "<text><header><text>" the output will be: <br>
			//! <text> <br>
			//! <header> <br>
			//! <text>
			Header1,

			//! \brief Medium header sized text (disable Text and other header setting).
			Header2,

			//! \brief Small header sized text (disable Text and other header setting).
			Header3,
			Text, //! \brief Regular text (disable Header).
		};

		//! @brief Creates a string representation of the provided ColorReference.
		static std::string toString(ColorReference _colorReference);

		//! @brief Creates a ColorReference from the provided string.
		static ColorReference stringToColorReference(const std::string& _colorReference);

	private:
		StyledText() = delete;
		StyledText(const StyledText&) = delete;
		StyledText& operator = (const StyledText&) = delete;
	};

}