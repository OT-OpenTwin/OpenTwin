// @otlicense
// File: CartesianMeshMaterial.h
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

class CartesianMeshMaterial
{
public:
	CartesianMeshMaterial() : isPEC(false), epsilon(1.0), mu(1.0), sigma(0.0), priority(0.0) {};
	virtual ~CartesianMeshMaterial() {};

	void setIsPEC(bool flag) { isPEC = flag; }
	void setEpsilon(double value) { epsilon = value; }
	void setMu(double value) { mu = value; }
	void setSigma(double value) { sigma = value; }

	void setPriority(double value) { priority = value; }

	bool   getIsPEC(void) { return isPEC; }
	double getEpsilon(void) { return epsilon; }
	double getMu(void) { return mu; }
	double getSigma(void) { return sigma; }

	double getPriority(void) { return priority; }

private:
	bool isPEC;
	double epsilon;
	double mu;
	double sigma;

	double priority;
};
