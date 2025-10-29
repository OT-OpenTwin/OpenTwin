// @otlicense

// RBE header
#include <rbeCore/RubberbandEngine.h>
#include <rbeCore/Point.h>
#include <rbeCore/NumericPoint.h>
#include <rbeCore/Step.h>
#include <rbeCore/jsonMember.h>
#include <rbeCore/rbeAssert.h>

// rapidjson
#include <rapidjson/document.h>
#include <rapidjson/rapidjson.h>
#include <rapidjson/writer.h>
#include <rapidjson/stringbuffer.h>

// C++ header
#include <map>
#include <vector>
#include <string>

using namespace rbeCore;

struct RubberbandEngine::d_data {
	std::map<int, rbeCore::Point *>	points;
	std::map<int, std::pair<Step *, std::string>> steps;
};

RubberbandEngine::RubberbandEngine(coordinate_t _originU, coordinate_t _originV, coordinate_t _originW)
	: m_origin(nullptr), m_current(nullptr), m_currentStep(0) {

	m_origin = new NumericPoint(_originU, _originV, _originW);
	m_current = new NumericPoint(_originU, _originV, _originW);

	m_data = new d_data;
}

RubberbandEngine::~RubberbandEngine() {
	clear();
	if (m_origin) { delete m_origin; } m_origin = nullptr;
	if (m_current) { delete m_current; } m_current = nullptr;
	delete m_data;
}

// #################################################################################################################

// Getter

Point * RubberbandEngine::point(int _id) {
	auto p = m_data->points.find(_id);
	if (p == m_data->points.end()) {
		rbeAssert(0, "Provided point ID not found @RubberbandEngine");
		return nullptr;
	}
	return p->second;
}

bool RubberbandEngine::hasPoint(int _id) {
	auto p = m_data->points.find(_id);
	return p != m_data->points.end();
}

std::string RubberbandEngine::debugInformation(void) {
	std::string ret{ "RubberbandEngine -  Data\n{\n\t\"Origin\": " };
	ret.append(m_origin->debugInformation("\t"));
	ret.append(",\n\t\"Current\": ");
	ret.append(m_current->debugInformation("\t"));
	ret.append(",\n\t\"Points\": [");

	bool first{ true };
	for (auto p : m_data->points) {
		if (first) { ret.append("\n\t\t"); first = false; }
		else { ret.append(",\n\t\t"); }
		ret.append(p.second->debugInformation("\t\t"));
	}

	ret.append("\n\t],\n\t\"Steps\": [");
	first = true;
	for (auto s : m_data->steps) {
		if (first) {
			ret.append("\n\t\t"); first = false;
		}
		else {
			ret.append(",\n\t\t");
		}
		if (s.second.first) {
			ret.append(s.second.first->debugInformation("\t\t"));
		}
		else {
			ret.append("{\n\t\t\t\"ID\": ");
			ret.append(std::to_string(s.first));
			ret.append(",\n\t\t\t\t\"json\": \"");
			ret.append(s.second.second).append("\n\t\t\t}");
		}
	}
	ret.append("\n\t]\n}");
	return ret;
}

std::string RubberbandEngine::connectionsJsonArray(void) {
	std::stringstream stream;
	stream << "[";
	bool first{ true };
	auto step = m_data->steps.find(m_currentStep);
	if (step != m_data->steps.end()) {
		if (step->second.first) {
			step->second.first->addConnectionsToJsonArray(this, stream, first, false);
		}
	}
	stream << "]";
	return stream.str();
}

std::list<AbstractConnection *> RubberbandEngine::connectionsToDisplay(void) {
	std::list<AbstractConnection *> ret;

	auto step = m_data->steps.find(m_currentStep);
	if (step != m_data->steps.end()) {
		if (step->second.first) {
			step->second.first->addConnectionsToList(ret, false);
		}
		else {
			rbeAssert(0, "Fatal error: Current step was not created @RubberbandEngine");
		}
	}
	else {
		rbeAssert(0, "Data error: The active step does not exist @RubberbandEngine");
	}

	return ret;
}

std::map<int, Point *>& RubberbandEngine::points(void) {
	return m_data->points;
}

std::string RubberbandEngine::pointsJsonArray(void) {
	std::string ret{ "[" };
	bool first{ true };

	for (auto pt : m_data->points) {
		if (first) { first = false; }
		else { ret.append(","); }
		ret.append("{\"" RBE_JSON_Point_ID "\":");
		ret.append(std::to_string(pt.second->id()));
		ret.append(",\"" RBE_JSON_Point_U "\":");
		ret.append(std::to_string(pt.second->u()));
		ret.append(",\"" RBE_JSON_Point_V "\":");
		ret.append(std::to_string(pt.second->v()));
		ret.append(",\"" RBE_JSON_Point_W "\":");
		ret.append(std::to_string(pt.second->w()));
		ret.append("}");
	}
	ret.append("]");
	return ret;
}

bool RubberbandEngine::hasStep(int _id) {
	auto s = m_data->steps.find(_id);
	return s != m_data->steps.end();
}

Step * RubberbandEngine::step(int _id) {
	auto s = m_data->steps.find(_id);
	if (s == m_data->steps.end()) {
		rbeAssert(0, "Step with the provided ID does not exist @RubberbandEngine");
		return nullptr;
	}
	if (s->second.first) {
		return s->second.first;
	}
	else {
		rbeAssert(0, "The requested step was not activated yet @RubberbandEngine");
		return nullptr;
	}
}

