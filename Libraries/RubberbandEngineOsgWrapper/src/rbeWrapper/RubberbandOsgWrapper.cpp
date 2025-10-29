// @otlicense

#include <stdafx.h>

// RBE header
#include "rbeWrapper/RubberbandOsgWrapper.h"

#include "rbeCore/AbstractConnection.h"
#include "rbeCore/LineConnection.h"
#include "rbeCore/CircleConnection.h"
#include "rbeCore/AbstractPoint.h"
#include "rbeCore/rbeAssert.h"
#include "rbeCore/NumericPoint.h"

// osg header
#include <osg/StateSet>
#include <osg/Node>
#include <osg/Geode>
#include <osg/Switch>
#include <osg/Geometry>
#include <osg/PolygonMode>
#include <osg/LineWidth>
#include <osg/Point>
#include <osg/ShapeDrawable>

using namespace rbeCore;

rbeWrapper::RubberbandOsgWrapper::RubberbandOsgWrapper(osg::Switch *_parentGroup, coordinate_t _originU, coordinate_t _originV, coordinate_t _originW)
	: rbeCore::RubberbandEngine(_originU, _originV, _originW), m_parentGroup(_parentGroup), m_node(nullptr),
	m_r(1.), m_g(0.), m_b(0.), m_depthTest(false), m_circleSegments(100), m_pickOriginEnabled(false)
{
	// Here we remove all potentially remaining items in the rubberband group
	while (m_parentGroup->getNumChildren() > 0)
	{
		m_parentGroup->removeChild(0, 1);
	}


}

rbeWrapper::RubberbandOsgWrapper::~RubberbandOsgWrapper() {
	if (m_node) {
		m_parentGroup->setChildValue(m_node, false);
		//m_parentGroup->removeChild(m_node);
		m_node = nullptr;
	}
}

// ############################################################################################

// Base class functions

void rbeWrapper::RubberbandOsgWrapper::addPoint(rbeCore::Point * _point) {

	RubberbandEngine::addPoint(_point);
}

void rbeWrapper::RubberbandOsgWrapper::replaceOrigin(coordinate_t _originU, coordinate_t _originV, coordinate_t _originW) {
	RubberbandEngine::replaceOrigin(_originU, _originV, _originW);
	updateNode();
}

void rbeWrapper::RubberbandOsgWrapper::updateCurrent(coordinate_t _currentU, coordinate_t _currentV, coordinate_t _currentW) {
	RubberbandEngine::updateCurrent(_currentU, _currentV, _currentW);
	updateNode();
}

void rbeWrapper::RubberbandOsgWrapper::setupFromJson(const char * _json) {
	cleanupOsgData();
	RubberbandEngine::setupFromJson(_json);
}

void rbeWrapper::RubberbandOsgWrapper::clear(void) {
	RubberbandEngine::clear();
	cleanupOsgData();
}

void rbeWrapper::RubberbandOsgWrapper::activateStepOne(void) {
	m_pickOriginEnabled = true;
	buildNode();
}

void rbeWrapper::RubberbandOsgWrapper::activateNextStep(void) {
	if (m_pickOriginEnabled) {
		m_pickOriginEnabled = false;
		RubberbandEngine::replaceOrigin(current()->u(), current()->v(), current()->w());
		RubberbandEngine::activateStepOne();
	}
	else {
		RubberbandEngine::activateNextStep();
	}
	
	buildNode();
}

void rbeWrapper::RubberbandOsgWrapper::applyCurrentStep(void) {
	if (m_pickOriginEnabled) {
		replaceOrigin(current()->u(), current()->v(), current()->w());
	}
	else {
		RubberbandEngine::applyCurrentStep();
	}
	
}

// ############################################################################################

// Private functions

void rbeWrapper::RubberbandOsgWrapper::cleanupOsgData(void) {
	if (m_node) {
		m_parentGroup->setChildValue(m_node, false);
		//m_parentGroup->removeChild(m_node);
		m_node = nullptr;
	}
}

