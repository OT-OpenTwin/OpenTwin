#pragma once
#ifndef IBase
#include <string>
using namespace std;
class IBase
{
protected:
	std::string scriptName;

public:
	virtual int setDataset(std::list<std::vector<double>> d) = 0;
	virtual int train() = 0;
	virtual int predict(std::list<std::vector<double>> d) = 0;
	virtual void getModel() = 0;

	IBase(const std::string scriptName) {
		this->scriptName.assign(scriptName);
	}
};

#endif
