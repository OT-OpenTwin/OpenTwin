// @otlicense
// File: ViewVisualisationHandler.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include <stdafx.h>

// Service header
#include "Model.h"
#include "Application.h"
#include "Handler/ViewVisualisationHandler.h"

// OpenTwin header
#include "OTCore/Debugging/RuntimeTests.h"
#include "OTModelEntities/Visualization/IVisualisationText.h"
#include "OTModelEntities/Visualization/IVisualisationTable.h"
#include "OTModelEntities/Visualization/IVisualisationPlot1D.h"
#include "OTModelEntities/Visualization/IVisualisationCurve.h"
#include "OTModelEntities/Visualization/IVisualisationGraphicsView.h"
#include "OTBlockEntities/EntityBlock.h"
#include "OTBlockEntities/EntityBlockConnection.h"

#if OT_TESTING_GLOBAL_AllTestsEnabled==true
#define OT_TESTING_LOCAL_VISUALIZATIONHANDLER_PERFORMANCETEST_ENABLED OT_TESTING_GLOBAL_AllTestsEnabled
#elif OT_TESTING_GLOBAL_RuntimeTestingEnabled==true
#define OT_TESTING_LOCAL_VISUALIZATIONHANDLER_PERFORMANCETEST_ENABLED OT_TESTING_GLOBAL_RuntimeTestingEnabled
#else
#define OT_TESTING_LOCAL_VISUALIZATIONHANDLER_PERFORMANCETEST_ENABLED false
#endif 

#if OT_TESTING_LOCAL_VISUALIZATIONHANDLER_PERFORMANCETEST_ENABLED==true
#define OT_TEST_VISUALIZATIONHANDLER_Interval(___testText) OT_TEST_Interval(ot_intern_visualizationhandler_lcl_performancetest, "ViewVisualisationHandler", ___testText)
#else
#define OT_TEST_VISUALIZATIONHANDLER_Interval(___testText)
#endif

