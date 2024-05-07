#include "stdafx.h"

// Project header
#include "AxisCross.h"
#include "SceneNodeMaterial.h"
#include "ViewerSettings.h"

// osg header
#include <osg/Group>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osg/LightModel>
#include <osg/MatrixTransform>
#include <osg/Material>
#include <osgText/Text>
#include <osg/BlendFunc>

// Rubberband engine header
#include <rbeCore/rbeAssert.h>

AxisCross::AxisCross(osg::Group * _parentGroup, osgText::Font *_font)
	: m_parentGroup(_parentGroup), m_isVisible(false), m_size(1.f),
	m_xNode(nullptr), m_yNode(nullptr), m_zNode(nullptr),
	m_midpointX(0.f), m_midpointY(0.f), m_midpointZ(0.f),
	m_cylinderRadius(0.08f), font(_font), m_isIdentity(true)
{
	m_axisCrossNode = new osg::MatrixTransform();
	m_parentGroup->addChild(m_axisCrossNode);

	ViewerSettings * settings = ViewerSettings::instance();

	textColor[0] = settings->axisCenterColor.r() / 255.0;
	textColor[1] = settings->axisCenterColor.g() / 255.0;
	textColor[2] = settings->axisCenterColor.b() / 255.0;
}

AxisCross::~AxisCross() {

}

void AxisCross::setTransformation(osg::Matrix matrix)
{
	bool needsRebuild = false;

	if (matrix.isIdentity())
	{
		if (!m_isIdentity)
		{
			needsRebuild = true;
		}
		m_isIdentity = true;
	}
	else
	{
		if (m_isIdentity)
		{
			needsRebuild = true;
		}
		m_isIdentity = false;
	}

	matrix.setTrans(0.0, 0.0, 0.0);
	m_axisCrossNode->setMatrix(matrix);

	if (needsRebuild)
	{
		rebuildNode();
	}
}

void AxisCross::setVisible(bool _visible) {
	if (m_isVisible == _visible) { return; }
	m_isVisible = _visible;
	rebuildNode();
}

void AxisCross::refreshAfterSettingsChange(void) {
	rebuildNode();
}

void AxisCross::setTextColor(double color[3])
{
	textColor[0] = color[0];
	textColor[1] = color[1];
	textColor[2] = color[2];

	rebuildNode();
}

// ############################################################################################################

// Private functions

