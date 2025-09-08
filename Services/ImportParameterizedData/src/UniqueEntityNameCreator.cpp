#include "UniqueEntityNameCreator.h"
std::string UniqueEntityNameCreator::create(const std::string& _entityPath)
{
	int count = 1;
	std::string fullFileName = _entityPath + "/" + m_entityShortNameBase;

	while (std::find(m_alreadyTakenNames.begin(), m_alreadyTakenNames.end(), fullFileName) != m_alreadyTakenNames.end())
	{
		fullFileName = _entityPath + "/" + m_entityShortNameBase + "_" + std::to_string(count);
		count++;
	}
	m_alreadyTakenNames.push_back(fullFileName);
	return fullFileName;
}

void UniqueEntityNameCreator::setAlreadyTakenNames(const std::list<std::string>& _alreadyTakenNames)
{
	m_alreadyTakenNames = _alreadyTakenNames;
}
