#pragma once

// std header
#include <string>
#include <list>

#include "OTCore/JSON.h"
#include "OTCore/OTClassHelper.h"
#include "OTServiceFoundation/BusinessLogicHandler.h"

class LibraryManagementWrapper : public BusinessLogicHandler {
public:
	LibraryManagementWrapper() {};
	~LibraryManagementWrapper() {};

	std::list<std::string> getCircuitModels();
	std::string getCircuitModel(std::string _modelName);
	std::string requestCreateConfig(const ot::JsonDocument& _doc);
	void createModelTextEntity(const std::string& _modelInfo, const std::string& _folder, const std::string& _elementType, const std::string& _modelName);
	void updatePropertyOfEntity(const ot::UID& _entityID, bool _dialogConfirmed,const std::string& _propertyValue);
private:
	std::string m_lmsLocalUrl = "127.0.0.1:8002";

	ot::UIDList m_entityIDsTopo;
	ot::UIDList m_entityVersionsTopo;
	ot::UIDList m_entityIDsData {};
	ot::UIDList m_entityVersionsData {};
	std::list<bool> m_forceVisible;

	void addModelToEntites();
};