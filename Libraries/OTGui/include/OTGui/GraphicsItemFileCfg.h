//! @file GraphicsItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GraphicsItemCfg.h"

#define OT_FactoryKey_GraphicsFileItem "OT_GIFile"

namespace ot {

	class OT_GUI_API_EXPORT GraphicsItemFileCfg : public GraphicsItemCfg {
		OT_DECL_NOCOPY(GraphicsItemFileCfg)
	public:
		GraphicsItemFileCfg();
		GraphicsItemFileCfg(const std::string& _subPath);
		virtual ~GraphicsItemFileCfg();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		//! @brief Creates a copy of this item.
		virtual GraphicsItemCfg* createCopy(void) const override;

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string getFactoryKey(void) const override { return std::string(OT_FactoryKey_GraphicsFileItem); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setFile(const std::string& _subPath) { m_file = _subPath; };
		const std::string& getFile(void) const { return m_file; };

	private:
		std::string m_file; //! @brief Starting point.
	};

}