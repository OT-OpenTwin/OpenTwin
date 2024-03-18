//Service Header
#include "NGSpice.h"
#include "Application.h"
#include "Connection.h"
#include "CircuitElement.h"
//Open Twin Header
#include "EntityBlockCircuitElement.h"
#include "EntityBlockCircuitResistor.h"
#include "EntityBlockConnection.h"
//Third Party Header
#include <string>
namespace Numbers
{
	static unsigned long long nodeNumber = 0;
	static unsigned long long voltageSourceNetlistNumber = 0;
	static unsigned long long resistorNetlistNumber = 0;
	
}

void NGSpice::clearBufferStructure(std::string name)
{
	this->getMapOfCircuits().find(name)->second.getMapOfElements().clear();

}

void NGSpice::updateBufferClasses(std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> allConnectionEntities, std::map<ot::UID, std::shared_ptr<EntityBlock>>& allEntitiesByBlockID,std::string editorname)
{
	auto it = Application::instance()->getNGSpice().getMapOfCircuits().find(editorname);
	if ( it == Application::instance()->getNGSpice().getMapOfCircuits().end())
	{
		Circuit circuit;
		circuit.setEditorName(editorname);
		circuit.setId(editorname);
		Application::instance()->getNGSpice().getMapOfCircuits().insert_or_assign(editorname, circuit);
	}
	for (auto& blockEntityByID : allEntitiesByBlockID)
	{
		std::shared_ptr<EntityBlock> blockEntity = blockEntityByID.second;
		CircuitElement element;
		element.setEditorName(editorname);
		element.setItemName(blockEntity->getBlockTitle());
		element.setUID(blockEntity->getEntityID());

		if (blockEntity->getBlockTitle() == "Circuit Element")
		{
			auto myElement = dynamic_cast<EntityBlockCircuitElement*>(blockEntity.get());
			element.setValue(myElement->getElementType());
		}
		else if (blockEntity->getBlockTitle() == "Circuit Element Resistor")
		{
			auto myElement = dynamic_cast<EntityBlockCircuitResistor*>(blockEntity.get());
			element.setValue(myElement->getElementType());
		}

		auto it = Application::instance()->getNGSpice().getMapOfCircuits().find(editorname);
		it->second.addElement(element.getUID(), element);
	}
	
	for (auto& blockEntityByID : allEntitiesByBlockID)
	{
		std::shared_ptr<EntityBlock> blockEntity = blockEntityByID.second;
		auto connections = blockEntity->getAllConnections();
	

		for (auto connectionID : connections)
		{
			
			auto it = Application::instance()->getNGSpice().getMapOfCircuits().find(editorname);
			std::shared_ptr<EntityBlockConnection> connectionEntity = allConnectionEntities.at(connectionID);
			ot::GraphicsConnectionCfg connectionCfg = connectionEntity->getConnectionCfg();

			Connection conn(connectionCfg);
			conn.setNodeNumber(std::to_string(Numbers::nodeNumber++));

			// if the adding doesnt work because it already added the connection than it counts the Nodenumber++ but i dont need that to do it
			bool res1 = it->second.addConnection(connectionCfg.getOriginUid(), conn);
			bool res2 = it->second.addConnection(connectionCfg.getDestinationUid(), conn);
			if (res1 == false && res2 == false )
			{
				Numbers::nodeNumber--;
			}
						
		}
	}



}

//
//void NGSpice::setConnectionNodeNumbers(std::map<std::string, std::shared_ptr<EntityBlock>>& allEntitiesByBlockID)
//{
//	for (auto it : allEntitiesByBlockID)
//	{
//		std::shared_ptr<EntityBlock> blockEntity = it.second;
//		
//			for (auto conn : blockEntity->getAllConnections())
//			{
//				Connection connection(conn);
//				connection.setNodeNumber(std::to_string(Numbers::currentNodeNumber++));
//				
//				addConnection(connection.originUid(), connection);
//				addConnection(connection.destUid(), connection);
//
//			}
//		
//	}
//}

//std::string NGSpice::getBlockEntityTitleByUID(std::string UID, std::map<std::string, std::shared_ptr<EntityBlock>>& list)
//{
//	if (list.find(UID) != list.end())
//	{
//		return list[UID]->getBlockTitle();
//	}
//	else
//	{
//		OT_LOG_E("Entity Title not Found");
//	}
//}



