// @otlicense

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

#include "OTGuiAPI/ButtonHandler.h"
#include "OTCommunication/ActionHandler.h"

#include "EntityCache.h"

// OC header
#include <TopoDS_Compound.hxx>

// C++ header
#include <string>

class EntityGeometry;
class EntityBrep;

class PrimitiveManager;
class BooleanOperations;
class UpdateManager;
class Transformations;
class ChamferEdges;
class BlendEdges;
class SimplifyRemoveFaces;
class STEPReader;

// Forward declaration
namespace ot {
	namespace components {
		class UiComponent;
		class ModelComponent;
	}
}

class Application : public ot::ApplicationBase, public ot::ActionHandler, public ot::ButtonHandler {
public:
	static Application& instance();

	// ##################################################################################################################################

	// Required functions

	//! @brief Will be called when a UI connected to the session and is ready to work
	virtual void uiConnected(ot::components::UiComponent * _ui) override;

	virtual void modelConnected(ot::components::ModelComponent* _model) override;

	virtual void modelDisconnected(const ot::components::ModelComponent* _serviceInfo) override;

	//! @brief Create settings that your application uses that are editable in the uiService
	//! The created class will be deleted after used for sending or synchronizing with the database.
	//! The created settings will be requested upon Service startup to synchronize with the database,
	//! aswell as when the uiService is connected
	virtual ot::PropertyGridCfg createSettings(void) const override;

	//! @brief This function will be called when the settings were synchronized with the database
	//! At this point the values from the dataset should be stored since the dataset will be deleted after this function call
	//! @param The dataset that contains all values
	virtual void settingsSynchronized(const ot::PropertyGridCfg& _dataset) override;

	//! @brief This function will be called when the settings were changed in the uiService
	//! The value of the provided item should be stored.
	//! If the change of the item will change the item visibility of any settings item, this function should return true,
	//! otherwise false. When returning true, the function createSettings() will be called and the created dataset will be
	//! send to the uiService to update the Settings in the dialog
	//! @param The item that has been changed in the uiService (instance will be deleted after this function call)
	virtual bool settingChanged(const ot::Property* _item) override;

	std::list<ot::EntityInformation> getSelectedGeometryEntities(void);
	EntityCache *getEntityCache(void) { return &entityCache; }
	virtual void modelSelectionChanged(void) override;
	virtual void propertyChanged(ot::JsonDocument& _doc) override;

	void actionHandlingCompleted();
	
	// ##################################################################################################################################

private:
	std::string CreateTmpFileFromCompressedData(const std::string &data, ot::UID uncompressedDataLength);
	
	PrimitiveManager    *getPrimitiveManager(void);
	BooleanOperations   *getBooleanOperations(void);
	UpdateManager       *getUpdateManager(void);
	Transformations     *getTransformationManager(void);
	ChamferEdges        *getChamferEdgesManager(void);
	BlendEdges          *getBlendEdgesManager(void);
	SimplifyRemoveFaces *getRemoveFacesOperation(void);
	STEPReader			*getSTEPReader(void);

private:
	EntityCache          entityCache;
	PrimitiveManager    *primitiveManager;
	BooleanOperations   *booleanOperations;
	UpdateManager       *updateManager;
	Transformations     *transformationManager;
	ChamferEdges        *chamferEdges;
	BlendEdges          *blendEdges;
	SimplifyRemoveFaces *removeFaces;
	STEPReader			*stepReader;

	void handleImportSTEP(ot::JsonDocument& _document);
	void handleCreateGeometryFromRubberband(ot::JsonDocument& _document);
	void handleEntitiesSelected(ot::JsonDocument& _document);


	ot::ToolBarButtonCfg m_buttonImportStep;

	ot::ToolBarButtonCfg m_buttonCreateCuboid;
	ot::ToolBarButtonCfg m_buttonCreateCylinder;
	ot::ToolBarButtonCfg m_buttonCreateSphere;
	ot::ToolBarButtonCfg m_buttonCreateTorus;
	ot::ToolBarButtonCfg m_buttonCreateCone;
	ot::ToolBarButtonCfg m_buttonCreatePyramid;

	ot::ToolBarButtonCfg m_buttonBooleanAdd;
	ot::ToolBarButtonCfg m_buttonBooleanSubtract;
	ot::ToolBarButtonCfg m_buttonBooleanIntersect;
	ot::ToolBarButtonCfg m_buttonTransform;
	ot::ToolBarButtonCfg m_buttonChamferEdges;
	ot::ToolBarButtonCfg m_buttonBlendEdges;

	ot::ToolBarButtonCfg m_buttonRemoveFaces;
	ot::ToolBarButtonCfg m_buttonHealing;

	void handleRequestImportSTEP();

	void handleCreateCuboid();
	void handleCreateCylinder();
	void handleCreateSphere();
	void handleCreateTorus();
	void handleCreateCone();
	void handleCreatePyramid();

	void handleBooleanAdd();
	void handleBooleanSubtract();
	void handleBooleanIntersect();
	void handleTransform();
	void handleChamferEdges();
	void handleBlendEdges();

	void handleRemoveFaces();
	void handleHealing();

	Application();
	virtual ~Application();
};