void ViewVisualisationHandler::handleVisualisationRequest(ot::UID _entityID, ot::VisualisationCfg& _visualisationCfg)
{
	Model* model = Application::instance()->getModel();
	EntityBase* baseEntity = model->getEntityByID(_entityID);
	assert(baseEntity != nullptr);

	ot::JsonDocument document;
	
	document.AddMember(OT_ACTION_PARAM_VisualisationConfig, ot::JsonObject(_visualisationCfg, document.GetAllocator()), document.GetAllocator());
	
	if (_visualisationCfg.getVisualisationType() == OT_ACTION_CMD_UI_TABLE_Setup)
	{

		OT_TEST_VISUALIZATIONHANDLER_Interval("Visualize table");
		ot::IVisualisationTable* tableEntity = dynamic_cast<ot::IVisualisationTable*>(baseEntity);
		assert(tableEntity != nullptr);
		if (tableEntity != nullptr && tableEntity->visualiseTable())
		{
			document.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_TABLE_Setup, document.GetAllocator());

			ot::TableCfg tableCfg = tableEntity->getTableConfig(_visualisationCfg.getOverrideViewerContent());

			if (_visualisationCfg.getCustomViewFlags().has_value()) {
				tableCfg.setViewFlags(_visualisationCfg.getCustomViewFlags().value());
			}

			ot::JsonObject cfgObj;
			tableCfg.addToJsonObject(cfgObj, document.GetAllocator());

			document.AddMember(OT_ACTION_PARAM_Config, cfgObj, document.GetAllocator());

			Application::instance()->queuedRequestToFrontend(document);
		}
	}
	else if (_visualisationCfg.getVisualisationType() == OT_ACTION_CMD_UI_TEXTEDITOR_Setup)
	{
		OT_TEST_VISUALIZATIONHANDLER_Interval("Visualize text");

		ot::IVisualisationText* textEntity = dynamic_cast<ot::IVisualisationText*>(baseEntity);
		assert(textEntity != nullptr);
		if (textEntity != nullptr && (textEntity->visualiseText() || _visualisationCfg.getIsAppend()))
		{
			document.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_TEXTEDITOR_Setup, document.GetAllocator());

			ot::TextEditorCfg configuration = textEntity->getTextConfig(_visualisationCfg);

			if (_visualisationCfg.getCustomViewFlags().has_value()) {
				configuration.setViewFlags(_visualisationCfg.getCustomViewFlags().value());
			}

			ot::JsonObject cfgObj;
			configuration.addToJsonObject(cfgObj, document.GetAllocator());

			document.AddMember(OT_ACTION_PARAM_Config, cfgObj, document.GetAllocator());

			Application::instance()->queuedRequestToFrontend(document);
		}
	}
	else if (_visualisationCfg.getVisualisationType() == OT_ACTION_CMD_VIEW1D_Setup)
	{
		ot::IVisualisationPlot1D* plotEntity = dynamic_cast<ot::IVisualisationPlot1D*>(baseEntity);
		if (plotEntity != nullptr && plotEntity->visualisePlot())
		{
			EntityContainer* containerEntity = dynamic_cast<EntityContainer*>(plotEntity);
			std::list<EntityBase*> curveEntities = containerEntity->getChildrenList();

			document.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_VIEW1D_Setup, document.GetAllocator());
			
			ot::Plot1DCfg plotCfg = plotEntity->getPlot();

			if (_visualisationCfg.getCustomViewFlags().has_value()) {
				plotCfg.setViewFlags(_visualisationCfg.getCustomViewFlags().value());
			}

			ot::JsonObject cfgObj;
			plotCfg.addToJsonObject(cfgObj, document.GetAllocator());
			document.AddMember(OT_ACTION_PARAM_Config, cfgObj, document.GetAllocator());

			if(_visualisationCfg.getOverrideViewerContent())
			{
				ot::JsonArray curveCfgs;
				for (EntityBase* curveEntity : curveEntities)
				{
					ot::IVisualisationCurve* curve = dynamic_cast<ot::IVisualisationCurve*>(curveEntity);
					ot::Plot1DCurveCfg curveCfg = curve->getCurve();
					ot::JsonObject curveCfgSerialised;
					curveCfg.addToJsonObject(curveCfgSerialised, document.GetAllocator());
					curveCfgs.PushBack(curveCfgSerialised, document.GetAllocator());
				}

				document.AddMember(OT_ACTION_PARAM_VIEW1D_CurveConfigs, curveCfgs, document.GetAllocator());
			}

			Application::instance()->queuedRequestToFrontend(document);
		}
		else
		{
			OT_LOG_E("Tried visualising an entity as plot which is not a plot.");
		}
	}
	else if (_visualisationCfg.getVisualisationType() == OT_ACTION_CMD_UI_GRAPHICSEDITOR_CreateGraphicsEditor)
	{
		ot::IVisualisationGraphicsView* graphicsView = dynamic_cast<ot::IVisualisationGraphicsView*>(baseEntity);
		if (graphicsView != nullptr && graphicsView->visualiseGraphicsView())
		{
			document.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_CreateGraphicsEditor, document.GetAllocator()), document.GetAllocator());

			ot::GraphicsNewEditorPackage* editor = graphicsView->getGraphicsEditorPackage();
			ot::JsonObject pckgObj;
			editor->addToJsonObject(pckgObj, document.GetAllocator());
			document.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj, document.GetAllocator());
			
			setupGraphicsScene(baseEntity);

			Application::instance()->queuedRequestToFrontend(document);
		}
	}
	else
	{
		OT_LOG_W("Unknown visualization request: \"" + _visualisationCfg.getVisualisationType() + "\"");
	}
}

void ViewVisualisationHandler::requestVisualisationIfNeeded(ot::UID _entityID)
{
	Model* model = Application::instance()->getModel();
	OTAssertNullptr(model);

	ot::JsonDocument document;
	document.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_RequestVisualizationIfNeeded, document.GetAllocator());
	document.AddMember(OT_ACTION_PARAM_MODEL_ID, model->getVisualizationModel(), document.GetAllocator());
	document.AddMember(OT_ACTION_PARAM_MODEL_EntityID, _entityID, document.GetAllocator());

	Application::instance()->queuedRequestToFrontend(document);
}

