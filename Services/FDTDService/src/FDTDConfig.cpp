//! @file FDTDConfig.cpp
//! @author Alexandros McCray (alexm-dev)
//! @date 08.10.2025
// ###########################################################################################################################################################################################################################################################################################################################

#include "FDTDConfig.h"
#include "FDTDPropertyReader.h"

FDTDConfig::FDTDConfig()
{
}

FDTDConfig::~FDTDConfig()
{
}

uint32_t FDTDConfig::getTimeSteps() const {
	return m_timeSteps;
}

double FDTDConfig::getEndCriteria() const {
	return m_endCriteria;
}

double FDTDConfig::getFrequencyStart() const {
	return m_freqStart;
}

double FDTDConfig::getFrequencyStop() const {
	return m_freqStop;
}

uint8_t FDTDConfig::getOversampling() const {
	return m_oversampling;
}

std::array<std::string, 6> FDTDConfig::getBoundaryConditions() const {
	return m_boundaryConditions;
}

std::string FDTDConfig::getBoundaryConditions(size_t _index) const {
	if (_index >= m_boundaryConditions.size()) {
		throw std::out_of_range("[Boundary Conditions] Index out of range!");
	}
	return m_boundaryConditions[_index];
}

uint8_t FDTDConfig::getExcitationType() const {
	return static_cast<uint8_t>(m_excitation);
}

void FDTDConfig::setTimeSteps(uint32_t _timeSteps) {
	m_timeSteps = _timeSteps;
}

void FDTDConfig::setExcitationType(ExcitationType _excitationType) {
	m_excitation = _excitationType;
}

void FDTDConfig::setExcitationType(uint8_t _value) {
	if (_value > 2 || _value < 0) {
		throw std::invalid_argument("[Excitation Type] Invalid excitation type! Must be 0 (Gaussian), 1 (Sinusoidal), or 2 (Ramp)");
	}
	m_excitation = static_cast<ExcitationType>(_value);
}

void FDTDConfig::setEndCriteria(double _endCriteria) {
	m_endCriteria = _endCriteria;
}

void FDTDConfig::setFrequencyStart(double _freqStart) {
	m_freqStart = _freqStart;
}

void FDTDConfig::setFrequencyStop(double _freqStop) {
	m_freqStop = _freqStop;
}

void FDTDConfig::setOverSampling(uint8_t _overSampling) {
	m_oversampling = _overSampling;
}

void FDTDConfig::setBoundaryCondition(const std::array<std::string, 6>& _values) {
	for (const auto& val : _values) {
		if (std::find(m_boundaryConditionTypes.begin(), m_boundaryConditionTypes.end(), val) == m_boundaryConditionTypes.end()) {
			throw std::invalid_argument(std::string("[Boundary Condition] Invalid boundary condition! " + val));
		}
	}
	m_boundaryConditions = _values;
}

void FDTDConfig::setBoundaryCondition(size_t _index, const std::string& _value) {
	if (std::find(m_boundaryConditionTypes.begin(), m_boundaryConditionTypes.end(), _value) == m_boundaryConditionTypes.end()) {
		throw std::invalid_argument(std::string("[Boundary Condition] Invalid boundary condition! " + _value));
	}
	if (_index >= m_boundaryConditions.size()) {
		throw std::out_of_range(std::string("[Boundary Condition] Index out of range"));
	}
	m_boundaryConditions[_index] = _value;
}

void FDTDConfig::setFromEntity(EntityBase* _solverEntity) {
	if(!_solverEntity) {
		throw std::invalid_argument("[FDTDConfig] Solver entity pointer is null!");
	}
	m_solverEntity = _solverEntity;
}

void FDTDConfig::ensureEntityIsSet() const {
	if (m_solverEntity == nullptr) {
		throw std::runtime_error("[FDTDConfig] Solver entity is not set. Please set the solver entity before accessing configuration values.");
	}
}

