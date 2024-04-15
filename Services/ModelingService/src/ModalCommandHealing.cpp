#include "ModalCommandHealing.h"

#include "ShapeHealing.h"
#include "Application.h"

#include "OTServiceFoundation/UiComponent.h"

#include <thread>	
#include <sstream>

bool ModalCommandHealing::healingSmallEdges = true;
bool ModalCommandHealing::healingSmallFaces = true;
bool ModalCommandHealing::healingSewFaces = true;
bool ModalCommandHealing::healingMakeSolid = true;
double ModalCommandHealing::healingTolerance = 1e-6;

ModalCommandHealing::ModalCommandHealing(ot::ApplicationBase *app, const std::string &actionMnuID, const std::string &actionBtnID)
	: ModalCommandBase(app, actionMnuID, actionBtnID)
{
	setupUI();
}

ModalCommandHealing::~ModalCommandHealing()
{
}

std::string ModalCommandHealing::initializeAndCreateUI(ot::LockTypeFlags& modelRead, ot::LockTypeFlags& modelWrite)
{
	addMenuPage("Healing");
	addMenuGroup("Healing", "Settings");
	addMenuGroup("Healing", "Operation");

	addMenuAction("Healing", "Settings", "Reset", "Reset", modelWrite, "ResetSettings");
	
	addMenuSubgroup("Healing", "Settings", "Column1");
	addMenuSubgroup("Healing", "Settings", "Column2");
	addMenuSubgroup("Healing", "Settings", "Column3");

	addMenuAction("Healing", "Operation", "Perform", "Perform", modelWrite, "Execute");
	addMenuAction("Healing", "Operation", "Close", "Close", modelRead, "Cancel");

	addMenuLineEdit("Healing", "Settings", "Column1", "Tolerance", std::to_string(healingTolerance), "Tolerance", modelWrite);
	addMenuCheckBox("Healing", "Settings", "Column2", "SmallEdges", "Fix small edges", healingSmallEdges, modelWrite);
	addMenuCheckBox("Healing", "Settings", "Column2", "SmallFaces", "Fix small faces", healingSmallFaces, modelWrite);
	addMenuCheckBox("Healing", "Settings", "Column3", "SewFaces", "Sew faces", healingSewFaces, modelWrite);
	addMenuCheckBox("Healing", "Settings", "Column3", "MakeSolid", "Make solids", healingMakeSolid, modelWrite);

	application->modelSelectionChanged(); // Update the control states

	return "Healing";  // return the name of the tab which shall be activated
}

bool ModalCommandHealing::executeAction(const std::string &action, rapidjson::Document &doc)
{
	if		(action == "Healing:Operation:Close")				delete this;
	else if (action == "Healing:Operation:Perform")				healSelectedShapes();
	else if (action == "Healing:Settings:Reset")				resetSettings();
	else if (action == "Healing:Settings:Column2:SmallEdges")	healingSmallEdges = !healingSmallEdges;
	else if (action == "Healing:Settings:Column2:SmallFaces")	healingSmallFaces = !healingSmallFaces;
	else if (action == "Healing:Settings:Column3:SewFaces")		healingSewFaces = !healingSewFaces;
	else if (action == "Healing:Settings:Column3:MakeSolid")	healingMakeSolid = !healingMakeSolid;
	else if (action == "Healing:Settings:Column1:Tolerance")	processToleranceSetting(doc);
	else return false;  // The command was not found in this list

	// The command was processed successfully
	return true;
}

void ModalCommandHealing::modelSelectionChanged(std::list< ot::UID> &selectedEntityID)
{
	if (selectedEntityID.size() > 0)
	{
		application->uiComponent()->setControlState("Healing:Operation:Perform", true);
	}
	else
	{
		application->uiComponent()->setControlState("Healing:Operation:Perform", false);
	}
}

void ModalCommandHealing::resetSettings(void)
{
	healingSmallEdges = true;
	healingSmallFaces = true;
	healingSewFaces = true;
	healingMakeSolid = true;
	healingTolerance = 1e-6;

	setMenuLineEdit("Healing", "Settings", "Column1", "Tolerance", std::to_string(healingTolerance), false);
	setMenuCheckBox("Healing", "Settings", "Column2", "SmallEdges", healingSmallEdges);
	setMenuCheckBox("Healing", "Settings", "Column2", "SmallFaces", healingSmallFaces);
	setMenuCheckBox("Healing", "Settings", "Column3", "SewFaces", healingSewFaces);
	setMenuCheckBox("Healing", "Settings", "Column3", "MakeSolid", healingMakeSolid);
}

void ModalCommandHealing::processToleranceSetting(rapidjson::Document &doc)
{
	std::string text = doc[OT_ACTION_PARAM_UI_CONTROL_ObjectText].GetString();

	// Check field for correctness
	std::stringstream stream(text);

	double value{ 0.0 };
	stream >> value;
	stream >> std::ws;

	bool error = false;

	if (!stream.eof())
	{
		// We have an error in the input
		error = true;
	}
	else
	{
		// The result is correct
		healingTolerance = value;
	}

	// Send the notification to either set or reset the error state
	setMenuLineEdit("Healing", "Settings", "Column1", "Tolerance", text, error);
}

void ModalCommandHealing::healSelectedShapes(void)
{
	std::thread workerThread(&ModalCommandHealing::healSelectedShapesWorker, this);
	workerThread.detach();
}

void ModalCommandHealing::healSelectedShapesWorker(void)
{
	Application *app = dynamic_cast<Application*>(application);
	assert(app != nullptr);

	ShapeHealing healer(app);
	healer.healSelectedShapes(healingTolerance, healingSmallEdges, healingSmallFaces, healingSewFaces, healingMakeSolid);
}
