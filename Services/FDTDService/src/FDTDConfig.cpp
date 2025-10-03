#include "FDTDConfig.h"

FDTDConfig::FDTDConfig()
{
}

FDTDConfig::FDTDConfig(uint16_t timeSteps, double endCriteria, double freqStart, double freqStop, uint8_t oversampling, std::array<std::string, 6>boundaryConditions, uint8_t excitationType)
	: m_timeSteps(timeSteps),
	m_endCriteria(endCriteria),
	m_freqStart(freqStart),
	m_freqStop(freqStop),
	m_oversampling(oversampling),
	m_boundaryConditions(boundaryConditions),
	m_excitationType(excitationType)
{

}

FDTDConfig::~FDTDConfig()
{
}

uint16_t FDTDConfig::getTimeSteps() const {
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

std::string FDTDConfig::getBoundaryConditions(size_t index) const {
	if (index >= m_boundaryConditions.size()) {
		throw std::out_of_range("[Boundary Conditions] Index out of range!");
	}
	return m_boundaryConditions[index];
}

uint8_t FDTDConfig::getExcitationType() const {
	return m_excitationType;
}

void FDTDConfig::setTimeSteps(uint16_t timeSteps) {
	m_timeSteps = timeSteps;
}

void FDTDConfig::setEndCriteria(double endCriteria) {
	m_endCriteria = endCriteria;
}

void FDTDConfig::setFrequencyStart(double freqStart) {
	m_freqStart = freqStart;
}

void FDTDConfig::setFrequencyStop(double freqStop) {
	m_freqStop = freqStop;
}

void FDTDConfig::setOverSampling(uint8_t overSampling) {
	m_oversampling = overSampling;
}

void FDTDConfig::setBoundaryCondition(std::array<std::string, 6> values) {
	const std::array<std::string, 4> bcNames = { "PEC", "PMC", "MUR", "PML_8" };
	for (const auto& val : values) {
		if (std::find(bcNames.begin(), bcNames.end(), val) == bcNames.end()) {
			throw std::invalid_argument(std::string("[Boundary Condition] Invalid boundary condition! " + val));
		}
	}
	m_boundaryConditions = values;
}

void FDTDConfig::setBoundaryCondition(size_t index, std::string value) {
	const std::array<std::string, 4> bcNames = { "PEC", "PMC", "MUR", "PML_8" };
	if (std::find(bcNames.begin(), bcNames.end(), value) == bcNames.end()) {
		throw std::invalid_argument(std::string("[Boundary Condition] Invalid boundary condition! " + value));
	}
	if (index >= m_boundaryConditions.size()) {
		throw std::out_of_range(std::string("[Boundary Condition] Index out of range"));
	}
	m_boundaryConditions[index] = value;
}