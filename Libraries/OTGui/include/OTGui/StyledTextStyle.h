// @otlicense

//! @file StyledTextStyle.h
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTGui/StyledText.h"
#include "OTGui/OTGuiAPIExport.h"

namespace ot {

	class OT_GUI_API_EXPORT StyledTextStyle : public Serializable {
	public:
		enum class TextSize {
			Regular,
			Header1,
			Header2,
			Header3
		};

		static std::string toString(TextSize _size);
		static TextSize stringToTextSize(const std::string& _size);

		StyledTextStyle();
		StyledTextStyle(const ConstJsonObject& _jsonObject);
		StyledTextStyle(const StyledTextStyle&) = default;
		virtual ~StyledTextStyle() = default;
		StyledTextStyle& operator = (const StyledTextStyle&) = default;

		bool operator == (const StyledTextStyle& _other) const;
		bool operator != (const StyledTextStyle& _other) const;

		//! @brief Add the object contents to the provided JSON object.
		//! @param _object Json object reference to write the data to.
		//! @param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setColorReference(StyledText::ColorReference _colorReference) { m_colorReference = _colorReference; };
		StyledText::ColorReference getColorReference(void) const { return m_colorReference; };

		void setBold(bool _isBold) { m_isBold = _isBold; };
		bool getBold(void) const { return m_isBold; };

		void setItalic(bool _isItalic) { m_isItalic = _isItalic; };
		bool getItalic(void) const { return m_isItalic; };

		void setUnderline(bool _isUnderline) { m_isUnderline = _isUnderline; };
		bool getUnderline(void) const { return m_isUnderline; };

		void setLineTrough(bool _isLineTrough) { m_isLineTrough = _isLineTrough; };
		bool getLineTrough(void) const { return m_isLineTrough; };

		void setTextSize(TextSize _size) { m_size = _size; };
		TextSize getTextSize(void) const { return m_size; };

		//! @brief Returns true if any style changes are set.
		//! If any style is set the generated text requires styling otherwise the default text editor style will be used.
		bool hasStyleSet(void) const { return m_colorReference != StyledText::ColorReference::Default || m_isBold || m_isItalic || m_isUnderline || m_isLineTrough || m_size != TextSize::Regular; };

	private:
		StyledText::ColorReference m_colorReference;
		bool m_isBold;
		bool m_isItalic;
		bool m_isUnderline;
		bool m_isLineTrough;
		TextSize m_size;

	};

}