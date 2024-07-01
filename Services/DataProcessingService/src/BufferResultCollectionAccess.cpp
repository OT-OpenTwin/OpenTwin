#include "BufferResultCollectionAccess.h"
#include "ProjectToCollectionConverter.h"
#include "Application.h"
#include "DataBase.h"

void BufferResultCollectionAccess::setModelComponent(ot::components::ModelComponent* modelComponent)
{
	_modelComponent = modelComponent;
}

#include "OTServiceFoundation/ExternalServicesComponent.h"
const std::shared_ptr<ResultMetadataAccess> BufferResultCollectionAccess::getResultCollectionAccessMetadata(EntityBlockDatabaseAccess* blockEntity)
{
	const std::string projectName = blockEntity->getSelectedProjectName();
	auto existingCampaignData = _resultCollectionAccessByProjectName.find(projectName);
	if ( existingCampaignData == _resultCollectionAccessByProjectName.end())
	{
		auto& classFactory = Application::instance()->getClassFactory();
		const std::string sessionServiceURL = Application::instance()->sessionServiceURL();
		ProjectToCollectionConverter collectionFinder(sessionServiceURL);
		std::string loggedInUserName = Application::instance()->getLogInUserName();
		std::string loggedInUserPsw = Application::instance()->getLogInUserPsw();
		std::string collectionName = collectionFinder.NameCorrespondingCollection(projectName, loggedInUserName, loggedInUserPsw);

		const std::string thisProjectsName = DataBase::GetDataBase()->getProjectName();
		
		std::shared_ptr<ResultMetadataAccess> newResultCollectionAccess;
		if (thisProjectsName == projectName)
		{
			newResultCollectionAccess.reset(new ResultMetadataAccess(collectionName, _modelComponent, &classFactory));
		}
		else //Crosscollection access
		{
			newResultCollectionAccess.reset(new ResultMetadataAccess(collectionName, _modelComponent, &classFactory, sessionServiceURL));
		}
		const auto temp = std::make_pair<>(projectName,newResultCollectionAccess);
		_resultCollectionAccessByProjectName.insert(temp);
		existingCampaignData = _resultCollectionAccessByProjectName.find(projectName);
	}
	return existingCampaignData->second;
}