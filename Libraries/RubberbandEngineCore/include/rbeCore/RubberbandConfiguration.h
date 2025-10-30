// @otlicense
// File: RubberbandConfiguration.h
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

#ifdef RUBBERBANDENGINECORE_EXPORTS
#pragma warning(disable: 4251)
#endif // RUBBERBANDENGINECORE_EXPORTS

#include <rbeCore/dataTypes.h>

#include <rapidjson/document.h>

#include <string>
#include <list>

namespace rbeCore {

	class cfgStep;
	class cfgLimit;
	class cfgPoint;
	class ConnectionConfigurationData;
	class cfgLineConnection;
	class cfgHistoryConnection;
	class cfgCircleConnection;

	class RBE_API_EXPORT RubberbandConfiguration {
	public:

		RubberbandConfiguration();
		virtual ~RubberbandConfiguration();

		// #############################################################################################

		// Defaults

		void setToDefaultCuboid(void);

		void setToDefaultCylinder(void);

		void setToDefaultSphere(void);

		void setToDefaultTorus(void);

		void setToDefaultCone(void);

		void setToDefaultPyramid(void);

		// #############################################################################################

		// Getter

		const std::list<cfgStep> steps(void) const { return m_steps; }

		void addToJsonObject(rapidjson::Document& _doc, rapidjson::Value& _object) const;

		// #############################################################################################

		// Operators

		void clear(void);

		RubberbandConfiguration& operator << (const cfgStep& _step);

		std::string toJson(void) const;

	private:
		std::list<cfgStep>		m_steps;

		RubberbandConfiguration(RubberbandConfiguration&) = delete;
		RubberbandConfiguration& operator = (RubberbandConfiguration&) = delete;
	};

	// ##################################################################################################################################################

	// ##################################################################################################################################################

	// ##################################################################################################################################################

	class RBE_API_EXPORT ConnectionConfigurationData {
	public:
		ConnectionConfigurationData();
		ConnectionConfigurationData(const ConnectionConfigurationData& _other);
		virtual ~ConnectionConfigurationData();
		ConnectionConfigurationData& operator = (const ConnectionConfigurationData& _other);

		// ###########################################################################################################

		// Setter

		void add(const cfgCircleConnection& _connection);
		void add(const cfgLineConnection& _connection);
		void add(const cfgHistoryConnection& _connection);

		// #############################################################################################

		// Getter

		const std::list<cfgCircleConnection>& circles(void) const { return m_circles; }
		const std::list<cfgLineConnection>& lines(void) const { return m_lines; }
		const std::list<cfgHistoryConnection>& history(void) const { return m_history; }

		void addToJsonArray(rapidjson::Document& _doc, rapidjson::Value& _array) const;

	private:

		std::list<cfgCircleConnection>		m_circles;
		std::list<cfgLineConnection>		m_lines;
		std::list<cfgHistoryConnection>		m_history;
	};

	// ##################################################################################################################################################

	// ##################################################################################################################################################

	// ##################################################################################################################################################

	class RBE_API_EXPORT cfgStep {
	public:
		enum Projection {
			UV,
			W
		};

		//! @param _projection The coordinate projection this step is using (only the values of the selected projection will be updated)
		//! @param _mayEndWithoutThisStep If true the execution of the rubberband may be finished without finishing this step
		cfgStep(Projection _projection = UV, bool _mayEndWithoutThisStep = false);
		cfgStep(const cfgStep& _other);
		virtual ~cfgStep();
		cfgStep& operator = (const cfgStep& _other);

		// #############################################################################################

		// Getter

		const std::list<cfgPoint>&	points(void) const { return m_points; }
		Projection projection(void) const { return m_projection; }
		const ConnectionConfigurationData& connectionData(void) const { return m_connectionData; }
		bool mayEndWithout(void) const { return m_mayEndWithoutThisStep; }

		void addToJsonObject(rapidjson::Document& _doc, rapidjson::Value& _object, int _id) const;

		// #############################################################################################

		cfgStep& operator << (const cfgPoint& _point);
		cfgStep& operator << (const cfgLineConnection& _connection);
		cfgStep& operator << (const cfgCircleConnection& _connection);
		cfgStep& operator << (const cfgHistoryConnection& _connection);
		cfgStep& operator << (const ConnectionConfigurationData& _connectionData);
		cfgStep& operator << (const cfgLimit& _limit);

	private:

		Projection						m_projection;
		bool							m_mayEndWithoutThisStep;
		std::list<cfgPoint>				m_points;
		std::list<cfgLimit>				m_limits;
		ConnectionConfigurationData		m_connectionData;
	};

	// ##################################################################################################################################################

	// ##################################################################################################################################################

	// ##################################################################################################################################################

	class RBE_API_EXPORT cfgLimit {
	public:
		cfgLimit(AxisLimit _axisLimit, int _pointId);
		cfgLimit(AxisLimit _axisLimit, const std::string& _formula);
		cfgLimit(const cfgLimit& _other);
		virtual ~cfgLimit(void);
		cfgLimit& operator = (const cfgLimit& _other);

		// ###########################################################################################################

		// Getter

		void addToJsonObject(rapidjson::Document& _doc, rapidjson::Value& _object) const;

