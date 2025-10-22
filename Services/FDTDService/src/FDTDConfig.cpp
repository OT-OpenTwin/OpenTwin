//! @file FDTDConfig.cpp
//! @author Alexandros McCray (alexm-dev)
//! @date 08.10.2025
// ###########################################################################################################################################################################################################################################################################################################################

#include "FDTDConfig.h"

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

uint32_t FDTDConfig::getOversampling() const {
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

uint32_t FDTDConfig::getExcitationType() const {
	return static_cast<uint32_t>(m_excitation);
}

void FDTDConfig::setTimeSteps(uint32_t _timeSteps) {
	m_timeSteps = _timeSteps;
}

void FDTDConfig::setExcitationType(ExcitationType _excitationType) {
	m_excitation = _excitationType;
}

void FDTDConfig::setExcitationType(uint32_t _value) {
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

void FDTDConfig::setOverSampling(uint32_t _overSampling) {
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
	// Refresh the property information from the entity properties
	refreshPropertyInfo();

	// Defining the boundary names used for the solver XML parser
	// These must match the expected names in the XML and will be different from the GUI
	const std::array<std::string, 6> solverBoundaryNames = { "xmax", "xmin", "ymax", "ymin", "zmax", "zmin" };
	const double f0 = (m_freqStart + m_freqStop) / 2.0;
	const double fc = (m_freqStop - m_freqStart) / 2.0;

	// Now we create the FDTD root node and set its attributes
	auto FDTD = _parentElement.GetDocument()->NewElement("FDTD");
	FDTD->SetAttribute("NumberOfTimesteps", m_timeSteps);
	FDTD->SetAttribute("OverSampling", m_oversampling);
	FDTD->SetAttribute("endCriteria", m_endCriteria);
	FDTD->SetAttribute("f_max", m_freqStop);

	// Now we add the excitation nodes to the FDTD root node
	auto excitation = _parentElement.GetDocument()->NewElement("Excitation");
	excitation->SetAttribute("Type", static_cast<int>(m_excitation));
	excitation->SetAttribute("f0", f0);
	excitation->SetAttribute("fc", fc);
	FDTD->InsertEndChild(excitation);

	// Finally we add the boundary conditions to the FDTD root node
	auto boundary = _parentElement.GetDocument()->NewElement("BoundaryCond");
	for (size_t i = 0; i < solverBoundaryNames.size(); ++i) {
		boundary->SetAttribute(solverBoundaryNames[i].c_str(), m_boundaryConditions[i].c_str());
	}
	FDTD->InsertEndChild(boundary);
	return FDTD;
}

tinyxml2::XMLElement* FDTDConfig::writeCSXMeshGrid(tinyxml2::XMLElement& _parentElement) {
	ensureEntityIsSet();
	// Load the mesh grid data from the solver entity	
	CSXMeshGrid grid;
	grid.loadMeshGridDataFromEntity(m_solverEntity);

	auto CSX = _parentElement.GetDocument()->NewElement("ContinuousStructure");
	CSX->SetAttribute("CoordSystem", grid.getCoordSystem());
	CSX->SetAttribute("DeltaUnit", grid.getDeltaUnit());
	auto RectGrid = _parentElement.GetDocument()->NewElement("RectilinearGrid");
	auto xElem = _parentElement.GetDocument()->NewElement("XLines");
	auto yElem = _parentElement.GetDocument()->NewElement("YLines");
	auto zElem = _parentElement.GetDocument()->NewElement("ZLines");
	xElem->SetText((grid.vectorToString(grid.getXLines())).c_str());
	yElem->SetText((grid.vectorToString(grid.getYLines())).c_str());
	zElem->SetText((grid.vectorToString(grid.getZLines())).c_str());
	RectGrid->InsertEndChild(xElem);
	RectGrid->InsertEndChild(yElem);
	RectGrid->InsertEndChild(zElem);
	CSX->InsertEndChild(RectGrid);
	return CSX;
}

void FDTDConfig::readTimestepInfo() {
	ensureEntityIsSet();
	m_timeSteps = PropertyHelper::getIntegerPropertyValue(m_solverEntity, "Timesteps", "Simulation Settings");
}

void FDTDConfig::readExcitationTypeInfo() {
	ensureEntityIsSet();
	std::string excitation = PropertyHelper::getSelectionPropertyValue(m_solverEntity, "Excitation Type", "Simulation Settings");
	if (excitation == "Gauss Excitation") {
		m_excitation = ExcitationType::GAUSSIAN;
	}
	else if (excitation == "Sinus Excitation") {
		m_excitation = ExcitationType::SINUSOIDAL;
	}
	else {
		throw std::invalid_argument("[Excitation Type] Invalid excitation type! Must be 'Gaussian' 'Sinusoidal'");
	}
}

void FDTDConfig::readEndCriteriaInfo() {
	ensureEntityIsSet();
	m_endCriteria = PropertyHelper::getDoublePropertyValue(m_solverEntity, "End Criteria", "Simulation Settings");
}

void FDTDConfig::readFrequencyStartInfo() {
	ensureEntityIsSet();
	m_freqStart = PropertyHelper::getDoublePropertyValue(m_solverEntity, "Start Frequency", "Frequency");
}

void FDTDConfig::readFrequencyStopInfo() {
	ensureEntityIsSet();
	m_freqStop = PropertyHelper::getDoublePropertyValue(m_solverEntity, "End Frequency", "Frequency");
}

void FDTDConfig::readOversamplingInfo() {
	ensureEntityIsSet();
	m_oversampling = PropertyHelper::getIntegerPropertyValue(m_solverEntity, "Oversampling", "Simulation Settings");
}

void FDTDConfig::readBoundaryConditions() {
	ensureEntityIsSet();
	for (size_t i = 0; i < m_boundaryNames.size(); ++i) {
		auto value = PropertyHelper::getSelectionPropertyValue(m_solverEntity, m_boundaryNames[i], "Boundary Conditions");
		if (value.empty()) {
			value = "PEC"; // default to PEC
		}
		m_boundaryConditions[i] = value;
	}
}

void FDTDConfig::refreshPropertyInfo() {
	ensureEntityIsSet();

	// Helper lambda function to safely read a property and log errors
	//! @brief This lambda function attempts to read a property using the provided read function.
	//! @brief If an exception occurs during the read, it logs the error and sets the target value to a default.
	//! @param _readFunc The function that reads the property.
	//! @param _propertyName The name of the property being read (for logging purposes).
	//! @param _targetValue The variable where the read value will be stored.
	//! @param _defaultValue The default value to use if reading fails.
	auto safeRead = [&](auto _readFunc, const std::string& _propertyName, auto& _targetValue, auto _defaultValue) {
		try {
			_readFunc();
		}
		catch (const std::exception& e) {
			_targetValue = _defaultValue;
			std::string msg = "[FDTDConfig] Error reading property '" + _propertyName + "': " + e.what();
			std::string msgDefaults = "Defaulting to: ";

			if (_propertyName == "Boundary Conditions") {
				msgDefaults = "Defaulting to: PEC on all sides" ;
			}
			else if constexpr (std::is_same_v<decltype(_defaultValue), std::string>) {
				msgDefaults += _defaultValue;
			}
			else if constexpr (std::is_arithmetic_v<decltype(_defaultValue)>) {
				msgDefaults += std::to_string(_defaultValue);
			}
			else if constexpr (std::is_enum_v<decltype(_defaultValue)>) {
				switch (_defaultValue) {
					case ExcitationType::GAUSSIAN: msgDefaults += "Excitation Type: Gaussian"; break;
					case ExcitationType::SINUSOIDAL: msgDefaults += "Excitation Type: Sinusoidal"; break;
					default: msgDefaults += "Unknown Excitation Type"; break;
				}
			}
			OT_LOG_E(msg);
			OT_LOG_E(msgDefaults + "\n");
		}
	};

	// Now read each property safely and log errors, and sets default values if reading fails
	safeRead([&]() { readTimestepInfo(); }, "Timesteps", m_timeSteps, 1000u);
	safeRead([&]() { readExcitationTypeInfo(); }, "Excitation Type", m_excitation, ExcitationType::GAUSSIAN);
	safeRead([&]() { readEndCriteriaInfo(); }, "End Criteria", m_endCriteria, 1e-6);
	safeRead([&]() { readFrequencyStartInfo(); }, "Start Frequency", m_freqStart, 1000.0);
	safeRead([&]() { readFrequencyStopInfo(); }, "End Frequency", m_freqStop, 2000.0);
	safeRead([&]() { readOversamplingInfo(); }, "Oversampling", m_oversampling, 1);
	safeRead([&]() { readBoundaryConditions(); }, "Boundary Conditions", m_boundaryConditions, std::array<std::string, 6>{"PEC", "PEC", "PEC", "PEC", "PEC", "PEC"});

	if (m_freqStart >= m_freqStop) {
		OT_LOG_W("[FDTDConfig] Start Frequency must be less than End Frequency! Adjusting to default values: Start Frequency = 1000.0, End Frequency = 2000.0\n");
		m_freqStart = 1000.0;
		m_freqStop = 2000.0;
	}
}

void FDTDConfig::addToXML(tinyxml2::XMLDocument& _doc) {
	auto declaration = _doc.NewDeclaration("xml version=\"1.0\" encoding=\"utf-8\"");
	_doc.InsertFirstChild(declaration);
	auto root = _doc.NewElement("openEMS");
	_doc.InsertEndChild(root);
	auto FDTD = writeFDTD(*root);
	root->InsertEndChild(FDTD);
	auto CSX = writeCSXMeshGrid(*root);
	root->InsertEndChild(CSX);
}