tinyxml2::XMLElement* FDTDConfig::writeFDTD(tinyxml2::XMLElement& _parentElement) {
	// Define the boundary names used for the solver XML parser
	// These must match the expected names in the XML and will be different from the GUI
	const std::array<std::string, 6> solverBoundaryNames = { "xmax", "xmin", "ymax", "ymin", "zmax", "zmin" };
	m_freqStart = readFrequencyStartInfo();
	m_freqStop = readFrequencyStopInfo();
	const double f0 = (m_freqStart + m_freqStop) / 2.0;
	const double fc = (m_freqStop - m_freqStart) / 2.0;

	// Now we create the FDTD root node and set its attributes
	auto FDTD = _parentElement.GetDocument()->NewElement("FDTD");
	FDTD->SetAttribute("NumberOfTimesteps", readTimestepInfo());
	FDTD->SetAttribute("OverSampling", readOversamplingInfo());
	FDTD->SetAttribute("endCriteria", readEndCriteriaInfo());
	FDTD->SetAttribute("f_max", m_freqStop);

	// Now we add the excitation nodes to the FDTD root node
	auto excitation = _parentElement.GetDocument()->NewElement("Excitation");
	excitation->SetAttribute("Type", static_cast<int>(readExcitationTypeInfo()));
	excitation->SetAttribute("f0", f0);
	excitation->SetAttribute("fc", fc);
	FDTD->InsertEndChild(excitation);

	// Finally we add the boundary conditions to the FDTD root node
	auto boundary = _parentElement.GetDocument()->NewElement("BoundaryCond");
	const auto boundaryConditions = readBoundaryConditions();
	for (size_t i = 0; i < solverBoundaryNames.size(); ++i) {
		boundary->SetAttribute(solverBoundaryNames[i].c_str(), boundaryConditions[i].c_str());
	}
	FDTD->InsertEndChild(boundary);
	return FDTD;
}

uint32_t FDTDConfig::readTimestepInfo() {
	ensureEntityIsSet();
	return readEntityPropertiesInfo<uint32_t>(m_solverEntity, "Simulation Settings", m_simulationSettingsProperties, "Timesteps", true);
}

uint8_t FDTDConfig::readExcitationTypeInfo() {
	std::string excitation = readEntityPropertiesInfo<std::string> (m_solverEntity, "Simulation Settings", m_simulationSettingsProperties, "Excitation type", true);
	if (excitation == "Gauss Excitation") {
		m_excitation = ExcitationType::GAUSSIAN;
	}
	else if (excitation == "Sinus Excitation") {
		m_excitation = ExcitationType::SINUSOIDAL;
	}
	else {
		throw std::invalid_argument("[Excitation Type] Invalid excitation type! Must be 'Gaussian' 'Sinusoidal'");
	}
	return static_cast<uint8_t>(m_excitation);
}

double FDTDConfig::readEndCriteriaInfo() {
	ensureEntityIsSet();
	return readEntityPropertiesInfo<double>(m_solverEntity, "Simulation Settings", m_simulationSettingsProperties, "End Criteria", true);
}

double FDTDConfig::readFrequencyStartInfo() {
	ensureEntityIsSet();
	return readEntityPropertiesInfo<double>(m_solverEntity, "Frequency", m_frequencyProperties, "Start Frequency", true);
}

double FDTDConfig::readFrequencyStopInfo() {
	ensureEntityIsSet();
	return readEntityPropertiesInfo<double>(m_solverEntity, "Frequency", m_frequencyProperties, "End Frequency", true);
}

uint16_t FDTDConfig::readOversamplingInfo() {
	ensureEntityIsSet();
	return readEntityPropertiesInfo<uint16_t>(m_solverEntity, "Simulation Settings", m_simulationSettingsProperties, "Oversampling", true);
}

std::array<std::string, 6> FDTDConfig::readBoundaryConditions() {
	ensureEntityIsSet();
	ot::ModelServiceAPI::getEntityProperties(m_solverEntity->getName(), true, "Boundary Conditions", m_boundaryConditionProperties);
	for (size_t i = 0; i < m_boundaryNames.size(); ++i) {
		auto value = readEntityPropertiesInfo<std::string>(m_solverEntity, "Boundary Conditions", m_boundaryConditionProperties, m_boundaryNames[i], false);
		if (value.empty()) {
			value = "PEC"; // default to PEC
		}
		m_boundaryConditions[i] = value;
	}
	return m_boundaryConditions;
}

void FDTDConfig::addToXML(tinyxml2::XMLDocument& _doc) {
	auto declaration = _doc.NewDeclaration("xml version=\"1.0\" encoding=\"utf-8\"");
	_doc.InsertFirstChild(declaration);
	auto root = _doc.NewElement("openEMS");
	_doc.InsertEndChild(root);
	auto FDTD = writeFDTD(*root);
	root->InsertEndChild(FDTD);
}