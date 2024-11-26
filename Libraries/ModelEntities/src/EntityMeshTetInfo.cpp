
#include "EntityMeshTetInfo.h"
#include "DataBase.h"
#include "OldTreeIcon.h"

#include <bsoncxx/builder/basic/array.hpp>

EntityMeshTetInfo::EntityMeshTetInfo(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner) :
	EntityBase(ID, parent, obs, ms, factory, owner)
{
	
}

EntityMeshTetInfo::~EntityMeshTetInfo()
{

}

bool EntityMeshTetInfo::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

void EntityMeshTetInfo::AddStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::AddStorageData(storage);

	// Now we store the specific data of this item (list of general, faces and volumes data)
	auto generalID = bsoncxx::builder::basic::array();
	auto generalVersion = bsoncxx::builder::basic::array();
	auto facesID = bsoncxx::builder::basic::array();
	auto facesVersion = bsoncxx::builder::basic::array();
	auto volumesID = bsoncxx::builder::basic::array();
	auto volumesVersion = bsoncxx::builder::basic::array();

	for (auto item : generalData)
	{
		generalID.append((long long)item.first);
		generalVersion.append((long long)item.second);
	}

	for (auto item : facesData)
	{
		facesID.append((long long)item.first);
		facesVersion.append((long long)item.second);
	}

	for (auto item : volumeData)
	{
		volumesID.append((long long)item.first);
		volumesVersion.append((long long)item.second);
	}

	storage.append(bsoncxx::builder::basic::kvp("GeneralID", generalID));
	storage.append(bsoncxx::builder::basic::kvp("GeneralVersion", generalVersion));
	storage.append(bsoncxx::builder::basic::kvp("FacesID", facesID));
	storage.append(bsoncxx::builder::basic::kvp("FacesVersion", facesVersion));
	storage.append(bsoncxx::builder::basic::kvp("VolumesID", volumesID));
	storage.append(bsoncxx::builder::basic::kvp("VolumesVersion", volumesVersion));
}

void EntityMeshTetInfo::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now we read the information about the list data
	auto generalID = doc_view["GeneralID"].get_array().value;
	auto generalVersion = doc_view["GeneralVersion"].get_array().value;
	auto facesID = doc_view["FacesID"].get_array().value;
	auto facesVersion = doc_view["FacesVersion"].get_array().value;
	auto volumesID = doc_view["VolumesID"].get_array().value;
	auto volumesVersion = doc_view["VolumesVersion"].get_array().value;

	size_t numberGeneral = std::distance(generalID.begin(), generalID.end());
	assert(numberGeneral == std::distance(generalVersion.begin(), generalVersion.end()));

	size_t numberFaces = std::distance(facesID.begin(), facesID.end());
	assert(numberFaces == std::distance(facesVersion.begin(), facesVersion.end()));

	size_t numberVolumes = std::distance(volumesID.begin(), volumesID.end());
	assert(numberVolumes == std::distance(volumesVersion.begin(), volumesVersion.end()));

	generalData.clear();
	facesData.clear();
	volumeData.clear();

	auto idGeneralID = generalID.begin();
	auto idGeneralVersion = generalVersion.begin();
	auto idFacesID = facesID.begin();
	auto idFacesVersion = facesVersion.begin();
	auto idVolumeID = volumesID.begin();
	auto idVolumeVersion = volumesVersion.begin();

	for (size_t index = 0; index < numberGeneral; index++)
	{
		generalData.push_back(std::pair<ot::UID, ot::UID>(idGeneralID->get_int64(), idGeneralVersion->get_int64()));
		idGeneralID++;
		idGeneralVersion++;
	}

	for (size_t index = 0; index < numberFaces; index++)
	{
		facesData.push_back(std::pair<ot::UID, ot::UID>(idFacesID->get_int64(), idFacesVersion->get_int64()));
		idFacesID++;
		idFacesVersion++;
	}

	for (size_t index = 0; index < numberVolumes; index++)
	{
		volumeData.push_back(std::pair<ot::UID, ot::UID>(idVolumeID->get_int64(), idVolumeVersion->get_int64()));
		idVolumeID++;
		idVolumeVersion++;
	}

	resetModified();
}
