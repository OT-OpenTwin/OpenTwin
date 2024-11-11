#pragma once

namespace osg
{
	class Node;
	class Switch;
}

#include "Geometry.h"
#include "SceneNodeBase.h"
#include "DataBase.h"
#include "Plot.h"

#include <string>
#include <map>

#include <osg/Array>

class Model;

class SceneNodePlot1D : public SceneNodeBase
{
public:
	SceneNodePlot1D();
	virtual ~SceneNodePlot1D();

	virtual void setTransparent(bool t) override;
	virtual void setWireframe(bool w) override;
	virtual void setVisible(bool v) override;
	virtual void setHighlighted(bool h) override;

	void setProjectName(const std::string &proj) { projectName = proj; };
	void setTitle(const std::string &title) { plotTitle = title; };
	void setGrid(bool flag) { plotGrid = flag; }
	void setGridColor(int color[]) { plotGridColorR = color[0]; plotGridColorG = color[1]; plotGridColorB = color[2]; }
	void setLegend(bool flag) { plotLegend = flag; }
	void setLogscaleX(bool flag) { plotLogscaleX = flag; }
	void setLogscaleY(bool flag) { plotLogscaleY = flag; }
	void setAutoscaleX(bool flag) { plotAutoscaleX = flag; }
	void setAutoscaleY(bool flag) { plotAutoscaleY = flag; }
	void setXmin(double value) { plotXmin = value; }
	void setXmax(double value) { plotXmax = value; }
	void setYmin(double value) { plotYmin = value; }
	void setYmax(double value) { plotYmax = value; }
	void setPlotType(AbstractPlot::PlotType value) { plotType = value; }
	void setPlotQuantity(PlotDataset::axisQuantity value) { plotQuantity = value; }

	void setCurves(std::list<unsigned long long> &curvesID, std::list<unsigned long long> &curvesVersions, std::list<std::string> &curvesNames);

	std::string getProjectName(void) { return projectName; };
	std::string getTitle(void) { return plotTitle; };

	bool getGrid(void) { return plotGrid; }
	int getGridColorR(void) { return plotGridColorR; }
	int getGridColorG(void) { return plotGridColorG; }
	int getGridColorB(void) { return plotGridColorB; }
	bool getLegend(void) { return plotLegend; }
	bool getLogscaleX(void) { return plotLogscaleX; }
	bool getLogscaleY(void) { return plotLogscaleY; }
	bool getAutoscaleX(void) { return plotAutoscaleX; }
	bool getAutoscaleY(void) { return plotAutoscaleY; }
	double getXmin(void) { return plotXmin; }
	double getXmax(void) { return plotXmax; }
	double getYmin(void) { return plotYmin; }
	double getYmax(void) { return plotYmax; }
	AbstractPlot::PlotType getPlotType(void) { return plotType; }
	PlotDataset::axisQuantity getPlotQuantity(void) { return plotQuantity; }

	bool getSelectChildren(void) override { return false; };

	Model *getModel(void) { return model; };
	void setModel(Model *m) { model = m; };

	void addCurveNodes(void);

	unsigned long long getNumberOfCurves(void) { return curveID.size(); };
	unsigned long long getCurveID(unsigned long long index) { return curveID[index]; };
	unsigned long long getCurveVersion(unsigned long long index) { return curveVersion[index]; };
	std::string getCurveName(unsigned long long index) { return curveName[index]; };

	virtual bool isItem1D(void) { return true; };
	virtual bool isItem3D(void) { return false; };

	bool changeResult1DEntityVersion(unsigned long long _curveEntityID, unsigned long long _curveVersion);

private:
	bool isAnyChildSelectedAndVisible(SceneNodeBase *root);

	std::string projectName;
	AbstractPlot::PlotType plotType;
	PlotDataset::axisQuantity plotQuantity;
	std::string plotTitle;
	bool plotGrid;
	int plotGridColorR;
	int plotGridColorG;
	int plotGridColorB;
	bool plotLegend;
	bool plotLogscaleX;
	bool plotLogscaleY;
	bool plotAutoscaleX;
	bool plotAutoscaleY;
	double plotXmin;
	double plotXmax;
	double plotYmin;
	double plotYmax;
	unsigned long long modelEntityVersion;
	std::vector<unsigned long long> curveID;
	std::vector<unsigned long long> curveVersion;
	std::vector<std::string> curveName;


	Model *model;
};

