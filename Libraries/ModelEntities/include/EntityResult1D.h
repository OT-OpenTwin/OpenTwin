#pragma once
#pragma warning(disable : 4251)

#include "EntityContainer.h"
#include "Types.h"

class EntityResult1DData;

class __declspec(dllexport) EntityResult1D : public EntityContainer
{
public:
	enum tDataType {EMPTY, REAL, IMAG, COMPLEX};

	EntityResult1D(ot::UID ID, EntityBase *parent, EntityObserver *mdl, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	virtual ~EntityResult1D();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	virtual bool considerForPropertyFilter(void) override { return true; };
	virtual bool considerChildrenForPropertyFilter(void) override { return false; };

	virtual void StoreToDataBase(void) override;
	virtual void addVisualizationNodes(void) override;
	
	virtual std::string getClassName(void) { return "EntityResult1D"; };

	virtual entityType getEntityType(void) override { return DATA; };
	virtual void removeChild(EntityBase *child) override;

	void createProperties(void);

	virtual bool updateFromProperties(void) override;

	void deleteCurveData(void);

	void storeCurveData(void);
	void releaseCurveData(void);

	long long getCurveDataStorageId(void) { return curveDataStorageId; }
	long long getCurveDataStorageVersion(void) { return curveDataStorageVersion; }

	void setColor(int colorR, int colorG, int colorB);
	void setXLabel(const std::string &label) { setCurveProperty("X axis label", label); }
	void setYLabel(const std::string &label) { setCurveProperty("Y axis label", label); }
	void setXUnit(const std::string &unit) { setCurveProperty("X axis unit", unit); }
	void setYUnit(const std::string &unit) { setCurveProperty("Y axis unit", unit); }

	void setCurveXData(const std::vector<double> &x);
	void setCurveYData(const std::vector<double> &yre, const std::vector<double> &yim);

	void setColorFromID(int colorID);

	tDataType getDataType(void) { return dataType; }

private:
	void EnsureCurveDataLoaded(void);
	virtual int getSchemaVersion(void) { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
	void setCurveProperty(const std::string &name, const std::string &value);
	void syncSettingsFromProperties(void);
	EntityResult1DData *getCurveData(void);

	EntityResult1DData *curveData;
	long long curveDataStorageId;
	long long curveDataStorageVersion;

	double color[3];
	std::string xAxisLabel;
	std::string yAxisLabel;
	std::string xAxisUnit;
	std::string yAxisUnit;

	tDataType dataType;
};



