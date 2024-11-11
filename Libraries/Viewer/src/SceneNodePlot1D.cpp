#include "stdafx.h"

#include "SceneNodePlot1D.h"

#include "DataBase.h"
#include "Model.h"

#include <osg/StateSet>
#include <osg/Node>
#include <osg/BlendFunc>
#include <osg/CullFace>
#include <osg/Switch>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/PolygonOffset>
#include <osg/PolygonMode>
#include <osg/LineWidth>
#include <osg/LightModel>

SceneNodePlot1D::SceneNodePlot1D() :
	plotGrid(false),
	plotGridColorR(0),
	plotGridColorG(0),
	plotGridColorB(0),
	plotLegend(false),
	plotLogscaleX(false),
	plotLogscaleY(false),
	plotAutoscaleX(false),
	plotAutoscaleY(false),
	plotXmin(0.0),
	plotXmax(0.0),
	plotYmin(0.0),
	plotYmax(0.0),
	modelEntityVersion(0),
	model(nullptr)
{

}

SceneNodePlot1D::~SceneNodePlot1D()
{
	// Remove the OSG objects 
	// This geometry node will always have a parent group or switch node

	// loop through all parent nodes
	if (getShapeNode() != nullptr)
	{
		unsigned int numParents = getShapeNode()->getNumParents();

		for (unsigned int i = 0; i < numParents; i++)
		{
			osg::Group *parent = getShapeNode()->getParent(i);

			// Remove the child node from the parent (the node itself will then be deleted by reference counting automatically)
			parent->removeChild(getShapeNode());
		}

		// Now the shape node is invalid, since it might have been deleted by removing it from its parent
		shapeNode = nullptr;
	}
}

void SceneNodePlot1D::setTransparent(bool t)
{
	if (t == isTransparent()) return;

	SceneNodeBase::setTransparent(t);
}

void SceneNodePlot1D::setWireframe(bool w)
{
	if (w == isWireframe()) return;

	SceneNodeBase::setWireframe(w);
}

void SceneNodePlot1D::setVisible(bool v)
{
	if (v == isVisible()) return;

	SceneNodeBase::setVisible(v);
}

void SceneNodePlot1D::setHighlighted(bool h)
{
}

void SceneNodePlot1D::setCurves(std::list<unsigned long long> &curvesID, std::list<unsigned long long> &curvesVersions, std::list<std::string> &curvesNames)
{
	curveID.clear();
	curveVersion.clear();
	curveName.clear();

	unsigned long long numberOfCurves = curvesID.size();
	assert(curvesVersions.size() == numberOfCurves);
	assert(curvesNames.size() == numberOfCurves);

	curveID.reserve(numberOfCurves);
	curveVersion.reserve(numberOfCurves);
	curveName.reserve(numberOfCurves);

	for (auto id : curvesID)
	{
		curveID.push_back(id);
	}

	for (auto version : curvesVersions)
	{
		curveVersion.push_back(version);
	}

	for (auto name : curvesNames)
	{
		curveName.push_back(name);
	}
}

void SceneNodePlot1D::addCurveNodes(void)
{


}

bool SceneNodePlot1D::changeResult1DEntityVersion(unsigned long long _curveEntityID, unsigned long long _curveVersion)
{
	bool changed = false;

	for (unsigned long long index = 0; index < curveID.size(); index++)
	{
		if (curveID[index] == _curveEntityID)
		{
			changed |= (curveVersion[index] != _curveVersion);
			curveVersion[index] = _curveVersion;
		}
	}

	bool plotSelectedAndVisible = isAnyChildSelectedAndVisible(this);

	return (changed && plotSelectedAndVisible);
}

bool SceneNodePlot1D::isAnyChildSelectedAndVisible(SceneNodeBase *root)
{
	if (root->isSelected() && root->isVisible()) return true;

	bool selected = false;

	for (auto child : root->getChildren())
	{
		selected |= isAnyChildSelectedAndVisible(child);
		if (selected) break;
	}

	return selected;
}
