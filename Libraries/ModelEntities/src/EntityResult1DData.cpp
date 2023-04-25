
#include "EntityResult1DData.h"
#include "DataBase.h"
#include "Types.h"

#include <bsoncxx/builder/basic/array.hpp>

EntityResult1DData::EntityResult1DData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactory *factory, const std::string &owner) :
	EntityBase(ID, parent, obs, ms,  factory, owner)
{
	
}

EntityResult1DData::~EntityResult1DData()
{
	dataX.clear();
	dataYre.clear();
	dataYim.clear();
}

bool EntityResult1DData::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

void EntityResult1DData::StoreToDataBase(void)
{
	EntityBase::StoreToDataBase();
}

void EntityResult1DData::AddStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityBase::AddStorageData(storage);

	// Now add the actual 1D curve data

	auto arrayX = bsoncxx::builder::basic::array();
	auto arrayYre = bsoncxx::builder::basic::array();
	auto arrayYim = bsoncxx::builder::basic::array();

	for (double x : dataX)
	{
		arrayX.append(x);
	}

	for (double y : dataYre)
	{
		arrayYre.append(y);
	}

	for (double y : dataYim)
	{
		arrayYim.append(y);
	}

	storage.append(
		bsoncxx::builder::basic::kvp("dataX", arrayX),
		bsoncxx::builder::basic::kvp("dataY", arrayYre),
		bsoncxx::builder::basic::kvp("dataYim", arrayYim)
	);
}

void EntityResult1DData::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	// Now read the curve data

	dataX.clear();
	dataYre.clear();
	dataYim.clear();

	// Now load the data from the storage

	auto arrayX = doc_view["dataX"].get_array().value;
	auto arrayYre = doc_view["dataY"].get_array().value;
	auto arrayYim = doc_view["dataYim"].get_array().value;

	size_t nX = std::distance(arrayX.begin(), arrayX.end());
	size_t nYre = std::distance(arrayYre.begin(), arrayYre.end());
	size_t nYim = std::distance(arrayYim.begin(), arrayYim.end());

	auto iX = arrayX.begin();
	auto iYre = arrayYre.begin();
	auto iYim = arrayYim.begin();

	dataX.reserve(nX);
	dataYre.reserve(nYre);
	dataYim.reserve(nYim);

	for (unsigned long index = 0; index < nX; index++)
	{
		dataX.push_back(iX->get_double());
		iX++;
	}

	for (unsigned long index = 0; index < nYre; index++)
	{
		dataYre.push_back(iYre->get_double());
		iYre++;
	}

	for (unsigned long index = 0; index < nYim; index++)
	{
		dataYim.push_back(iYim->get_double());
		iYim++;
	}


	resetModified();
}

void EntityResult1DData::removeChild(EntityBase *child)
{
	EntityBase::removeChild(child);
}

void EntityResult1DData::setXData(const std::vector<double> &x)
{
	dataX = x;
}

void EntityResult1DData::setYData(const std::vector<double> &yre, const std::vector<double> &yim)
{
	dataYre = yre;
	dataYim = yim;
}

void EntityResult1DData::clearData(void)
{
	clearXData();
	clearYData();
}

void EntityResult1DData::clearXData(void)
{
	dataX.clear();
}

void EntityResult1DData::clearYData(void)
{
	dataYre.clear();
	dataYim.clear();
}

const std::vector<double> &EntityResult1DData::getXData(void)
{
	return dataX;
}

const std::vector<double> &EntityResult1DData::getYDataReal(void)
{
	return dataYre;
}

const std::vector<double> &EntityResult1DData::getYDataImag(void)
{
	return dataYim;
}