void ViewVisualisationHandler::handleRenaming(ot::UID _entityID)
{
	Model* model = Application::instance()->getModel();
	EntityBase* baseEntity = model->getEntityByID(_entityID);
	assert(baseEntity != nullptr);
	const bool includeData = false;
	
	ot::IVisualisationTable* tableEntity = dynamic_cast<ot::IVisualisationTable*>(baseEntity);
	
	ot::JsonDocument documentBase;
	ot::VisualisationCfg visualisationCfg;
	visualisationCfg.setAsActiveView(true);
	visualisationCfg.setOverrideViewerContent(includeData);
	ot::JsonObject visualisationCfgJson;
	visualisationCfg.addToJsonObject(visualisationCfgJson, documentBase.GetAllocator());
	documentBase.AddMember(OT_ACTION_PARAM_VisualisationConfig, visualisationCfgJson, documentBase.GetAllocator());

	if (tableEntity != nullptr && tableEntity->visualiseTable())
	{
		ot::JsonDocument singleRequest;
		singleRequest.CopyFrom(documentBase, singleRequest.GetAllocator());

		singleRequest.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_TEXTEDITOR_Setup, singleRequest.GetAllocator());
		
		ot::TableCfg tableCfg = tableEntity->getTableConfig(includeData);
		ot::JsonObject cfgObj;
		tableCfg.addToJsonObject(cfgObj, singleRequest.GetAllocator());

		singleRequest.AddMember(OT_ACTION_PARAM_Config, cfgObj, singleRequest.GetAllocator());

		Application::instance()->queuedRequestToFrontend(singleRequest);
	}

	ot::IVisualisationText* textEntity = dynamic_cast<ot::IVisualisationText*>(baseEntity);
	if (textEntity != nullptr && textEntity->visualiseText())
	{
		ot::JsonDocument singleRequest;
		singleRequest.CopyFrom(documentBase, singleRequest.GetAllocator());

		singleRequest.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_TEXTEDITOR_Setup, singleRequest.GetAllocator());

		ot::VisualisationCfg vizCfg;
		vizCfg.setOverrideViewerContent(false);

		ot::TextEditorCfg configuration = textEntity->getTextConfig(vizCfg);
		ot::JsonObject cfgObj;
		configuration.addToJsonObject(cfgObj, singleRequest.GetAllocator());

		singleRequest.AddMember(OT_ACTION_PARAM_Config, cfgObj, singleRequest.GetAllocator());
		
		Application::instance()->queuedRequestToFrontend(singleRequest);
	}

	ot::IVisualisationPlot1D* plotEntity = dynamic_cast<ot::IVisualisationPlot1D*>(baseEntity);
	if (plotEntity != nullptr && plotEntity->visualisePlot())
	{
		ot::JsonDocument singleRequest;
		singleRequest.CopyFrom(documentBase, singleRequest.GetAllocator());

		singleRequest.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_VIEW1D_Setup, singleRequest.GetAllocator());

		const ot::Plot1DCfg plotCfg = plotEntity->getPlot();
		ot::JsonObject cfgObj;
		plotCfg.addToJsonObject(cfgObj, singleRequest.GetAllocator());
		singleRequest.AddMember(OT_ACTION_PARAM_Config, cfgObj, singleRequest.GetAllocator());

		Application::instance()->queuedRequestToFrontend(singleRequest);
	}

	ot::IVisualisationCurve* curve = dynamic_cast<ot::IVisualisationCurve*>(baseEntity);
	if (curve != nullptr && curve->visualiseCurve())
	{
		ot::JsonDocument singleRequest;
		singleRequest.CopyFrom(documentBase, singleRequest.GetAllocator());

		singleRequest.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UpdateCurvesOfPlot, singleRequest.GetAllocator());

		const std::string plotName = baseEntity->getParent()->getName();
		singleRequest.AddMember(OT_ACTION_PARAM_NAME, ot::JsonString(plotName, singleRequest.GetAllocator()), singleRequest.GetAllocator());

		ot::JsonObject curveCfgSerialised;
		ot::Plot1DCurveCfg curveCfg = curve->getCurve();
		curveCfg.addToJsonObject(curveCfgSerialised, singleRequest.GetAllocator());
		singleRequest.AddMember(OT_ACTION_PARAM_VIEW1D_CurveConfigs, curveCfgSerialised, singleRequest.GetAllocator());

		Application::instance()->queuedRequestToFrontend(singleRequest);
	}

}

