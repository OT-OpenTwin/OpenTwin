#pragma once

#include <string>
#include <stdexcept>
#include <cstdint>
#include <array>
#include <vector>
#include <map>


class FDTDConfig {
public:
	FDTDConfig();
	FDTDConfig(uint16_t timeSteps, double endCriteria, double freqStart, double freqStop, uint8_t oversampling, std::array<std::string, 6>boundaryConditions, uint8_t excitationType);
	virtual ~FDTDConfig();

	uint16_t getTimeSteps() const;
	double getEndCriteria() const;
	double getFrequencyStart() const;
	double getFrequencyStop() const;
	uint8_t getOversampling() const;
	std::array<std::string, 6> getBoundaryConditions() const;
	std::string getBoundaryConditions(size_t index) const;
	uint8_t getExcitationType() const;

	void setTimeSteps(uint16_t timeSteps);
	void setEndCriteria(double endCriteria);
	void setFrequencyStart(double freqStart);
	void setFrequencyStop(double freqStop);
	void setOverSampling(uint8_t overSampling);
	void setBoundaryCondition(std::array<std::string, 6> values);
	void setBoundaryCondition(size_t index, std::string value);


private:
	uint16_t m_timeSteps = 0;
	double m_endCriteria = 1e-5; //default = 1e-5
	double m_freqStart = 0.0;
	double m_freqStop = 0.0;
	uint8_t m_oversampling = 0;
	std::array<std::string, 6> m_boundaryConditions = { "PEC", "PEC", "PEC", "PEC", "PEC", "PEC" };
	uint8_t m_excitationType = 0; //default = 0;
};