void AxisCross::rebuildNode(void) {
	if (m_xTransform) {
		m_xTransform->removeChild(m_xNode);
		m_axisCrossNode->removeChild(m_xTransform);
		m_xTransform = nullptr;
		m_xNode = nullptr;
		
	}
	if (m_yTransform) {
		m_yTransform->removeChild(m_yNode);
		m_axisCrossNode->removeChild(m_yTransform);
		m_yTransform = nullptr;
		m_yNode = nullptr;
	}
	if (m_zTransform) {
		m_zTransform->removeChild(m_zNode);
		m_axisCrossNode->removeChild(m_zTransform);
		m_zTransform = nullptr;
		m_zNode = nullptr;
	}
	if (m_centerPoint) {
		m_axisCrossNode->removeChild(m_centerPoint);
		m_centerPoint = nullptr;
	}

	if (!m_isVisible) { return; }

	if (m_size <= 0.f) {
		rbeAssert(0, "Invalid size @AxisCross");
	}

	ViewerSettings * settings = ViewerSettings::instance();

	// Center point

	osg::StateSet* stateSet = m_axisCrossNode->getOrCreateStateSet();
	stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

	m_centerPoint = new osg::Geode;
	osg::ref_ptr<osg::ShapeDrawable> centerPoint = new osg::ShapeDrawable;
	centerPoint->setShape(new osg::Sphere(osg::Vec3(m_midpointX, m_midpointY, m_midpointZ), m_cylinderRadius * 2.f));

	osg::StateSet * ssCenter = centerPoint->getOrCreateStateSet();

	ssCenter->setMode(GL_CULL_FACE, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	ssCenter->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL));

	osg::LightModel * lightModel = new osg::LightModel;
	lightModel->setTwoSided(true);
	ssCenter->setAttributeAndModes(lightModel);

	// Enable polygon offset
	//ssCenter->setAttributeAndModes(new osg::PolygonOffset(-1.0f, 1.0f));

	//centerPoint->setNormalBinding(osg::Geometry::BIND_OVERALL);

	// Set color
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(1);
	colors->at(0).set(settings->axisCenterColor.r() / 255.0, settings->axisCenterColor.g() / 255.0, settings->axisCenterColor.b() / 255.0, settings->axisCenterColor.a() / 255.0);
	centerPoint->setColorArray(colors.get());
	centerPoint->setColorBinding(osg::Geometry::BIND_OVERALL);

	m_centerPoint->addDrawable(centerPoint);

	osg::StateSet * ssCenterGeode = m_centerPoint->getOrCreateStateSet();

	ssCenterGeode->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	ssCenterGeode->setMode(GL_BLEND, osg::StateAttribute::ON);
	ssCenterGeode->setMode(GL_POINT_SMOOTH, osg::StateAttribute::ON);

	osg::ref_ptr<osg::Material> material = new osg::Material;

	material->setAmbient(osg::Material::FRONT_AND_BACK, osg::Vec4(0.5f * settings->axisCenterColor.r() / 255.0, 0.5f * settings->axisCenterColor.g() / 255.0, 0.5f * settings->axisCenterColor.b() / 255.0, 1.0f));
	material->setDiffuse(osg::Material::FRONT_AND_BACK, osg::Vec4(0.8f * settings->axisCenterColor.r() / 255.0, 0.8f * settings->axisCenterColor.g() / 255.0, 0.8f * settings->axisCenterColor.b() / 255.0, 1.0f));
	material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f, 0.0f, 0.0f, 1.0f));

	material->setColorMode(osg::Material::OFF);

	material->setAlpha(osg::Material::FRONT_AND_BACK, settings->axisCenterColor.a() / 255.0);

	material->setShininess(osg::Material::FRONT_AND_BACK, 0);

	ssCenterGeode->setAttribute(material.get());

	m_axisCrossNode->addChild(m_centerPoint);

	std::string xlabel = "x";
	std::string ylabel = "y";
	std::string zlabel = "z";

	if (!m_isIdentity)
	{
		xlabel = "x'";
		ylabel = "y'";
		zlabel = "z'";
	}

	// Arrow X

	m_xNode = new osg::Geode;
	createArrow(m_xNode, settings->axisXColor);
	createText(m_xNode, xlabel);
	m_xTransform = new osg::MatrixTransform;
	m_xTransform->setMatrix(osg::Matrix::rotate(osg::PI_2, osg::Y_AXIS));
	m_xTransform->addChild(m_xNode);
	m_axisCrossNode->addChild(m_xTransform);

	// Arrow Y

	m_yNode = new osg::Geode;
	createArrow(m_yNode, settings->axisYColor);
	createText(m_yNode, ylabel);
	m_yTransform = new osg::MatrixTransform;
	m_yTransform->setMatrix(osg::Matrix::rotate(-osg::PI_2, osg::X_AXIS));
	m_yTransform->addChild(m_yNode);
	m_axisCrossNode->addChild(m_yTransform);

	// Arrow Z

	m_zNode = new osg::Geode;
	createArrow(m_zNode, settings->axisZColor);
	createText(m_zNode, zlabel);
	m_zTransform = new osg::MatrixTransform;
	m_zTransform->addChild(m_zNode);
	m_axisCrossNode->addChild(m_zTransform);
}
/*
	Box, Capsule, Cone, Cylinder, Sphere
*/
void AxisCross::createArrow(osg::Geode * _geode, const ot::Color& _color) {
	osg::ref_ptr<osg::ShapeDrawable> arrow = new osg::ShapeDrawable;
	arrow->setShape(new osg::Cylinder(osg::Vec3(m_midpointX, m_midpointY, m_midpointZ + (m_size / 2.f)), m_cylinderRadius, m_size));

	osg::StateSet * ssArrow = arrow->getOrCreateStateSet();

	ssArrow->setMode(GL_CULL_FACE, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	ssArrow->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL));

	osg::LightModel * lightModel = new osg::LightModel;
	lightModel->setTwoSided(true);
	ssArrow->setAttributeAndModes(lightModel);

	// Enable polygon offset
	//ssArrow->setAttributeAndModes(new osg::PolygonOffset(-1.0f, 1.0f));

	arrow->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

	// Set color
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(1);
	colors->at(0).set(_color.r() / 255.0, _color.g() / 255.0, _color.b() / 255.0, _color.a() / 255.0);
	arrow->setColorArray(colors.get());
	arrow->setColorBinding(osg::Geometry::BIND_OVERALL);

	_geode->addDrawable(arrow);

	// ##########################################################################

	// Arrow tip

	osg::ref_ptr<osg::ShapeDrawable> arrowTip = new osg::ShapeDrawable;
	arrowTip->setShape(new osg::Cone(osg::Vec3(m_midpointX, m_midpointY, m_midpointZ + m_size), 
		m_cylinderRadius * 2.5f, m_cylinderRadius * 5.f));

	osg::StateSet * ssArrowTip = arrowTip->getOrCreateStateSet();

	ssArrowTip->setMode(GL_CULL_FACE, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	ssArrowTip->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL));

	osg::LightModel * lightModelTip = new osg::LightModel;
	lightModelTip->setTwoSided(true);
	ssArrowTip->setAttributeAndModes(lightModelTip);

	// Enable polygon offset
	//ssArrowTip->setAttributeAndModes(new osg::PolygonOffset(-1.0f, 1.0f));

	arrowTip->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

	// Set color
	arrowTip->setColorArray(colors.get());
	arrowTip->setColorBinding(osg::Geometry::BIND_OVERALL);

	_geode->addDrawable(arrowTip);

	osg::StateSet * ssGeode = _geode->getOrCreateStateSet();

	ssGeode->setMode(GL_LIGHTING, osg::StateAttribute::ON);
	ssGeode->setMode(GL_BLEND, osg::StateAttribute::ON);
	ssGeode->setMode(GL_POINT_SMOOTH, osg::StateAttribute::ON);

	osg::ref_ptr<osg::Material> material = new osg::Material;

	material->setAmbient(osg::Material::FRONT_AND_BACK,  osg::Vec4(0.5f * _color.r() / 255.0, 0.5f * _color.g() / 255.0, 0.5f * _color.b() / 255.0, 1.0f));
	material->setDiffuse(osg::Material::FRONT_AND_BACK,  osg::Vec4(0.8f * _color.r() / 255.0, 0.8f * _color.g() / 255.0, 0.8f * _color.b() / 255.0, 1.0f));
	material->setSpecular(osg::Material::FRONT_AND_BACK, osg::Vec4(0.0f                     , 0.0f                     , 0.0f                     , 1.0f));

	material->setColorMode(osg::Material::OFF);

	material->setAlpha(osg::Material::FRONT_AND_BACK, _color.a() / 255.0);

	material->setShininess(osg::Material::FRONT_AND_BACK, 0);

	ssGeode->setAttribute(material.get());
}

