#pragma once

#include "OTServiceFoundation/ModalCommandBase.h"

class ModalCommandHealing : public ot::ModalCommandBase
{
public:
	ModalCommandHealing(ot::ApplicationBase *app, const std::string &actionMnuID, const std::string &actionBtnID);
	virtual ~ModalCommandHealing();

	virtual bool executeAction(const std::string &action, rapidjson::Document &doc);
	virtual void modelSelectionChanged(std::list< ot::UID> &selectedEntityID);

protected:
	virtual std::string initializeAndCreateUI(ot::LockTypeFlags& modelRead, ot::LockTypeFlags& modelWrite) override;

private:
	void processToleranceSetting(rapidjson::Document &doc);
	void healSelectedShapes(void);
	void healSelectedShapesWorker(void);
	void resetSettings(void);

	static bool healingSmallEdges;
	static bool healingSmallFaces;
	static bool healingSewFaces;
	static bool healingMakeSolid;
	static double healingTolerance;
};