void rbeWrapper::RubberbandOsgWrapper::buildNode(void) {
	cleanupOsgData();

	if (m_pickOriginEnabled) { buildOriginNode(); return; }
	

	// Read connections and create edges list
	std::list<osg::Vec3> edgesList;
	calculateEdges(edgesList);

	// Create array
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(edgesList.size());
	size_t ct{ 0 };
	for (auto pt : edgesList) {
		vertices->at(ct++).set(pt);
	}

	assert((ct / 2) * 2 == ct);

	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(m_r, m_g, m_b, 1.0f));

	// Create the geometry object to store the data
	osg::ref_ptr<osg::Geometry> newGeometry = new osg::Geometry;

	newGeometry->setVertexArray(vertices.get());

	newGeometry->setColorArray(colors.get());
	newGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	newGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::Mode::LINES, 0, ct));

	//	if (depthTest) {
	//		osg::ref_ptr<osg::Material> matEdges = new osg::Material;
	//		this->setMaterialProperties(matEdges, r, g, b);
	//		newGeometry->getOrCreateStateSet()->setAttribute(matEdges.get());
	//	}

		// Now create the geometry node and assign the drawable
	m_node = new osg::Geode;
	m_node->addDrawable(newGeometry);

	// Set the display attributes for the edges geometry
	osg::StateSet *ss = m_node->getOrCreateStateSet();

	//osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
	//blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//ss->setAttributeAndModes(blendFunc);

	ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	ss->setMode(GL_BLEND, osg::StateAttribute::OFF);
	ss->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
	ss->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
	ss->setAttribute(new osg::LineWidth(1.0), osg::StateAttribute::ON);

	if (!m_depthTest)
	{
		ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	}

	m_parentGroup->addChild(m_node);
}

void rbeWrapper::RubberbandOsgWrapper::buildOriginNode(void) {
	// Create array
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(1);
	vertices->at(0).set(osg::Vec3(current()->u(), current()->v(), current()->w()));

	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
	colors->push_back(osg::Vec4(m_r, m_g, m_b, 1.0f));

	// Create the geometry object to store the data
	osg::ref_ptr<osg::Geometry> newGeometry = new osg::Geometry;

	newGeometry->setVertexArray(vertices.get());

	newGeometry->setColorArray(colors.get());
	newGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	newGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0, 1));

	//	if (depthTest) {
	//		osg::ref_ptr<osg::Material> matEdges = new osg::Material;
	//		this->setMaterialProperties(matEdges, r, g, b);
	//		newGeometry->getOrCreateStateSet()->setAttribute(matEdges.get());
	//	}

		// Now create the geometry node and assign the drawable
	m_node = new osg::Geode;
	m_node->addDrawable(newGeometry);
	m_node->setCullingActive(false);

	// Set the display attributes for the edges geometry
	osg::StateSet *ss = m_node->getOrCreateStateSet();

	//osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
	//blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//ss->setAttributeAndModes(blendFunc);

	ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	ss->setMode(GL_BLEND, osg::StateAttribute::OFF);
	ss->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
	ss->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
	ss->setAttribute(new osg::Point(10), osg::StateAttribute::ON);

	if (!m_depthTest)
	{
		//ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);
	}

	m_parentGroup->addChild(m_node);
}

void rbeWrapper::RubberbandOsgWrapper::updateNode(void) {
	if (m_node == nullptr)
	{
		assert(0);
		return;
	}

	if (m_pickOriginEnabled) {
		updateOriginNode();
		return;
	}

	osg::Geometry * geometry = dynamic_cast<osg::Geometry *>(m_node->getDrawable(0));
	if (geometry) {
		// Read connections and create edges list
		std::list<osg::Vec3> edgesList;
		calculateEdges(edgesList);

		// Create array
		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(edgesList.size());
		size_t ct{ 0 };
		for (auto pt : edgesList) {
			vertices->at(ct++).set(pt);
		}

		geometry->setVertexArray(vertices.get());
		geometry->dirtyDisplayList();
	}
	else {
		assert(0);
	}
}