#include "OTGui/Graphics/Builder/GraphicsHierarchicalItemBuilder.h"
#include "QueuingHttpRequestsRAII.h"
#include "OTGui/Painter/StyleRefPainter2D.h"
void ViewVisualisationHandler::handleDependencyGraphRequest()
{
	QueuingHttpRequestsRAII raiiUI;
	const std::string sceneName = "Dependency Graph";
	/*
	ot::GraphicsNewEditorPackage editor(sceneName, sceneName);
	
	ot::JsonObject pckgObj;
	
	ot::JsonDocument document;
	editor.addToJsonObject(pckgObj, document.GetAllocator());
	document.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_CreateGraphicsEditor, document.GetAllocator()), document.GetAllocator());
	document.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj, document.GetAllocator());
	Application::instance()->queuedRequestToFrontend(document);*/

	ot::GraphicsHierarchicalItemBuilder builder;
	std::list<std::string> vertexNames = { "Data file", "Series data", "Post processed" };
	uint64_t vertexID = 0;
	std::list<ot::Point2DD> positions = { ot::Point2DD(0., 0.), ot::Point2DD(-180, 0), ot::Point2DD(180, 0.) };
	auto currentPosition = positions.begin();

	/*const std::string graphicsSceneName = ot::BlockConfigurationHelper::getGraphicSceneName(getName(), m_graphicsScenePackageChildName);

	ot::GraphicsScenePackage pckg(graphicsSceneName);
	pckg.addItem(blockCfg);
	pckg.setPickerKey(m_graphicsPickerKey);*/
	ot::GraphicsScenePackage pckg(sceneName);
	std::list < ot::GraphicsItemCfg*> blockCfgs;
	for (const std::string& name : vertexNames)
	{
		builder.setEntityName(name);
		builder.setTopText(name);
		builder.setBackgroundShape(ot::GraphicsHierarchicalItemBuilder::BackgroundShape::Ellipse);
		blockCfgs.push_back(builder.createGraphicsItem());
		blockCfgs.back()->setUid(vertexID++);
		blockCfgs.back()->setPosition((*currentPosition));
		currentPosition++;
		pckg.addItem(blockCfgs.back());
	}


	auto allCfgs = blockCfgs.begin();
	std::string originConnector = ot::GraphicsHierarchicalItemBuilder::createConnectorItemName(ot::Alignment::Right);
	allCfgs++;
	std::string destConnector = ot::GraphicsHierarchicalItemBuilder::createConnectorItemName(ot::Alignment::Left);
	ot::GraphicsConnectionCfg cfg1 (0, originConnector, 1, destConnector);
	
	cfg1.setLinePainter(new ot::StyleRefPainter2D(ot::ColorStyleValueEntry::GraphicsItemConnection));
	cfg1.setUid(13);
	ot::GraphicsConnectionPackage connPkg(sceneName);
	connPkg.addConnection(cfg1);

	ot::JsonDocument reqDoc;
	reqDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddItem, reqDoc.GetAllocator()), reqDoc.GetAllocator());
	ot::VisualisationCfg visualisationCfg;
	ot::JsonObject visualisationCfgJson;
	visualisationCfg.addToJsonObject(visualisationCfgJson, reqDoc.GetAllocator());
	reqDoc.AddMember(OT_ACTION_PARAM_VisualisationConfig, visualisationCfgJson, reqDoc.GetAllocator());

	ot::JsonObject pckgObj;
	pckg.addToJsonObject(pckgObj, reqDoc.GetAllocator());
	reqDoc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj, reqDoc.GetAllocator());

	Application::instance()->queuedRequestToFrontend(reqDoc);


	ot::JsonDocument connReqDoc;
	connReqDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection, reqDoc.GetAllocator()), reqDoc.GetAllocator());
	ot::VisualisationCfg v;
	ot::JsonObject vObj;
	v.addToJsonObject(vObj, connReqDoc.GetAllocator());
	connReqDoc.AddMember(OT_ACTION_PARAM_VisualisationConfig, vObj, connReqDoc.GetAllocator());

	ot::JsonObject connPckgObj;
	connPkg.addToJsonObject(connPckgObj, connReqDoc.GetAllocator());
	connReqDoc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, connPckgObj, reqDoc.GetAllocator());

	Application::instance()->queuedRequestToFrontend(connReqDoc);

	//ot::PenFCfg outlineCfg;

	///*const EntityPropertiesGuiPainter* painterProperty = dynamic_cast<const EntityPropertiesGuiPainter*>(this->getProperties().getProperty("Line Painter"));
	//outlineCfg.setPainter(painterProperty->getValue()->createCopy());

	//const EntityPropertiesDouble* lineWidthProperty = dynamic_cast<const EntityPropertiesDouble*>(this->getProperties().getProperty("Line Width"));
	//outlineCfg.setWidth(lineWidthProperty->getValue());

	//const EntityPropertiesSelection* lineStyleProperty = dynamic_cast<const EntityPropertiesSelection*>(this->getProperties().getProperty("Line Style"));
	//outlineCfg.setStyle(ot::stringToLineStyle(lineStyleProperty->getValue()));

	//cfg.setLineStyle(outlineCfg);*/

	///*const EntityPropertiesSelection* lineShapeProperty = dynamic_cast<const EntityPropertiesSelection*>(this->getProperties().getProperty("Line Shape"));
	//cfg.setLineShape(ot::GraphicsConnectionCfg::stringToShape(lineShapeProperty->getValue()));*/

	//cfg.setDestinationPos(ot::Point2DD(0., 0.));
	//cfg.setOriginPos(ot::Point2DD(-5, 0));
	//cfg.setUid(9);

	//ot::GraphicsConnectionPackage connectionPckg(sceneName);
	//

	//connectionPckg.setPickerKey(OT_INFO_SERVICE_TYPE_MODEL);
	//connectionPckg.addConnection(cfg);
	//
	//ot::JsonDocument reqDoc;
	//reqDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_AddConnection, reqDoc.GetAllocator()), reqDoc.GetAllocator());

	//ot::VisualisationCfg visualisationCfg;
	//ot::JsonObject visualisationCfgJson;
	//visualisationCfg.addToJsonObject(visualisationCfgJson, reqDoc.GetAllocator());
	//reqDoc.AddMember(OT_ACTION_PARAM_VisualisationConfig, visualisationCfgJson, reqDoc.GetAllocator());
	//reqDoc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj, reqDoc.GetAllocator());
	//Application::instance()->queuedRequestToFrontend(reqDoc);

	//graph = createGraph()

	//	createGraphViews(graph)
	//{
	//	createSceneConfig();
	//	buildPositionGrid(vertices);
	//	radius = 5 ppt;
	//	angle = alpha;


	//	for (auto vertex : graph.vertices)
	//	{
	//		if (vertex.isView())
	//		{
	//			createItemCfg(vertex)
	//		}
	//	}
	//	for (auto edge : graph.edges)
	//	{
	//		crateConneectionCfg(edge)
	//	}
	//}

}

void ViewVisualisationHandler::setupPlot(EntityBase* _plotEntityBase, bool _setAsActiveView)
{

}

void ViewVisualisationHandler::setupGraphicsScene(EntityBase* _container)
{
	EntityContainer* container = dynamic_cast<EntityContainer*>(_container);
	if (container != nullptr)
	{
		std::list<EntityBase*> children = container->getChildrenList();
		for (EntityBase* child : children)
		{
			ot::EntityBlock* childBlock = dynamic_cast<ot::EntityBlock*>(child);
			if (childBlock != nullptr)
			{
				childBlock->createBlockItem();
			}
			else
			{
				ot::EntityBlockConnection* connection = dynamic_cast<ot::EntityBlockConnection*>(child);
				if (connection != nullptr)
				{
					connection->createConnectionItem();
				}
				else
				{
					//Maybe we have another folder inbetween the scene entity and the block entities.
					setupGraphicsScene(child);
				}
			}
		}
	}
}
