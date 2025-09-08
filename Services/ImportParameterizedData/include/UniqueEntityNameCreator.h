#pragma once
#include <list>
#include <string>

class UniqueEntityNameCreator
{
public:
	UniqueEntityNameCreator() = default;

	std::string create(const std::string& _entityPath);
	
	void setNameBase(const std::string& _entityShortNameBase)
	{
		m_entityShortNameBase = _entityShortNameBase;
	}

	const std::string& getNameBase() 
	{
		return m_entityShortNameBase;
	}

	void setAlreadyTakenNames(const std::list<std::string>& _alreadyTakenNames);

	void clearAlreadyTakenNames() 
	{ 
		m_alreadyTakenNames.clear(); 
	}

private:
	std::list<std::string> m_alreadyTakenNames;
	std::string m_entityShortNameBase;
};