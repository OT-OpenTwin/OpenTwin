#pragma once

#include "OTCore/CoreTypes.h"

// C++ header
#include <string>

#include <rbeCore/dataTypes.h>
#include <osg/Array>

namespace rbeWrapper {
	class RubberbandOsgWrapper;
}
namespace osg {
	class Switch;
}

class Rubberband {
public:

	Rubberband(osg::Switch *_group, ot::serviceID_t _creator, const std::string& _note, const std::string& _configurationJson);

	virtual ~Rubberband();

	// ##########################################################################################

	// Setter

	bool switchToNextStep(void);

	void applyCurrentStep(void);

	// ##########################################################################################

	// Getter

	ot::serviceID_t creator(void) const { return m_creatorId; }

	const std::string& note(void) const { return m_note; }

	rbeWrapper::RubberbandOsgWrapper * engine(void) { return m_engine; }

	void updateCurrentPosition(coordinate_t _u, coordinate_t _v, coordinate_t _w);

	std::string createPointDataJson(void);

private:
	ot::serviceID_t						m_creatorId;
	std::string							m_note;
	rbeWrapper::RubberbandOsgWrapper *	m_engine;

	Rubberband() = delete;
	Rubberband(Rubberband&) = delete;
	Rubberband& operator = (Rubberband&) = delete;
};