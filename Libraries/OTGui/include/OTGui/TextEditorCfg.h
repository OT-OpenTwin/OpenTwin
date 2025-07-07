//! @file TextEditorCfg.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GuiTypes.h"
#include "OTGui/FileExtension.h"
#include "OTGui/WidgetViewBase.h"

// std header
#include <list>
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_GUI_API_EXPORT TextEditorCfg : public WidgetViewBase {
		OT_DECL_DEFCOPY(TextEditorCfg)
		OT_DECL_DEFMOVE(TextEditorCfg)
	public:
		TextEditorCfg();
		virtual ~TextEditorCfg();

		//! \brief Add the object contents to the provided JSON object.
		//! \param _object Json object reference to write the data to.
		//! \param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! \brief Set the object contents from the provided JSON object.
		//! \param _object The JSON object containing the information.
		//! \throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setPlainText(const std::string& _text) { m_text = _text; };
		const std::string& getPlainText(void) const { return m_text; };

		//! @brief Set read only enabled.
		//! @param _readOnly If true the text in the text editor can not be edited by the user.
		void setTextReadOnly(bool _readOnly) { m_readOnly = _readOnly; };
		bool getTextReadOnly(void) const { return m_readOnly; };
		
		void setDocumentSyntax(DocumentSyntax _syntax) { m_syntax = _syntax; };
		DocumentSyntax getDocumentSyntax(void) const { return m_syntax; };

		void setFileExtensionFilter(const std::list<FileExtension::DefaultFileExtension>& _extensions);
		void setFileExtensionFilter(const std::string& _filter) { m_fileExtensionFilter = _filter; };
		const std::string& getFileExtensionFilters() const { return m_fileExtensionFilter; };

	private:
		std::string m_text;
		bool m_readOnly;
		DocumentSyntax m_syntax;
		std::string m_fileExtensionFilter;
	};

}
