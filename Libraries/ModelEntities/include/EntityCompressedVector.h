#pragma once
#pragma warning(disable : 4251)

#include "EntityBase.h"
#include "OldTreeIcon.h"

#include <list>
#include <vector>

class __declspec(dllexport) EntityCompressedVector : public EntityBase
{
public:
	EntityCompressedVector(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	virtual ~EntityCompressedVector();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	virtual void StoreToDataBase(void) override;

	virtual std::string getClassName(void) { return "EntityCompressedVector"; };

	virtual entityType getEntityType(void) override { return DATA; };
	virtual void removeChild(EntityBase *child) override;

	void setTolerance(double tol);
	double getTolerance(void);
	
	void setConstantValue(double value, long long dimension);

	void setValues(const std::vector<double> &values);
	void getValues(std::vector<double> &values);

	void setValues(const double *values, size_t length);
	void getValues(double *values, size_t length);

	long long getUncompressedLength(void) { return uncompressedLength; }

	void clearData(void);

	bool multiply(std::vector<double> &values);
	bool add(std::vector<double> &values);

protected:
	virtual int getSchemaVersion(void) { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

private:
	void compressData(const double *values, size_t length, bool storeData, long long &compressedDataValuesSize, long long &compressedDataCountSize);

	const size_t			maximumDataSize;
	double					tolerance;
	long long				uncompressedLength;
	std::vector<double>		compressedDataValues;
	std::vector<long long>	compressedDataCount;
	long long				valuesSize;
	long long				countSize;
};