std::string NGSpice::generateNetlist(std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> allConnectionEntities,std::map<ot::UID, std::shared_ptr<EntityBlock>>& allEntitiesByBlockID,std::string simulationType,std::string printSettings,std::string editorname)
{

	// Now I write the informations to the File
	// Here i get the Circuit and the map of Elements
	

		std::string TitleLine = "circbyline *Test";
		ngSpice_Command(const_cast<char*>(TitleLine.c_str()));

	auto it =Application::instance()->getNGSpice().getMapOfCircuits().find(editorname);
	 
	for (auto mapOfElements : it->second.getMapOfElements())
	{
		auto element = mapOfElements.second;
		std::string netlistElementName = "";
		std::string netlistLine="circbyline ";
		std::string netlistValue = element.getValue();
		std::string netlistNodeNumbers;

		if (element.getItemName() == "Circuit Element")
		{
			netlistElementName += "V" + std::to_string(++Numbers::voltageSourceNetlistNumber);
			netlistLine += netlistElementName + " ";
		}
		else if (element.getItemName() == "Circuit Element Resistor")
		{
			netlistElementName = "R" + std::to_string(++Numbers::resistorNetlistNumber);
			netlistLine += netlistElementName + " ";
		}

		auto connections = element.getList();
		for (auto conn : connections)
		{
			netlistNodeNumbers += conn.getNodeNumber() + " ";
		}

		netlistLine += netlistNodeNumbers;
		netlistLine += netlistValue;
		ngSpice_Command(const_cast<char*>(netlistLine.c_str()));
	}


	simulationType = "circbyline " + simulationType;
	printSettings = "circbyline " + printSettings;

	ngSpice_Command(const_cast<char*>(simulationType.c_str()));
	ngSpice_Command(const_cast<char*>("circbyline .Control"));
	ngSpice_Command(const_cast<char*>("circbyline run"));
	ngSpice_Command(const_cast<char*>(printSettings.c_str()));
	ngSpice_Command(const_cast<char*>("circbyline .endc"));
	ngSpice_Command(const_cast<char*>("circbyline .end"));



	return "succes";

	
}

std::string NGSpice::ngSpice_Initialize(std::map<ot::UID, std::shared_ptr<EntityBlockConnection>> allConnectionEntities,std::map<ot::UID, std::shared_ptr<EntityBlock>>& allEntitiesByBlockID,std::string editorname, std::string simulationType,std::string printSettings)
{
	SendChar* printfcn = MySendCharFunction;
	SendStat* statfcn = MySendStat;
	ControlledExit* ngexit = MyControlledExit;
	std::string myString;

	int status = ngSpice_Init(MySendCharFunction, MySendStat, MyControlledExit, nullptr, nullptr, nullptr, nullptr);

	if (status == 0)
	{
		OT_LOG_D("Worked");

		std::list<std::string> enabled;

		std::list<std::string> disabled;
		/*disabled.push_back("Circuit Simulator:Simulate:New Simulation");
		m_uiComponent->setControlsEnabledState(enabled, disabled);*/

	}
	else if (status == 1)
	{
		OT_LOG_E("Something went wrong");
	}



	/* Some simulation*/
	/* setConnectionNodeNumbers(allEntitiesByBlockID);*/
	 updateBufferClasses(allConnectionEntities,allEntitiesByBlockID,editorname);
	 generateNetlist(allConnectionEntities,allEntitiesByBlockID,simulationType,printSettings,editorname);

	 Numbers::nodeNumber = 0;
	 Numbers::resistorNetlistNumber = 0;
	 Numbers::voltageSourceNetlistNumber = 0;
	// clearBufferStructure(); // Must be corrected

	 
	
	/*char command[1000];
	const char* netlist = "C:/Users/Sebastian/Desktop/NGSpice_Dateien_Test/Test.cir";
	sprintf_s(command, sizeof(command), "source %s", netlist);
	ngSpice_Command(command);*/

	myString = std::to_string(status);

	return myString;
}


//Callback Functions for NGSpice
int NGSpice::MySendCharFunction(char* output, int ident, void* userData)
{
	Application::instance()->uiComponent()->displayMessage(std::string(output) + "\n");

	return 0;
}

int NGSpice::MySendStat(char* outputReturn, int ident, void* userData)
{
	Application::instance()->uiComponent()->displayMessage(std::string(outputReturn) + "\n");


	return 0;
}

int NGSpice::MyControlledExit(int exitstatus, bool immediate, bool quitexit, int ident, void* userdata)
{

	OT_LOG_D(std::to_string(exitstatus));

	return 0;

}

//void NGSpice::addConnection(std::string key, const Connection& obj)
//{
//	mapOfConnections.insert(std::make_pair(key, obj));
//}