//! @file Font.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/OTGuiAPIExport.h"
#include "OTGui/GuiTypes.h"
#include "OTCore/Serializable.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_GUI_API_EXPORT Font : public Serializable {
	public:
		Font();
		Font(FontFamily _fontFamily, int _sizePx = 12, bool _isBold = false, bool _isItalic = false);
		Font(const std::string& _fontFamily, int _sizePx = 12, bool _isBold = false, bool _isItalic = false);
		Font(const Font& _other);
		virtual ~Font();

		Font& operator = (const Font& _other);
		bool operator == (const Font& _other) const;
		bool operator != (const Font& _other) const;

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		void setFamily(const std::string& _fontFamily) { m_family = _fontFamily; };
		void setFamily(FontFamily _fontFamily);
		const std::string& family(void) const { return m_family; };

		void setSize(int _px) { m_sizePx = _px; };
		int size(void) const { return m_sizePx; };

		void setBold(bool _isBold = true) { m_isBold = _isBold; };
		bool isBold(void) const { return m_isBold; };

		void setItalic(bool _isItalic = true) { m_isItalic = _isItalic; };
		bool isItalic(void) const { return m_isItalic; };

	private:
		std::string m_family;
		int m_sizePx;
		bool m_isBold;
		bool m_isItalic;
	};

}