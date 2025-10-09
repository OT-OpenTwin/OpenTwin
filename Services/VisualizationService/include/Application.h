/*
 * Application.h
 *
 *  Created on:
 *	Author:
 *  Copyright (c)
 */

#pragma once

// Open twin header
#include "OTServiceFoundation/ApplicationBase.h"		// Base class
#include "EntityInformation.h"

// C++ header
#include <string>
#include <list>
#include <map>

// Forward declaration
class EntityBase;
namespace ot {
	namespace components {
		class UiComponent;
		class ModelComponent;
	}
}

class EntityVis2D3D;

class Application : public ot::ApplicationBase {
public:
	Application();
	virtual ~Application();

	// ##################################################################################################################################

	// Required functions

	//! @brief Will be called when a UI connected to the session and is ready to work
	virtual void uiConnected(ot::components::UiComponent * _ui) override;

	// ##################################################################################################################################

	// Add your custom functions/ members here

	void propertyChanged(ot::JsonDocument& _doc) override;
private:
	void updateEntities(std::list<ot::UID> &entityIDs, std::list<ot::UID> &entityVersions, bool itemsVisible);
	void updateSingleEntity(ot::UID entityID, ot::UID entityVersion, bool itemsVisible);
	std::pair<ot::UID, ot::UID> storeBinaryData(const char *data, size_t dataLength);
	void sendNewVisualizationDataToModeler(EntityVis2D3D *visEntity, ot::UID binaryDataItemID, ot::UID binaryDataItemVersion);
	std::pair<ot::UID, ot::UID> createDataItems(EntityVis2D3D *visEntity);
};
