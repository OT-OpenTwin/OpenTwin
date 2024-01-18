
#include "EntityResult1DCurveData.h"
#include "DataBase.h"
#include "Types.h"

#include <bsoncxx/builder/basic/array.hpp>

EntityResult1DCurveData::EntityResult1DCurveData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner) :
	EntityBase(ID, parent, obs, ms,  factory, owner)
{
	
}

EntityResult1DCurveData::~EntityResult1DCurveData()
{
	dataX.clear();
	dataYre.clear();
	dataYim.clear();
}

bool EntityResult1DCurveData::getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax)
{
	return false;
}

void EntityResult1DCurveData::StoreToDataBase(void)
{
	EntityBase::StoreToDataBase();
}

void EntityResult1DCurveData::AddStorageData(bsoncxx::builder::basic::document &storage)
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

void EntityResult1DCurveData::readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
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

void EntityResult1DCurveData::removeChild(EntityBase *child)
{
	EntityBase::removeChild(child);
}

void EntityResult1DCurveData::setXData(const std::vector<double> &x)
{
	dataX = x;
}

void EntityResult1DCurveData::setYData(const std::vector<double> &yre, const std::vector<double> &yim)
{
	dataYre = yre;
	dataYim = yim;
}

void EntityResult1DCurveData::clearData(void)
{
	clearXData();
	clearYData();
}

void EntityResult1DCurveData::clearXData(void)
{
	dataX.clear();
}

void EntityResult1DCurveData::clearYData(void)
{
	dataYre.clear();
	dataYim.clear();
}

const std::vector<double> &EntityResult1DCurveData::getXData(void)
{
	return dataX;
}

const std::vector<double> &EntityResult1DCurveData::getYDataReal(void)
{
	return dataYre;
}

const std::vector<double> &EntityResult1DCurveData::getYDataImag(void)
{
	return dataYim;
}

