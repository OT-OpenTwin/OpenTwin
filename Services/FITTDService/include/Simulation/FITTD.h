#pragma once
/*
 * FDTD.h
 *
 *  Created on: 21.11.2020
 *      Author: barbarossa
 */

#ifndef FITTD_H_
#define FITTD_H_

#include <vector>
class FITTD {

public:
	virtual ~FITTD() {};

	virtual inline void UpdateSimulationValuesE() = 0;
	virtual inline void UpdateSimulationValuesH() = 0;
};


#endif /* FITTD_H_ */
