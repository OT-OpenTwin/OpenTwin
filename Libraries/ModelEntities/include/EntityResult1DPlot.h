#pragma once
#pragma warning(disable : 4251)

#include "EntityContainer.h"
#include "OldTreeIcon.h"
#include "OTCore/UIDNamePair.h"
#include "OTGui/Plot1DDataBaseCfg.h"

#include <list>

class __declspec(dllexport) EntityResult1DPlot : public EntityContainer {
public:
	EntityResult1DPlot(ot::UID ID, EntityBase *parent, EntityObserver *mdl, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	virtual ~EntityResult1DPlot();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	virtual bool considerForPropertyFilter(void) override { return true; };
	virtual bool considerChildrenForPropertyFilter(void) override { return false; };

	virtual void StoreToDataBase(void) override;
	virtual void addVisualizationNodes(void) override;
	
	virtual std::string getClassName(void) { return "EntityResult1DPlot"; };

	void addVisualizationItem(bool isHidden);

	virtual entityType getEntityType(void) override { return TOPOLOGY; };

	void createProperties(void);

	virtual bool updateFromProperties(void) override;

	void setTitle(const std::string &title) { setStringPlotProperty("Title", title); }
	std::string getTitle(void) { return getStringPlotProperty("Title"); }

	void setPlotType(const std::string &type);
	std::string getPlotType(void);

	void setPlotQuantity(const std::string &quantity);
	std::string getPlotQuantity(void);

	void setGrid(bool flag) { setBoolPlotProperty("Grid", flag); }
	bool getGrid(void) { return getBoolPlotProperty("Grid"); }

	void setGridColor(int colorR, int colorG, int colorB);
	void getGridColor(int &colorR, int &colorG, int &colorB);

	void setLegend(bool flag) { setBoolPlotProperty("Legend", flag); }
	bool getLegend(void) { return getBoolPlotProperty("Legend"); }

	void setLogscaleX(bool flag) { setBoolPlotProperty("Logscale X", flag); }
	bool getLogscaleX(void) { return getBoolPlotProperty("Logscale X"); }

	void setLogscaleY(bool flag) { setBoolPlotProperty("Logscale Y", flag); }
	bool getLogscaleY(void) { return getBoolPlotProperty("Logscale Y"); }

	void setAutoscaleX(bool flag) { setBoolPlotProperty("Autoscale X", flag); }
	bool getAutoscaleX(void) { return getBoolPlotProperty("Autoscale X"); }

	void setAutoscaleY(bool flag) { setBoolPlotProperty("Autoscale Y", flag); }
	bool getAutoscaleY(void) { return getBoolPlotProperty("Autoscale Y"); }

	void setXmin(double value) { setDoublePlotProperty("X min", value); }
	void setXmax(double value) { setDoublePlotProperty("X max", value); }

	void setYmin(double value) { setDoublePlotProperty("Y min", value); }
	void setYmax(double value) { setDoublePlotProperty("Y max", value); }

	double getXmin(void) { return getDoublePlotProperty("X min"); }
	double getXmax(void) { return getDoublePlotProperty("X max"); }

	double getYmin(void) { return getDoublePlotProperty("Y min"); }
	double getYmax(void) { return getDoublePlotProperty("Y max"); }
	
	bool updatePropertyVisibilities(void);

	void addCurve(ot::UID _curveID, const std::string& _name);
	bool deleteCurve(ot::UID _curveID);
	bool deleteCurve(const std::string& _curveName);
	const ot::UIDNamePairList& getCurves(void) const { return m_curves; };
	ot::UIDList getCurveIDs(void) const;
	std::list<std::string> getCurveNames(void) const;
	void overrideReferencedCurves(const ot::UIDNamePairList& _curves);
	
private:
	virtual int getSchemaVersion(void) { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;
	void setStringPlotProperty(const std::string &name, const std::string &value);
	void setSelectionPlotProperty(const std::string &name, const std::string &value);
	void setBoolPlotProperty(const std::string &name, bool value);
	void setDoublePlotProperty(const std::string &name, double value);
	std::string getStringPlotProperty(const std::string &name);
	std::string getSelectionPlotProperty(const std::string &name);
	bool getBoolPlotProperty(const std::string &name);
	double getDoublePlotProperty(const std::string &name);

	void addBasicsToConfig(ot::Plot1DCfg& _config);

	ot::UIDNamePairList m_curves;
};



