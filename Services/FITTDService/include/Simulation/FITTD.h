// @otlicense
// File: FITTD.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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
