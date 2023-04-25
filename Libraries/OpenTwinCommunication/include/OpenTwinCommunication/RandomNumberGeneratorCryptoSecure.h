/*
 * CSRandomNumberGeneratorWrapper.h
 *
 *  Created on: January 17, 2022
 *      Author: Jan Wagner
 */

#ifndef CSRANDOMNUMBERGENERATORWRAPPER_H
#define CSRANDOMNUMBERGENERATORWRAPPER_H

#include "RandomNumberGenerator.h"

class __declspec(dllexport) RandomNumberGeneratorCryptoSecure : public RandomNumberGenerator
{

public:

	virtual unsigned __int64 GetPositiveRandomInt64(unsigned __int64 maxValue) override;
};
#endif