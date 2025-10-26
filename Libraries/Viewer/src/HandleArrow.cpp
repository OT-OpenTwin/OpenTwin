#include "stdafx.h"
#include "HandleArrow.h"
#include "Viewer.h"

#include <osg/Switch>
#include <osg/ShapeDrawable>
#include <osg/MatrixTransform>
#include <osg/PolygonMode>
#include <osg/LightModel>
#include <osg/Material>

#include <cassert>

HandleArrow::HandleArrow(osg::Vec3d point, osg::Vec3d dir, double color[4], double length, double radius)
{
	// Create the osg node
	osgNode = new osg::MatrixTransform;

	position = point;
	direction = dir;

	updateTransform();

	osgSwitchNode = new osg::Switch;
	osgNode->addChild(osgSwitchNode);

	arrowNode = new osg::Geode;
	createArrow(arrowNode, color, length, radius);

	arrowNodeSelected = new osg::Geode;
	createArrow(arrowNodeSelected, color, length, 1.3 * radius);

	std::list<osg::Vec3d> pointList;
	intersectionLineLength = 100.0 * length;
	pointList.push_back(osg::Vec3d(0.0, 0.0, -0.5 * intersectionLineLength));
	pointList.push_back(osg::Vec3d(0.0, 0.0, 0.5 * intersectionLineLength));
	createSelectLine(osgSwitchNode, pointList);

	osgSwitchNode->addChild(arrowNode);
	osgSwitchNode->addChild(arrowNodeSelected);

	osgSwitchNode->setChildValue(arrowNodeSelected, false);

	referenceOffset = 0.0;
}

HandleArrow::~HandleArrow()
{

}

osg::Node *HandleArrow::getNode(void)
{
	return osgNode;
}

void HandleArrow::createArrow(osg::Geode * _geode, double color[4], double length, double radius) 
{
	osg::ref_ptr<osg::ShapeDrawable> arrow = new osg::ShapeDrawable;
	arrow->setShape(new osg::Cylinder(osg::Vec3(0.0, 0.0, 0.35 * length), radius, 0.7 * length));

	osg::StateSet * ssArrow = arrow->getOrCreateStateSet();

	ssArrow->setMode(GL_CULL_FACE, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	ssArrow->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL));

	osg::LightModel * lightModel = new osg::LightModel;
	lightModel->setTwoSided(true);
	ssArrow->setAttributeAndModes(lightModel);

	arrow->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

	// Set color
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(1);
	colors->at(0).set(color[0], color[1], color[2], color[3]);
	arrow->setColorArray(colors);
	arrow->setColorBinding(osg::Geometry::BIND_OVERALL);

	_geode->addDrawable(arrow);

	// ##########################################################################

	// Arrow tip

	osg::ref_ptr<osg::ShapeDrawable> arrowTip = new osg::ShapeDrawable;
	arrowTip->setShape(new osg::Cone(osg::Vec3(0.0, 0.0, 0.85 * length), radius * 2, 0.6 * length));

	osg::StateSet * ssArrowTip = arrowTip->getOrCreateStateSet();

	ssArrowTip->setMode(GL_CULL_FACE, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	ssArrowTip->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL));

	osg::LightModel * lightModelTip = new osg::LightModel;
	lightModelTip->setTwoSided(true);
	ssArrowTip->setAttributeAndModes(lightModelTip);

	arrowTip->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

	// Set color
	arrowTip->setColorArray(colors);
	arrowTip->setColorBinding(osg::Geometry::BIND_OVERALL);

	_geode->addDrawable(arrowTip);

	osg::StateSet * ssGeode = _geode->getOrCreateStateSet();

	ssGeode->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	ssGeode->setMode(GL_BLEND, osg::StateAttribute::ON);
	ssGeode->setMode(GL_POINT_SMOOTH, osg::StateAttribute::ON);

	osg::ref_ptr<osg::Material> material = new osg::Material;

	material->setAmbient(osg::Material::FRONT_AND_BACK,  osg::Vec4(0.5f * color[0], 0.5f *  color[1], 0.5f * color[2], 1.0f));
	material->setDiffuse(osg::Material::FRONT_AND_BACK,  osg::Vec4(0.8f *  color[0], 0.8f * color[1], 0.8f * color[2], 1.0f));
	material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f    , 0.0f    , 0.0f    , 1.0f));

	material->setColorMode(osg::Material::OFF);

	material->setAlpha(osg::Material::FRONT_AND_BACK, color[3]);

	material->setShininess(osg::Material::FRONT_AND_BACK, 0);

	ssGeode->setAttribute(material);
}

void HandleArrow::updateTransform(void)
{
	osg::Matrix matrixRotate;
	matrixRotate.makeRotate(osg::Vec3d(0.0, 0.0, 1.0), direction);

	osg::Matrix matrixTranslate;
	matrixTranslate.makeTranslate(position);

	osgNode->setMatrix(matrixRotate * matrixTranslate);
}

void HandleArrow::setPosition(osg::Vec3d pos)
{
	position = pos;
	updateTransform();
}

void HandleArrow::mouseOver(bool flag)
{
	osgSwitchNode->setChildValue(arrowNode, !flag);
	osgSwitchNode->setChildValue(arrowNodeSelected, flag);
}

void HandleArrow::setInteractionReference(int intersectionIndex, double intersectionRatio)
{
	// Calculate the new reference offset
	assert(intersectionIndex == 0);

	referenceOffset = intersectionLineLength * (intersectionRatio - 0.5);
}

void HandleArrow::setInteraction(Viewer *viewer, int intersectionIndex, double intersectionRatio)
{
	// Calculate the new position
	assert(intersectionIndex == 0);

	double offset = intersectionLineLength * (intersectionRatio - 0.5) - referenceOffset;

	offset = viewer->snapDimension(offset);

	osg::Vec3d newPos = position + direction * offset;

	setPosition(newPos);

	reportInteraction();
}
