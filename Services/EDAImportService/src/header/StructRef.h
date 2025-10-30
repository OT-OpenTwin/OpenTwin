// @otlicense
// File: StructRef.h
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

#ifndef STRUCTREF_H_
#define STRUCTREF_H_

#include <string>


// This class represents a gds2 structure reference. 
// It contains the name of the referenced structure and the coordinates where the "copy" should be placed
class StructRef{
private:
	std::string name;
	std::pair<int, int> coordinates;

public:
	StructRef(std::string name, std::pair<int, int> coords);

	std::string getName();
	std::pair<int, int> getCoordinates();

	void setName(std::string name);
	void setCoordinates(std::pair<int, int> coords);
};

#endif

