//! @file StyledText.h
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

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
			Italic, //! \brief Enable italic text.
			NotBold, //! \brief Disable bold text.
			NotItalic //! \brief Disable italic text.
		};

		static std::string toString(ColorReference _colorReference);
		static ColorReference stringToColorReference(const std::string& _colorReference);

	private:
		StyledText() = delete;
		StyledText(const StyledText&) = delete;
		StyledText& operator = (const StyledText&) = delete;
	};

}