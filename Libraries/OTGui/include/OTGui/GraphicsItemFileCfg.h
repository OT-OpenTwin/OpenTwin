// @otlicense

//! @file GraphicsItemCfg.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GraphicsItemCfg.h"

namespace ot {

	class OT_GUI_API_EXPORT GraphicsItemFileCfg : public GraphicsItemCfg {
		OT_DECL_NOMOVE(GraphicsItemFileCfg)
	public:
		static std::string className() { return "GraphicsItemFileCfg"; };

		GraphicsItemFileCfg();
		GraphicsItemFileCfg(const std::string& _subPath);
		GraphicsItemFileCfg(const GraphicsItemFileCfg& _other);
		virtual ~GraphicsItemFileCfg();

		GraphicsItemFileCfg& operator = (const GraphicsItemFileCfg&) = delete;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Base class functions

		//! @brief Creates a copy of this item.
		virtual GraphicsItemCfg* createCopy() const override { return new GraphicsItemFileCfg(*this); };

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		//! @brief Returns the key that is used to create an instance of this class in the simple factory
		virtual std::string getFactoryKey() const override { return GraphicsItemFileCfg::className(); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setFile(const std::string& _subPath) { m_file = _subPath; };
		const std::string& getFile() const { return m_file; };

	private:
		std::string m_file; //! @brief Starting point.
	};

}