// @otlicense

#pragma once

// RBE header
#include <rbeCore/dataTypes.h>

// C++ header
#include <string>
#include <list>
#include <sstream>

namespace rbeCalc { class VariableValue; }

namespace rbeCore {

	class Point;
	class RubberbandEngine;
	class AbstractConnection;
	class AbstractPoint;
	class Limit;

	class RBE_API_EXPORT Step {
	public:

		enum eProjection {
			pUV,
			pW
		};

		Step();
		virtual ~Step();

		// ###################################################################################

		// Setter

		void setId(int _id) { m_id = _id; }

		void setMayEndWithout(bool _mayEnd) { m_mayEndWithout = _mayEnd; }

		void setProjection(eProjection _projection) { m_projection = _projection; }

		void addPoint(Point * _p);

		void addLimit(Limit * _limit);

		void setupFromJson(RubberbandEngine * _engine, const std::string& _json);

		void givePointOwnershipToEngine(RubberbandEngine * _engine);

		// ###################################################################################

		// Getter

		int id(void) const { return m_id; }

		bool mayEndWithout(void) const { return m_mayEndWithout; }

		bool hasPointOwnership(void) const { return m_pointsOwned; }

		eProjection projection(void) const { return m_projection; }

		Point * point(int _id);

		bool hasPoint(int _id);

		std::string debugInformation(const std::string& _prefix);

		void addConnectionsToJsonArray(RubberbandEngine * _engine, std::stringstream& _array, bool& _first, bool _isHistory);

		void addConnectionsToList(std::list<AbstractConnection *>& _list, bool _isHistory);

		//! @brief Will check the limits for the provided points. If a limit is exceeded the values will be adjusted to match the limtis
		void adjustCoordinateToLimits(coordinate_t & _u, coordinate_t & _v, coordinate_t & _w);

		void setLastPosition(coordinate_t _u, coordinate_t _v, coordinate_t _w);

		AbstractPoint * lastPosition(void);

	private:
		struct sData;

		bool						m_pointsOwned;
		bool						m_mayEndWithout;
		int							m_id;
		sData *						m_data;
		eProjection					m_projection;
		AbstractPoint *				m_lastCurrent;
		rbeCalc::VariableValue *	m_lastCurrentU;
		rbeCalc::VariableValue *	m_lastCurrentV;
		rbeCalc::VariableValue *	m_lastCurrentW;

		Step(Step&) = delete;
		Step& operator = (Step&) = delete;
	};

}