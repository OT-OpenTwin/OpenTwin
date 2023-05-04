//! @file FlowBlockConfiguration.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OpenTwinCore/Color.h"
#include "OTBlockEditorAPI/BlockEditorAPIAPIExport.h"
#include "OTBlockEditorAPI/BlockConfiguration.h"

// std header
#include <string>
#include <list>

#define OT_FLOWBLOCKCONFIGURATION_TYPE "FlowBlock"

namespace ot {

	class BlockConnectorConfiguration;
	class Painter2D;

	class BLOCKEDITORAPI_API_EXPORT FlowBlockConfiguration : public ot::BlockConfiguration {
	public:
		FlowBlockConfiguration(const std::string& _name = std::string());
		virtual ~FlowBlockConfiguration();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		// ########################################################################################################################################################

		// Setter/Getter

		//! @brief Returns the block type as a string
		virtual std::string type(void) const { return OT_FLOWBLOCKCONFIGURATION_TYPE; };

		void setBackgroundIconSubPath(const std::string& _path) { m_backgroundIconSubPath = _path; };
		const std::string& backgroundIconSubPath(void) const { return m_backgroundIconSubPath; };

		void setBackgroundPainter(Painter2D *& _painter);
		Painter2D *& backgroundPainter(void) { return m_backgroundPainter; };

		void setTitleIconSubPath(const std::string& _path) { m_titleIconSubPath = _path; };
		const std::string& titleIconSubPath(void) const { return m_titleIconSubPath; };

		void setTitleColorLeft(const Color& _color) { m_titleColorLeft = _color; };
		const Color& titleColorLeft(void) const { return m_titleColorLeft; };

		void setTitleColorRight(const Color& _color) { m_titleColorRight = _color; };
		const Color& titleColorRight(void) const { return m_titleColorRight; };

		void setTitleTextColor(const Color& _color) { m_titleTextColor = _color; };
		const Color& titleTextColor(void) const { return m_titleTextColor; };

		void addInput(BlockConnectorConfiguration* _block);
		const std::list<BlockConnectorConfiguration*>& inputs(void) const { return m_inputs; };

		void addOutput(BlockConnectorConfiguration* _block);
		const std::list<BlockConnectorConfiguration*>& outputs(void) const { return m_outputs; };

	private:
		std::string m_backgroundIconSubPath;
		ot::Painter2D * m_backgroundPainter;
		std::string m_titleIconSubPath;
		ot::Color m_titleColorLeft;
		ot::Color m_titleColorRight;
		ot::Color m_titleTextColor;
		std::list<BlockConnectorConfiguration*> m_inputs;
		std::list<BlockConnectorConfiguration*> m_outputs;
		std::string m_bottomLeftIconSubPath;
		std::string m_bottomRightIconSubPath;

		FlowBlockConfiguration(FlowBlockConfiguration&) = delete;
		FlowBlockConfiguration& operator = (FlowBlockConfiguration&) = delete;
	};

}