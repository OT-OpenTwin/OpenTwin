#pragma once

#include "OTGuiAPI/ButtonHandler.h"
#include "OTGuiAPI/LineEditHandler.h"
#include "OTServiceFoundation/ModalCommandBase.h"

class ModalCommandHealing : public ot::ModalCommandBase, public ot::ButtonHandler, public ot::LineEditHandler
{
public:
	ModalCommandHealing(ot::ApplicationBase *app, const std::string &actionMnuID, const std::string &actionBtnID);
	virtual ~ModalCommandHealing();

	virtual bool executeAction(const std::string &action, rapidjson::Document &doc);
	virtual void modelSelectionChanged(std::list< ot::UID> &selectedEntityID);

protected:
	virtual std::string initializeAndCreateUI(const ot::LockTypes& modelRead, const ot::LockTypes& modelWrite) override;

private:
	void handleDelete();
	void handleToggleSmallEdges();
	void handleToggleSmallFaces();
	void handleToggleSewFaces();
	void handleToggleMakeSolid();

	void processToleranceSetting(const std::string& _newText);
	void healSelectedShapes(void);
	void healSelectedShapesWorker(void);
	void resetSettings(void);

	static bool healingSmallEdges;
	static bool healingSmallFaces;
	static bool healingSewFaces;
	static bool healingMakeSolid;
	static double healingTolerance;
};
