/*
 * RandomNumberGeneratorCryptoSecure.h
 *
 *  Created on: January 17, 2022
 *      Author: Jan Wagner
 */

#ifndef RANDOMNUMBERGENERATORCRYPTOSECURE_H
#define RANDOMNUMBERGENERATORCRYPTOSECURE_H

#include "RandomNumberGenerator.h"

class OT_RANDOM_API_EXPORT RandomNumberGeneratorCryptoSecure : public RandomNumberGenerator {
public:
	virtual unsigned __int64 GetPositiveRandomInt64(unsigned __int64 maxValue) override;
};
#endif // !RANDOMNUMBERGENERATORCRYPTOSECURE_H