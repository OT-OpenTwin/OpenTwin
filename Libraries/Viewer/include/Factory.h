#pragma once

#include "ClassFactory.h"

class Factory
{
public:
	Factory() {};
	~Factory() {};

	static ClassFactory& getClassFactory() { return classFactory; }

private:
	static ClassFactory classFactory;
};
