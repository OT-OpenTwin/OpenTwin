// @otlicense

#pragma once

// Project header
#include <rbeCore/dataTypes.h>

// C++ header
#include <string>
#include <list>
#include <map>

namespace rbeCore {

	class Point;
	class Step;
	class AbstractConnection;
	class NumericPoint;

	class RBE_API_EXPORT RubberbandEngine {
	public:
		RubberbandEngine(coordinate_t _originU, coordinate_t _originV, coordinate_t _originW);
		virtual ~RubberbandEngine(); 

		// #################################################################################################################

		// Getter

		NumericPoint * origin(void) { return m_origin; }
		
		NumericPoint * current(void) { return m_current; }

		Point * point(int _id);
		
		bool hasPoint(int _id);
		
		int currentStep(void) const { return m_currentStep; }
		
		bool hasStep(int _id);

		Step * step(int _id);

		std::string debugInformation(void);

		std::string connectionsJsonArray(void);

		std::list<AbstractConnection *> connectionsToDisplay(void);

		std::map<int, Point *>& points(void);

		std::string pointsJsonArray(void);

		// #################################################################################################################

		// Setter

		bool hasNextStep(void);

		virtual void addPoint(Point * _point);

		virtual void replaceOrigin(coordinate_t _originU, coordinate_t _originV, coordinate_t _originW);

		virtual void updateCurrent(coordinate_t _currentU, coordinate_t _currentV, coordinate_t _currentW);

		virtual void setupFromJson(const char * _json);

		virtual void clear(void);

		virtual void activateStepOne(void);

		virtual void activateNextStep(void);
		
		virtual void applyCurrentStep(void);

	private:

		struct d_data;
		d_data *						m_data;

		int								m_currentStep;
		rbeCore::NumericPoint *			m_origin;
		rbeCore::NumericPoint *			m_current;
		
		RubberbandEngine() = delete;
		RubberbandEngine(RubberbandEngine&) = delete;
		RubberbandEngine& operator = (RubberbandEngine&) = delete;
	};

}