// @otlicense
// File: RubberbandOsgWrapper.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include <stdafx.h>

// RBE header
#include "rbeWrapper/RubberbandOsgWrapper.h"
#include "rbeWrapper/RubberbandHiddenLineNode.h"
#include "rbeWrapper/RubberbandPointNode.h"

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
#include <osg/BlendFunc>
#include <osg/Material>

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

	osg::ref_ptr<RubberbandHiddenLineNode> rb = new RubberbandHiddenLineNode();

	rb->setVertices(vertices);

	RubberbandHiddenLineNode::Style st;
	st.solidColor = osg::Vec4(0.12f, 0.72f, 1.0f, 0.95f);
	st.solidWidth = 2.0f;
	st.dashedColor = osg::Vec4(0.95f, 0.60f, 0.25f, 0.85f);
	st.dashedWidth = 2.0f;
	st.dashPattern = 0xF0F0; st.dashFactor = 1;
	st.lineSmooth = true;
	rb->setStyle(st);

	m_node = rb;

	m_parentGroup->addChild(m_node);
}

void rbeWrapper::RubberbandOsgWrapper::buildOriginNode(void) {
	// Create array
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(1);
	vertices->at(0).set(osg::Vec3(current()->u(), current()->v(), current()->w()));

	auto node = new RubberbandPointNode();
	node->setVertices(vertices);              
	node->enableDebugVisual(false);     
	node->setHiddenDash(false);

	m_node = node;
	m_node->setCullingActive(false);

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

	RubberbandHiddenLineNode *edges = dynamic_cast<RubberbandHiddenLineNode*>(m_node);
	if (edges != nullptr)
	{
		// Read connections and create edges list
		std::list<osg::Vec3> edgesList;
		calculateEdges(edgesList);

		// Create array
		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(edgesList.size());
		size_t ct{ 0 };
		for (auto pt : edgesList) {
			vertices->at(ct++).set(pt);
		}

		edges->setVertices(vertices);
	}
}

void rbeWrapper::RubberbandOsgWrapper::updateOriginNode(void) 
{
	RubberbandPointNode* point = dynamic_cast<RubberbandPointNode*>(m_node);
	if (point != nullptr)
	{
		osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(1);
		vertices->at(0).set(osg::Vec3(current()->u(), current()->v(), current()->w()));

		point->setVertices(vertices);
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