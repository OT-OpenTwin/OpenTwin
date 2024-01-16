//Service Header
#include "NGSpice.h"
#include "Application.h"
#include "Connection.h"
//Open Twin Header
#include "EntityBlockCircuitElement.h"
#include "EntityBlockCircuitResistor.h"
//Third Party Header
#include <string>
namespace Numbers
{
	static unsigned long long currentNodeNumber = 0;
}

std::vector<Connection> ElementConnections;
std::vector<Connection> ResistorConnections;

std::string NGSpice::generateNetlist(std::map<std::string, std::shared_ptr<EntityBlock>>& allEntitiesByBlockID)
{

	//1st Approach: One could sort based on the size of the node numbers, meaning that all nodes within a NetlistString would be sorted. 
	//For example, instead of "2 1," it would be "1 2," ensuring that node 1 comes before node 2. However, for positive results concerning voltage sources, 
	//the order would need to be reversed.

	//2nd Approach: Another option is to sort the list of connections for each element in ascending order of node numbers using a function. 
	//This way, the connections are already sorted when retrieved from the list.


	// First I declare the path to the file

	//std::ofstream outfile("C:/Users/Sebastian/Desktop/NGSpice_Dateien_Test/output.cir");
	//Here i have two Vectors for incomging and putgoing Connections


	/*std::string Title = "*Test";
	outfile << Title << std::endl;

	int DiodeCounter = 0;
	int TransistorCounter = 0;*/

	// Note: The connector is not yet added to the netlist, and the transistor values along with the model are missing.
	// Additionally, appropriate simulation settings for the transistor are still pending.

	


	// Now I write the informations to the File
	// Here i get the Circuit and the map of Elements
	for (auto& blockEntityByID : allEntitiesByBlockID)
	{
		//This is just for element Entity (VoltageSource)
		//when having resistor we need to cast it to other Entity before get Properties
		std::shared_ptr<EntityBlock> blockEntity = blockEntityByID.second;
		std::cout << blockEntity->getBlockTitle() << std::endl;
		std::cout << blockEntity->getBlockID() << std::endl;
		std::string TitleLine = "circbyline *Test";
		ngSpice_Command(const_cast<char*>(TitleLine.c_str()));
	
		if (blockEntity->getBlockTitle() == "Circuit Element")
		{
			std::string myLine = "circbyline ";
			auto myElement = dynamic_cast<EntityBlockCircuitElement*>(blockEntity.get());
			std::string value = myElement->getElementType();
			std::string elementName = "V";
			unsigned long long number = 1;
			std::string elementNumber = std::to_string(number);
			std::string netlistElement = elementName + elementNumber;
			std::string connectionNumbers;

			auto list = myElement->getAllConnections();
			for (auto it : list)
			{
				Connection connection(it);
				connection.setNodeNumber(std::to_string(Numbers::currentNodeNumber++));
				connectionNumbers += connection.getNodeNumber();
				connectionNumbers += " ";
				//ElementConnections.push_back(connection);
			}

			std::string netlistLine = "";
			netlistLine = netlistElement + " " + connectionNumbers + " " + value + "\n";

			myLine += netlistLine;
			ngSpice_Command(const_cast<char*>(myLine.c_str()));
		}
		else if (blockEntity->getBlockTitle() == "Circuit Element Resistor")
		{
			auto myElement = dynamic_cast<EntityBlockCircuitResistor*>(blockEntity.get());
			std::string myLine="circbyline ";
			std::string value = myElement->getElementType();
			std::string elementName = "R";
			unsigned long long number = 1;
			std::string elementNumber = std::to_string(number);
			std::string netlistElement = elementName + elementNumber;

			std::string connectionNumbers;

			auto list = myElement->getAllConnections();
			for (auto it : list)
			{
				Connection connection(it);
				connection.setNodeNumber(std::to_string(--Numbers::currentNodeNumber));
				connectionNumbers += connection.getNodeNumber();
				connectionNumbers += " ";
				//ResistorConnections.push_back(connection);
			}

			std::string netlistLine = "";
			netlistLine = netlistElement + " " + connectionNumbers + " " + value + "\n";

			myLine += netlistLine;
			ngSpice_Command(const_cast<char*>(myLine.c_str()));
		}
		else
		{
			OT_LOG_E("No EntityBlock found!");
		}
	}
	ngSpice_Command(const_cast<char*>("circbyline .dc V1 0 12 1"));
	ngSpice_Command(const_cast<char*>("circbyline .Control"));
	ngSpice_Command(const_cast<char*>("circbyline run"));
	ngSpice_Command(const_cast<char*>("circbyline print all"));
	ngSpice_Command(const_cast<char*>("circbyline .endc"));
	ngSpice_Command(const_cast<char*>("circbyline .end"));

	
	
	//ngSpice_Command(const_cast<char*>(netlist.c_str()));

	return "succes";

	/*auto map = Application::instance()->getNGSpice().mapOfCircuits.find("Circuit Simulator")->second.getMapOfElements();*/
	
	// Now i iterate through this Map of Elements and save the information
	//for (auto it : map)
	//{


	//	std::string NodeNumbersString = "";
	//	std::string elementName_temp = it.second.getItemName();
	//	std::string netlistElementName = it.second.getNetlistElementName();
	//	std::string value;
	//	std::string elmentUID = it.second.getUID();

	//	if (elementName_temp == "Connector")
	//	{
	//		continue;
	//	}

	//	//Here i get the Connection List
	//	auto connectionList = it.second.getList();

	//	if (elementName_temp == "EntityBlockCircuitElement")
	//	{
	//		for (auto c = connectionList.rbegin(); c != connectionList.rend(); c++)
	//		{

	//			NodeNumbersString += c->getNodeNumber();
	//			NodeNumbersString += " ";
	//		}
	//	}

	//	else
	//	{

	//		for (auto c : connectionList)
	//		{
	//			NodeNumbersString += c.getNodeNumber();
	//			NodeNumbersString += " ";

	//		}

	//	}

	//	//I set default values
	//	if (elementName_temp == "VoltageSource")
	//	{
	//		value = "12V";


	//	}
	//	else if (elementName_temp == "Resistor")
	//	{
	//		value = "200";
	//	}
	//	else if (elementName_temp == "Diode")
	//	{
	//		value = "myDiode";
	//		DiodeCounter++;
	//	}
	//	else
	//	{
	//		value = "BC546B";
	//		TransistorCounter++;
	//	}

	//	//Create the end string 
	//	std::string NetlistLine = netlistElementName + " " + NodeNumbersString + value;

	//	outfile << NetlistLine << std::endl;
	//}

	//if (DiodeCounter > 0)
	//{
	//	std::string diodeModel = ".model myDiode D (IS=1n RS=0.1 N=1)";
	//	outfile << diodeModel << std::endl;
	//}

	//if (TransistorCounter > 0)
	//{
	//	std::string tranModel = ".model BC546B npn (BF=200)";
	//	outfile << tranModel << std::endl;
	//}

	//std::string Properties = ".dc V1 0V 12V 1V";
	//outfile << Properties << std::endl;

	//std::string control = ".Control";
	//outfile << control << std::endl;;

	//std::string run = "run";
	//outfile << run << std::endl;

	//std::string print = "print all";
	//outfile << print << std::endl;

	//std::string endc = ".endc";
	//outfile << endc << std::endl;;

	//std::string end = ".end";
	//outfile << end << std::endl;


	//std::string myString = "Succesfull";
	//return myString;
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


	// Some simulation

	 generateNetlist(allEntitiesByBlockID);
	

	

	//const char* netlistPath = "C:\\Users\\Sebastian\\Desktop\\NGSpice_Dateien_Test\\SpannungsTeiler2.cir";

	//char command[100];
	//sprintf_s(command, "source %s", netlistPath);
	//ngSpice_Command(command);



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