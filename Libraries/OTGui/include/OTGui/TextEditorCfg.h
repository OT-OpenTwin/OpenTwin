//! @file TextEditorCfg.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTGui/GuiTypes.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_GUI_API_EXPORT TextEditorCfg : public Serializable {
	public:
		TextEditorCfg();
		TextEditorCfg(const TextEditorCfg& _other);
		virtual ~TextEditorCfg();

		TextEditorCfg& operator = (const TextEditorCfg& _other);

		//! \brief Add the object contents to the provided JSON object.
		//! \param _object Json object reference to write the data to.
		//! \param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! \brief Set the object contents from the provided JSON object.
		//! \param _object The JSON object containing the information.
		//! \throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& getName(void) const { return m_name; };

		void setTitle(const std::string& _title) { m_title = _title; };
		const std::string& getTitle(void) const { return (m_title.empty() ? m_name : m_title); };

		void setPlainText(const std::string& _text) { m_text = _text; };
		const std::string& getPlainText(void) const { return m_text; };

		void setDocumentSyntax(DocumentSyntax _syntax) { m_syntax = _syntax; };
		DocumentSyntax getDocumentSyntax(void) const { return m_syntax; };

	private:
		std::string m_name;
		std::string m_title;
		std::string m_text;
		DocumentSyntax m_syntax;
	};

}
