//! @file SyntaxHighlighterRule.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTGui/Font.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <string>

namespace ot {

	class Painter2D;

	class OT_GUI_API_EXPORT SyntaxHighlighterRule : public Serializable {
	public:
		SyntaxHighlighterRule();

		//! \brief Constructor.
		//! The SyntaxHighlighterRule takes ownership of the painter.
		SyntaxHighlighterRule(const std::string& _regularExpression, Painter2D* _painter);
		
		SyntaxHighlighterRule(const SyntaxHighlighterRule& _other);
		
		virtual ~SyntaxHighlighterRule();

		SyntaxHighlighterRule& operator = (const SyntaxHighlighterRule& _other);

		//! \brief Add the object contents to the provided JSON object.
		//! \param _object Json object reference to write the data to.
		//! \param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! \brief Set the object contents from the provided JSON object.
		//! \param _object The JSON object containing the information.
		//! \throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		//! \brief Set the painter.
		//! The SyntaxHighlighterRule takes ownership of the painter.
		//! An existing painter will be destroyed before replacing.
		void setPainter(Painter2D* _painter);

		//! \brief Returns the current painter.
		//! The SyntaxHighlighterRule keeps ownership of the painter.
		Painter2D* getPainter(void) { return m_painter; };

		//! \brief Returns the current painter.
		//! The SyntaxHighlighterRule keeps ownership of the painter.
		const Painter2D* getPainter(void) const { return m_painter; };

		void setRegularExpression(const std::string& _regularExpression) { m_regex = _regularExpression; };
		const std::string& getRegularExpression(void) const { return m_regex; };

		void setFont(const Font& _f) { m_font = _f; };
		const Font& getFont(void) const { return m_font; };

	private:
		Painter2D* m_painter;
		std::string m_regex;
		Font m_font;
	};

}