// #################################################################################################################

// Setter

void RubberbandEngine::addPoint(Point * _point) {
	if (_point) {
		auto p = m_data->points.find(_point->id());
		if (p == m_data->points.end()) {
			m_data->points.insert_or_assign(_point->id(), _point);
		}
		else {
			rbeAssert(0, "Provided point ID already in use @RubberbandEngine");
		}
	}
	else {
		rbeAssert(0, "nullptr provided to addPoint @RubberbandEngine");
	}
}

void RubberbandEngine::replaceOrigin(coordinate_t _originU, coordinate_t _originV, coordinate_t _originW) {
	m_origin->set(_originU, _originV, _originW);
}

void RubberbandEngine::updateCurrent(coordinate_t _currentU, coordinate_t _currentV, coordinate_t _currentW) {
	if (hasStep(m_currentStep)) {
		step(m_currentStep)->adjustCoordinateToLimits(_currentU, _currentV, _currentW);
	}
	m_current->set(_currentU, _currentV, _currentW);
}

void RubberbandEngine::setupFromJson(const char * _json) {
	clear();

	if (_json == nullptr) {
		rbeAssert(0, "No json string provided @RubberbandEngine");
		return;
	}

	rapidjson::Document doc;
	doc.Parse(_json);

	if (!doc.IsObject()) {
		rbeAssert(0, "Invalid JSON format: Document is not an object @RubberbandEngine"); return;
	}
	if (!doc.HasMember(RBE_JSON_DOC_RubberbandSteps)) {
		rbeAssert(0, "Invalid JSON format: Document has no Steps @RubberbandEngine"); return;
	}
	if (!doc[RBE_JSON_DOC_RubberbandSteps].IsArray()) {
		rbeAssert(0, "Invalid JSON format: Steps data is not an array @RubberbandEngine"); return;
	}

	auto stepsData = doc[RBE_JSON_DOC_RubberbandSteps].GetArray();
	
	for (rapidjson::SizeType i{ 0 }; i < stepsData.Size(); i++) {
		if (!stepsData[i].IsObject()) {
			rbeAssert(0, "Invalid JSON format: Step entry is not an object @RubberbandEngine"); return;
		}
		auto sObj = stepsData[i].GetObject();

		if (!sObj.HasMember(RBE_JSON_STEP_Step)) {
			rbeAssert(0, "Invalid JSON format: Step id is missing @RubberbandEngine"); return;
		}
		else if (!sObj[RBE_JSON_STEP_Step].IsInt()) {
			rbeAssert(0, "Invalid JSON format: Step id format is invalid @RubberbandEngine"); return;
		}
		int id = sObj[RBE_JSON_STEP_Step].GetInt();

		rapidjson::Document sDoc;
		sDoc.Set(sObj);

		rapidjson::StringBuffer buffer;
		buffer.Clear();

		// Setup the Writer
		rapidjson::Writer<rapidjson::StringBuffer> writer(buffer);
		sDoc.Accept(writer);

		// Return string
		m_data->steps.insert_or_assign(id, std::pair<Step *, std::string>(nullptr, buffer.GetString()));
	}

}

void RubberbandEngine::clear(void) {
	for (auto p : m_data->points) {
		delete p.second;
	}
	m_data->points.clear();

	m_currentStep = 0;
}

void RubberbandEngine::activateStepOne(void) {
	if (m_currentStep != 0) {
		rbeAssert(0, "Data error: Current step is not 0 @RubberbandEngine");
		return;
	}
	m_currentStep = 1;
	auto stepData = m_data->steps.find(m_currentStep);
	if (stepData == m_data->steps.end()) {
		rbeAssert(0, "Data error: No step 1 found @RubberbandEngine");
		return;
	}

	if (stepData->second.first) {
		delete stepData->second.first;
	}
	
	Step * newStep = new Step;
	newStep->setupFromJson(this, stepData->second.second);
	m_data->steps.insert_or_assign(m_currentStep, std::pair<Step *, std::string>(newStep, stepData->second.second));
}

void RubberbandEngine::activateNextStep(void) {
	auto current = m_data->steps.find(m_currentStep);
	if (current != m_data->steps.end()) {
		if (current->second.first->hasPointOwnership()) {
			current->second.first->givePointOwnershipToEngine(this);
		}
	}
	m_currentStep++;
	auto nextStep = m_data->steps.find(m_currentStep);
	if (nextStep->second.first) {
		delete nextStep->second.first;
	}
	
	Step * newStep = new Step;
	newStep->setupFromJson(this, nextStep->second.second);
	m_data->steps.insert_or_assign(m_currentStep, std::pair<Step *, std::string>(newStep, nextStep->second.second));
}

bool RubberbandEngine::hasNextStep(void) {
	auto s = m_data->steps.find(m_currentStep + 1);
	return s != m_data->steps.end();
}

void RubberbandEngine::applyCurrentStep(void) {
	auto s = m_data->steps.find(m_currentStep);
	if (s == m_data->steps.end()) {
		rbeAssert(0, "Data error: No active step @RubberbandEngine"); return;
	}
	if (s->second.first) {
		if (s->second.first->hasPointOwnership()) {
			s->second.first->givePointOwnershipToEngine(this);
		}
	}
	else {
		rbeAssert(0, "Data error: Current step was not created");
	}
}
