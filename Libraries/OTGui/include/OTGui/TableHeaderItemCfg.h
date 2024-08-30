//! @file TableHeaderItemCfg.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"
#include "OTGui/OTGuiAPIExport.h"

namespace ot {

	class OT_GUI_API_EXPORT TableHeaderItemCfg : public Serializable {
	public:
		enum ResizeMode {
			Default,
			ResizeToContents,
			Stretch
		};

		static ResizeMode stringToResizeMode(const std::string& _mode);
		static std::string resizeModeToString(ResizeMode _mode);

		TableHeaderItemCfg();
		TableHeaderItemCfg(const std::string& _text, ResizeMode _resizeMode = ResizeMode::Default);
		TableHeaderItemCfg(const TableHeaderItemCfg& _other);
		virtual ~TableHeaderItemCfg();

		TableHeaderItemCfg& operator = (const TableHeaderItemCfg& _other);

		//! \brief Add the object contents to the provided JSON object.
		//! \param _object Json object reference to write the data to.
		//! \param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! \brief Set the object contents from the provided JSON object.
		//! \param _object The JSON object containing the information.
		//! \throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

	private:
		ResizeMode m_resizeMode;
		std::string m_text;
	};

}