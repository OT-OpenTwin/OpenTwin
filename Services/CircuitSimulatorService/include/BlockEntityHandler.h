#pragma once
//Service Header
#include "NGSpice.h"

// Open twin header
#include "OTServiceFoundation/BusinessLogicHandler.h"
#include "EntityBlock.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/FolderNames.h"
#include "OTGui/GraphicsConnectionCfg.h"
#include "OTGui/GraphicsPackage.h"
#include "EntityBlockConnection.h"
#include "ClassFactory.h"

// C++ header
#include <string>
#include <memory>

class BlockEntityHandler : public BusinessLogicHandler
{
public:
	std::shared_ptr<EntityBlock> CreateBlockEntity(const std::string& editorName, const std::string& blockName, ot::Point2DD& position);
	void UpdateBlockPosition(const ot::UID& blockID, const ot::Point2DD& position, const ot::Transform transform, ClassFactory* classFactory);
	void OrderUIToCreateBlockPicker();
	std::map<ot::UID, std::shared_ptr<EntityBlock>> findAllBlockEntitiesByBlockID();
	std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> findAllEntityBlockConnections();
	bool connectorHasTypeOut(std::shared_ptr<EntityBlock> blockEntity, const std::string& connectorName);
	void AddBlockConnection(const std::list<ot::GraphicsConnectionCfg>& connections,std::string name);
	void AddConnectionToConnection(const std::list<ot::GraphicsConnectionCfg>& connections, std::string editorName, ot::Point2DD pos);

	//Function for resultCurves of Simulation
	void createResultCurves(std::string solverName,std::string simulationType,std::string circuitName);

	//Setter
	void setPackageName(std::string name);
	
	//Getter
	const std::string getPackageName() const;
private:
	const std::string _blockFolder = "Circuits";
	std::string _packageName = "Circuit Simulator";
	

	std::string InitSpecialisedCircuitElementEntity(std::shared_ptr<EntityBlock> blockEntity);
	ot::GraphicsPickerCollectionPackage* BuildUpBlockPicker();
};


