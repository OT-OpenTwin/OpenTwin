// @otlicense

#include "Documentation.h"

Documentation* Documentation::INSTANCE()
{
	static Documentation documentation;
	return &documentation;
}

void Documentation::AddToDocumentation(const std::string& note)
{
	_documentation += note;
}

std::string Documentation::GetFullDocumentation() const
{
	return _documentation;
}

void Documentation::ClearDocumentation()
{
	_documentation = "";
}
