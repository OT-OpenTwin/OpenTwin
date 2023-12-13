// Service header
#include "BlockEntityHandler.h"

// Open twin header
#include "OTCommunication/ActionTypes.h"
//#include "ExternalDependencies.h"
#include "Application.h"
#include "ClassFactoryBlock.h"
#include "ClassFactory.h"
#include "EntityBlockCircuitElement.h"
// Third Party Header

void BlockEntityHandler::CreateBlockEntity(const std::string& editorName, const std::string& blockName, ot::Point2DD& position)
{
	ClassFactoryBlock factory;
	EntityBase* baseEntity = factory.CreateEntity(blockName);
	assert(baseEntity != nullptr);
	std::shared_ptr<EntityBlock> blockEntity(dynamic_cast<EntityBlock*>(baseEntity));

	std::string entName = CreateNewUniqueTopologyName(_blockFolder + "/" + editorName, blockEntity->getBlockTitle());
	blockEntity->setName(entName);
	blockEntity->SetServiceInformation(Application::instance()->getBasicServiceInformation());
	blockEntity->setOwningService(OT_INFO_SERVICE_TYPE_CircuitSimulatorService);
	blockEntity->setEntityID(_modelComponent->createEntityUID());
	blockEntity->SetGraphicsScenePackageName(_packageName);

	std::unique_ptr<EntityCoordinates2D> blockCoordinates(new EntityCoordinates2D(_modelComponent->createEntityUID(), nullptr, nullptr, nullptr, nullptr, OT_INFO_SERVICE_TYPE_CircuitSimulatorService));
	blockCoordinates->setCoordinates(position);
	blockCoordinates->StoreToDataBase();

	blockEntity->setCoordinateEntityID(blockCoordinates->getEntityID());

	//Von Blockentity in CircuitEntity casten und createProperties aufrufen
	//InitSpecialisedBlockEntity(blockEntity);

	EntityBlockCircuitElement* CircuitElement = dynamic_cast<EntityBlockCircuitElement*>(blockEntity.get());
	CircuitElement->createProperties();

	blockEntity->StoreToDataBase();
	_modelComponent->addEntitiesToModel({ blockEntity->getEntityID() }, { blockEntity->getEntityStorageVersion() }, { false }, { blockCoordinates->getEntityID() }, { blockCoordinates->getEntityStorageVersion() }, { blockEntity->getEntityID() }, "Added Block: " + blockName);
}

void BlockEntityHandler::OrderUIToCreateBlockPicker()
{
	auto graphicsEditorPackage = BuildUpBlockPicker();
	ot::JsonDocument doc;
	ot::JsonObject pckgObj;
	graphicsEditorPackage->addToJsonObject(pckgObj, doc.GetAllocator());

	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_GRAPHICSEDITOR_CreateGraphicsEditor, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_GRAPHICSEDITOR_Package, pckgObj, doc.GetAllocator());

	Application::instance()->getBasicServiceInformation().addToJsonObject(doc, doc.GetAllocator());

	// Message is queued, no response here
	_uiComponent->sendMessage(true, doc);
}

ot::GraphicsNewEditorPackage* BlockEntityHandler::BuildUpBlockPicker()
{
	ot::GraphicsNewEditorPackage* pckg = new ot::GraphicsNewEditorPackage(_packageName, _packageName);
	ot::GraphicsCollectionCfg* a = new ot::GraphicsCollectionCfg("CircuitElements", "Circuit Elements");
	ot::GraphicsCollectionCfg* a1 = new ot::GraphicsCollectionCfg("PassiveElements", "Passive Elements");
	a->addChildCollection(a1);
	EntityBlockCircuitElement element(0, nullptr, nullptr, nullptr,nullptr, "");
	a1->addItem(element.CreateBlockCfg());

	pckg->addCollection(a);

	return pckg;
}