	private:

		AxisLimit		m_axisLimit;
		std::string		m_formula;

		cfgLimit() = delete;
	};

	// ##################################################################################################################################################

	// ##################################################################################################################################################

	// ##################################################################################################################################################
	
	class RBE_API_EXPORT cfgPoint {
	public:
		cfgPoint(int _id, const std::string& _formulaU, const std::string& _formulaV, const std::string& _formulaW);
		cfgPoint(int _id, DefaultPoint _formulaU, const std::string& _formulaV, const std::string& _formulaW);
		cfgPoint(int _id, const std::string& _formulaU, DefaultPoint _formulaV, const std::string& _formulaW);
		cfgPoint(int _id, DefaultPoint _formulaU, DefaultPoint _formulaV, const std::string& _formulaW);
		cfgPoint(int _id, const std::string& _formulaU, const std::string& _formulaV, DefaultPoint _formulaW);
		cfgPoint(int _id, DefaultPoint, const std::string& _formulaV, DefaultPoint _formulaW);
		cfgPoint(int _id, const std::string& _formulaU, DefaultPoint _formulaV, DefaultPoint _formulaW);
		cfgPoint(int _id, DefaultPoint _formulaU, DefaultPoint _formulaV, DefaultPoint _formulaW);
		cfgPoint(const cfgPoint& _other);
		virtual ~cfgPoint();
		cfgPoint& operator = (const cfgPoint& _other);

		// ###########################################################################################################

		// Getter

		int id(void) const { return m_id; }
		const std::string& u(void) const { return m_formulaU; }
		const std::string& v(void) const { return m_formulaV; }
		const std::string& w(void) const { return m_formulaW; }

		void addToJsonObject(rapidjson::Document& _doc, rapidjson::Value& _object) const;

	private:
		static std::string toString(DefaultPoint _f, eAxis _axis);

		int				m_id;
		std::string		m_formulaU;
		std::string		m_formulaV;
		std::string		m_formulaW;
	};

	// ##################################################################################################################################################

	// ##################################################################################################################################################

	// ##################################################################################################################################################

	class RBE_API_EXPORT cfgLineConnection {
	public:
		cfgLineConnection(int _pointFromId, int _pointToId, bool _ignoreInHistory = false);
		cfgLineConnection(DefaultPoint _pointFrom, int _pointToId, bool _ignoreInHistory = false);
		cfgLineConnection(int _pointFromId, DefaultPoint _pointTo, bool _ignoreInHistory = false);
		cfgLineConnection(DefaultPoint _pointFrom, DefaultPoint _pointTo, bool _ignoreInHistory = false);
		cfgLineConnection(const cfgLineConnection& _other);
		virtual ~cfgLineConnection();
		cfgLineConnection& operator = (const cfgLineConnection& _other);

		// ###########################################################################################################

		// Getter

		const std::string& idFrom(void) const { return m_from; }
		const std::string& idTo(void) const { return m_to; }

		void addToJsonObject(rapidjson::Document& _doc, rapidjson::Value& _object) const;

	private:

		std::string		m_from;
		std::string		m_to;
		bool	m_ignoreInHistory;

		cfgLineConnection() = delete;
	};

	// ##################################################################################################################################################

	// ##################################################################################################################################################

	// ##################################################################################################################################################

	class RBE_API_EXPORT cfgHistoryConnection {
	public:
		//! @param _stepId The id of the step. The id of the step is its index in the step list (starting from 1)
		cfgHistoryConnection(int _stepId, bool _ignoreInHistory = false);
		cfgHistoryConnection(const cfgHistoryConnection& _other);
		virtual ~cfgHistoryConnection();
		cfgHistoryConnection& operator = (const cfgHistoryConnection& _other);

		// ###########################################################################################################

		// Getter

		int stepId(void) const { return m_stepId; }

		void addToJsonObject(rapidjson::Document& _doc, rapidjson::Value& _object) const;

	private:
		int		m_stepId;
		bool	m_ignoreInHistory;

		cfgHistoryConnection() = delete;
	};

	// ##################################################################################################################################################

	// ##################################################################################################################################################

	// ##################################################################################################################################################

	class RBE_API_EXPORT cfgCircleConnection {
	public:
		cfgCircleConnection(int _centerPointId, const std::string& _radius, CircleOrientation _circleOrientation, bool _ignoreInHistory = false);
		cfgCircleConnection(DefaultPoint _centerPoint, const std::string& _radius, CircleOrientation _circleOrientation, bool _ignoreInHistory = false);
		cfgCircleConnection(const cfgCircleConnection& _other);
		virtual ~cfgCircleConnection();
		cfgCircleConnection& operator = (const cfgCircleConnection& _other);
		// ###########################################################################################################

		// Getter

		const std::string& centerPoint(void) const { return m_centerPoint; }
		const std::string& radiusFormula(void) const { return m_radius; }
		CircleOrientation orientation(void) const { return m_orientation; }

		void addToJsonObject(rapidjson::Document& _doc, rapidjson::Value& _object) const;

	private:
		std::string			m_centerPoint;
		std::string			m_radius;
		CircleOrientation	m_orientation;
		bool				m_ignoreInHistory;
	};
}
