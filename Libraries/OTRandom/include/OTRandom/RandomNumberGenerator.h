// @otlicense

/*
 * RandomNumberGenerator.h
 *
 *  Created on: January 17, 2022
 *      Author: Jan Wagner
 */

#ifndef RANDOMNUMBERGENERATOR_H
#define RANDOMNUMBERGENERATOR_H

// OpenTwin header
#include "OTRandom/OTRandomAPIExport.h"

// std header
#include <stdlib.h>

class OT_RANDOM_API_EXPORT RandomNumberGenerator {
public:
	virtual unsigned __int64  GetPositiveRandomInt64(unsigned __int64 maxValue) = 0;
};


#endif // !RANDOMNUMBERGENERATOR_H