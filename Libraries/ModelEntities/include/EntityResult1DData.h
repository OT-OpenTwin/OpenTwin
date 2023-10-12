#pragma once
#pragma warning(disable : 4251)

#include "EntityBase.h"
#include "Types.h"

#include <list>
#include <vector>

class __declspec(dllexport) EntityResult1DData : public EntityBase
{
public:
	EntityResult1DData(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	virtual ~EntityResult1DData();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	virtual void StoreToDataBase(void) override;

	virtual std::string getClassName(void) { return "EntityResult1DData"; };

	virtual entityType getEntityType(void) override { return DATA; };
	virtual void removeChild(EntityBase *child) override;

	void setXData(const std::vector<double> &x);
	void setYData(const std::vector<double> &yre, const std::vector<double> &yim);

	void clearData(void);
	void clearXData(void);
	void clearYData(void);

	const std::vector<double> &getXData(void);
	const std::vector<double> &getYDataReal(void);
	const std::vector<double> &getYDataImag(void);

private:
	virtual int getSchemaVersion(void) { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
	
	std::vector<double> dataX, dataYre, dataYim;
};



