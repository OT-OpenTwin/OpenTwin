#include "FixtureModelState.h"

std::string FixtureModelState::getPrecedingVersion(const std::string& _currentVersion)
{
	ModelState state(0,0);
	return state.getPrecedingVersion(_currentVersion);
}

std::string FixtureModelState::getProceedingVersion(const std::string& _currentVersion, const std::string& _targetVersion)
{
	ModelState state(0, 0);
	return state.getProceedingVersion(_currentVersion, _targetVersion);
}