void AxisCross::createText(osg::Geode * _geode, const std::string &label) 
{
	// We create the text as a child node
	osg::Geode *textNode = new osg::Geode;

	osg::Vec3 pos3(m_midpointX, m_midpointY, m_midpointZ + (1.5 * m_size));

	osg::ref_ptr<osgText::Text> text = new osgText::Text;

	text->setFont(font);
	text->setCharacterSize(0.32 * m_size);
	text->setFontResolution(120, 120);
	text->setAxisAlignment(osgText::Text::SCREEN);
	text->setAlignment(osgText::TextBase::AlignmentType::CENTER_CENTER);
	text->setPosition(pos3);
	text->setColor(osg::Vec4(textColor[0], textColor[1], textColor[2], 1.0));
	text->setText(label);
	text->setCharacterSizeMode(osgText::TextBase::OBJECT_COORDS);

	osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
	blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//_geode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	textNode->getOrCreateStateSet()->setAttributeAndModes(blendFunc);

	textNode->addDrawable(text);

	osg::StateSet * ssText = textNode->getOrCreateStateSet();

	ssText->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	ssText->setMode(GL_BLEND, osg::StateAttribute::ON);
	ssText->setMode(GL_POINT_SMOOTH, osg::StateAttribute::ON);

	_geode->addChild(textNode);
}
