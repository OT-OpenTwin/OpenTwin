//! @file PDFWidgetCfg.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/WidgetViewBase.h"

// std header
#include <list>
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_GUI_API_EXPORT PDFWidgetCfg : public WidgetViewBase {
		OT_DECL_DEFCOPY(PDFWidgetCfg)
		OT_DECL_DEFMOVE(PDFWidgetCfg)
	public:
		PDFWidgetCfg();
		virtual ~PDFWidgetCfg() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Serialization

		//! @brief Add the object contents to the provided JSON object.
		//! @param _object Json object reference to write the data to.
		//! @param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter/Getter

		void setPDFData(const std::vector<char>& _data) { m_pdfData = _data; };
		void setPDFData(std::vector<char>&& _data) { m_pdfData = std::move(_data); };
		const std::vector<char>& getPDFData() const { return m_pdfData; };

	private:
		std::vector<char> m_pdfData;
	};
}