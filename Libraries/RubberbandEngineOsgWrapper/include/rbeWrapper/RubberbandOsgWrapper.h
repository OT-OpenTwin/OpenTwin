// @otlicense
// File: RubberbandOsgWrapper.h
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

#pragma once

// RBE header
#include "rbeWrapper/rbeOsgWrapperDatatypes.h"
#include "rbeCore/RubberbandEngine.h"

#include <osg/Array>
#include <list>

namespace osg {
	class Group;
	class Switch;
}

namespace rbeWrapper {

	class RBE_OSG_API_EXPORT RubberbandOsgWrapper : public rbeCore::RubberbandEngine {
	public:

		RubberbandOsgWrapper(osg::Switch *_parentGroup, coordinate_t _originU, coordinate_t _originV, coordinate_t _originW);
		virtual ~RubberbandOsgWrapper();

		// ############################################################################################

		// Base class functions

		virtual void addPoint(rbeCore::Point * _point) override;

		virtual void replaceOrigin(coordinate_t _originU, coordinate_t _originV, coordinate_t _originW) override;

		virtual void updateCurrent(coordinate_t _currentU, coordinate_t _currentV, coordinate_t _currentW) override;

		virtual void setupFromJson(const char * _json) override;

		virtual void clear(void) override;

		//! @brief Will enable the Origin pick mode
		//! The origin pick mode is allowing the user the select the origin of the geometry
		virtual void activateStepOne(void) override;

		//! @brief Will activate the next step
		//! Can be used to activate the first step if the origin should not be picked by the used.
		virtual void activateNextStep(void) override;

		virtual void applyCurrentStep(void) override;

		// ############################################################################################

		// Getter

		osg::Group * osgNode(void) { return m_node; }

		float lineColorR(void) const { return m_r; }
		float lineColorG(void) const { return m_g; }
		float lineColorB(void) const { return m_b; }

		bool isDepthTestActive(void) const { return m_depthTest; }

		// ############################################################################################

		// Setter

		void setLineColor(float _r, float _g, float _b) { m_r = _r; m_g = _g; m_b = _b; }

		void setDepthTestActive(bool _isActive) { m_depthTest = _isActive; }



	protected:
		void calculateEdges(std::list<osg::Vec3>& _list);

	private:

		void cleanupOsgData(void);

		void buildNode(void);

		void buildOriginNode(void);

		void updateNode(void);

		void updateOriginNode(void);

		osg::Switch *m_parentGroup;
		osg::Group  *m_node;

		bool			m_pickOriginEnabled;
		float			m_r;
		float			m_g;
		float			m_b;
		bool			m_depthTest;
		int				m_circleSegments;
	};
	
}

