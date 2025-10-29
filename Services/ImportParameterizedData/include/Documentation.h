// @otlicense

#pragma once
#include <string>

class Documentation
{
public:
	static Documentation* INSTANCE();

	void AddToDocumentation(const std::string& note);
	std::string GetFullDocumentation()const;
	void ClearDocumentation();
private:
	Documentation() {};
	std::string _documentation;

};
