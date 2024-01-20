//Service Header
#include "NGSpice.h"
#include "Application.h"
#include "Connection.h"
#include "CircuitElement.h"
//Open Twin Header
#include "EntityBlockCircuitElement.h"
#include "EntityBlockCircuitResistor.h"
//Third Party Header
#include <string>
namespace Numbers
{

	static unsigned long long voltageSourceNetlistNumber = 0;
	static unsigned long long resistorNetlistNumber = 0;
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



std::string NGSpice::generateNetlist(std::map<std::string, std::shared_ptr<EntityBlock>>& allEntitiesByBlockID)
{
	/*std::string Title = "*Test";
	outfile << Title << std::endl;

	int DiodeCounter = 0;
	int TransistorCounter = 0;*/

	// Note: The connector is not yet added to the netlist, and the transistor values along with the model are missing.
	// Additionally, appropriate simulation settings for the transistor are still pending.
	
	/*std::string VoltageSourceNetlistLine = "";
	std::string ResistorNetlsitLine = "";

	for (auto it : mapOfConnections)
	{
		if (getBlockEntityTitleByUID(it.first,allEntitiesByBlockID) == "Circuit Element")
		{
			VoltageSourceNetlistLine += "V" +std::to_string( Numbers::resistorNetlistNumber++);
			VoltageSourceNetlistLine += it.second.getNodeNumber();


		}
		else if (getBlockEntityTitleByUID(it.first,allEntitiesByBlockID) == "Circuit Element Resistor")
		{
				
		}
	}*/

	


	// Now I write the informations to the File
	// Here i get the Circuit and the map of Elements

		std::string TitleLine = "circbyline *Test";
		ngSpice_Command(const_cast<char*>(TitleLine.c_str()));

	auto it =Application::instance()->getNGSpice().getMapOfCircuits().find("Circuit Simulator");
	 
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


	//for (auto& blockEntityByID : allEntitiesByBlockID)
	//{
	//	//This is just for element Entity (VoltageSource)
	//	//when having resistor we need to cast it to other Entity before get Properties
	//	std::shared_ptr<EntityBlock> blockEntity = blockEntityByID.second;
	//	std::cout << blockEntity->getBlockTitle() << std::endl;
	//	std::cout << blockEntity->getBlockID() << std::endl;
	//	std::string TitleLine = "circbyline *Test";
	//	ngSpice_Command(const_cast<char*>(TitleLine.c_str()));
	//
	//	if (blockEntity->getBlockTitle() == "Circuit Element")
	//	{
	//		std::string myLine = "circbyline ";
	//		auto myElement = dynamic_cast<EntityBlockCircuitElement*>(blockEntity.get());
	//		std::string value = myElement->getElementType();
	//		std::string elementName = "V";
	//		
	//		std::string elementNumber = std::to_string(Numbers::voltageSourceNetlistNumber++);
	//		std::string netlistElement = elementName + elementNumber;
	//		std::string connectionNumbers="";

	//		
	//		


	//		std::string netlistLine = "";
	//		netlistLine = netlistElement + " " + connectionNumbers + " " + value + "\n";

	//		myLine += netlistLine;
	//		ngSpice_Command(const_cast<char*>(myLine.c_str()));
	//	}
	//	else if (blockEntity->getBlockTitle() == "Circuit Element Resistor")
	//	{
	//		auto myElement = dynamic_cast<EntityBlockCircuitResistor*>(blockEntity.get());
	//		std::string myLine="circbyline ";
	//		std::string value = myElement->getElementType();
	//		std::string elementName = "R";
	//		std::string elementNumber = std::to_string(Numbers::resistorNetlistNumber++);
	//		std::string netlistElement = elementName + elementNumber;
	//		std::string connectionNumbers = "";


	//		std::string netlistLine = "";
	//		netlistLine = netlistElement + " " + connectionNumbers + " " + value + "\n";

	//		myLine += netlistLine;
	//		ngSpice_Command(const_cast<char*>(myLine.c_str()));
	//	}
	//	else
	//	{
	//		OT_LOG_E("No EntityBlock found!");
	//	}
	//}
	ngSpice_Command(const_cast<char*>("circbyline .dc V1 0 12 1"));
	ngSpice_Command(const_cast<char*>("circbyline .Control"));
	ngSpice_Command(const_cast<char*>("circbyline run"));
	ngSpice_Command(const_cast<char*>("circbyline print all"));
	ngSpice_Command(const_cast<char*>("circbyline .endc"));
	ngSpice_Command(const_cast<char*>("circbyline .end"));



	return "succes";

	
}

std::string NGSpice::ngSpice_Initialize(std::map<std::string, std::shared_ptr<EntityBlock>>& allEntitiesByBlockID)
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
	 generateNetlist(allEntitiesByBlockID);
	
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