void rbeWrapper::RubberbandOsgWrapper::updateOriginNode(void) {
	osg::Geometry * geometry = dynamic_cast<osg::Geometry *>(m_node->getDrawable(0));
	if (geometry) {
		// Read connections and create edges list
		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(1);
		vertices->at(0).set(osg::Vec3(current()->u(), current()->v(), current()->w()));

		geometry->setVertexArray(vertices.get());
		geometry->dirtyDisplayList();
	}
	else {
		assert(0);
	}
}

void rbeWrapper::RubberbandOsgWrapper::calculateEdges(std::list<osg::Vec3>& _list) {
	std::list<AbstractConnection *> connections = connectionsToDisplay();
	for (AbstractConnection * connection : connections) {
		switch (connection->type())
		{
		case AbstractConnection::ctLine:
		{
			LineConnection * actualConnection = dynamic_cast<LineConnection *>(connection);
			_list.push_back({ actualConnection->from()->u(), actualConnection->from()->v(), actualConnection->from()->w() });
			_list.push_back({ actualConnection->to()->u(), actualConnection->to()->v(), actualConnection->to()->w() });
		}
		break;
		case AbstractConnection::ctCircle:
		{
			CircleConnection * actualConnection = dynamic_cast<CircleConnection *>(connection);
			coordinate_t midU = actualConnection->centerPoint()->u();
			coordinate_t midV = actualConnection->centerPoint()->v();
			coordinate_t midW = actualConnection->centerPoint()->w();
			coordinate_t radius = actualConnection->radius();
			CircleOrientation orientation = actualConnection->orientation();

			unsigned long long ct{ 0 };
			coordinate_t lU, lV, lW;
			coordinate_t fU, fV, fW;

			switch (orientation)
			{
			case rbeCore::coUV:
			{
				for (int i = 0; i < m_circleSegments; i++)
				{
					if (ct++ > 1) {
						_list.push_back({ lU, lV, lW });
					}

					float theta = 2.f * 3.1415926f * float(i) / float(m_circleSegments);
					float u = radius * cosf(theta);
					float v = radius * sinf(theta);

					lU = midU + u;
					lV = midV + v;
					lW = midW;
					_list.push_back({ lU, lV, lW });

					if (ct == 1) {
						fU = lU;
						fV = lV;
						fW = lW;
					}
				}
				if (ct > 0) {
					_list.push_back({ lU, lV, lW });
					_list.push_back({ fU, fV, fW });
				}
			}
			break;
			case rbeCore::coUW:
			{
				for (int i = 0; i < m_circleSegments; i++)
				{
					if (ct++ > 1) {
						_list.push_back({ lU, lV, lW });
					}

					float theta = 2.f * 3.1415926f * float(i) / float(m_circleSegments);
					float u = radius * cosf(theta);
					float w = radius * sinf(theta);

					lU = midU + u;
					lV = midV;
					lW = midW + w;
					_list.push_back({ lU, lV, lW });

					if (ct == 1) {
						fU = lU;
						fV = lV;
						fW = lW;
					}
				}
				if (ct > 0) {
					_list.push_back({ lU, lV, lW });
					_list.push_back({ fU, fV, fW });
				}
			}
			break;
			case rbeCore::coVW:
			{
				for (int i = 0; i < m_circleSegments; i++)
				{
					if (ct++ > 1) {
						_list.push_back({ lU, lV, lW });
					}

					float theta = 2.f * 3.1415926f * float(i) / float(m_circleSegments);
					float v = radius * cosf(theta);
					float w = radius * sinf(theta);

					lU = midU;
					lV = midV + v;
					lW = midW + w;
					_list.push_back({ lU, lV, lW });

					if (ct == 1) {
						fU = lU;
						fV = lV;
						fW = lW;
					}
				}
				if (ct > 0) {
					_list.push_back({ lU, lV, lW });
					_list.push_back({ fU, fV, fW });
				}
			}
			break;
			default:
				rbeAssert(0, "Invalid circle orientation");
				break;
			}
		}
		break;
		default:
			rbeAssert(0, "Not implemented connection type");
			break;
		}
	}

}