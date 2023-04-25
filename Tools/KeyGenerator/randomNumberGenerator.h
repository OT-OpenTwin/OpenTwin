/*
 * RandomNumberGenerator.h
 *
 *  Created on: January 17, 2022
 *      Author: Jan Wagner
 */

#ifndef RANDOMGENERATOR_H
#define RANDOMGENERATOR_H

#include <stdlib.h>

class __declspec(dllexport) RandomNumberGenerator
{

public:
	virtual unsigned __int64  GetPositiveRandomInt64(unsigned __int64 maxValue) = 0;
};


#endif // !CSRANDOMGENERATORWRAPPER_H