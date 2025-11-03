// @otlicense
// File: Gds2Import.cpp
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

#include "Gds2Import.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <cassert>

// macros which define tags in a gds2 file
#define BGNSTR 0x0502 // begin of a structure
#define STRNAME 0x0606 // name of a structure
#define ENDSTR 0x0700 // end of a structure
#define BOUNDARY 0x0800 // begin of a boundary / polygon
#define SREF 0x0A00 // begin of an structure reference element, a copy of the referenced structure will be placed at the given coordinates
#define SNAME 0x1206 // name of the referenced structure
#define LAYER 0x0D02 // begin of a layer
#define PATH 0x0900 // begin of path element
#define PATHTYPE 0x2102 // type of the path element, 3 different path types ( type 0 is default type if none is specified)
#define WIDTH 0x0f03 // width of the path
#define XY 0x1003  // list of coordinates, each coordinate is a 4 byte long singed integer
#define BOX 0x2D00



Gds2Import::Gds2Import(std::string filePath) {
	data = readFileData(filePath);
	filesize = data.size();
}

inline void Gds2Import::checkSize(unsigned int x) {
	assert(x < filesize);
}

// takes 2 std::byte objects, add them together and converts them into an int (unsigned)
int Gds2Import::getWordInt(std::byte a, std::byte b) {
	return (int)a << 8 | (int)b;
}

// gds2 coordinates are 4byte long, those 4 bytes are converted to a signed int
int Gds2Import::getCoordinate(std::byte a, std::byte b, std::byte c, std::byte d) {
	return static_cast<int>((static_cast<uint32_t>(a) << 24) | (static_cast<uint32_t>(b) << 16) | static_cast<uint32_t>(c) << 8 | static_cast<uint32_t>(d));
}

int Gds2Import::getElemSize(std::byte a, std::byte b) {
	return getWordInt(a, b) / 2;
}

std::string Gds2Import::getStructName(uint32_t rp) {
	int size = getWordInt(data[rp - 2], data[rp - 1]);
	std::string structName(size, '\0');
	rp += 2; // the received read position is at the first byte which denotes a STRNAME or SNAME (each 2 byte long). To read the string the program starts 2 bytes later at the first byte which represents a char
	size -= 4; // size is reduced because it contains 2 bytes which contain the entry size and 2 bytes which are the STRNAME or SNAME tag
	int strIt = 0;


	for (strIt; strIt < size; strIt++) {
		checkSize(rp); // assertion which checks if the filedata has coordIt bytes

		structName[strIt] = static_cast<char>(data[rp]);
		rp++;
	}
	return structName;
}

std::vector<std::pair<int, int>> Gds2Import::getXY(uint32_t rp) {
	unsigned int entrySize = getWordInt(data[rp - 2], data[rp - 1]) - 4; // the word before the start of the xy coordinates denotes the number of coordinates; 4 is subtracted because 2 bytes are occupied by the XY-tag and 2 by the entrysize
	rp += 2; // Position of the byte which is read from the filedata
	std::vector<std::pair<int, int>> coordinates = {};

	checkSize(rp); // assertion which checks if the filedata has rp bytes

	//Print information about coordinates
	//std::cout << "entry size: " << entrySize << " bytes" << std::endl;
	//std::cout << "Position: " << rp << std::endl;

	// extract coordinates of the boundary/polygon
	// iterates over the coordinates
	// entrySize is devided by 4, because each coordinate is 4 byte long and than by 2, because the coordinates are extracted in pairs
	for (unsigned int i = 0; i < (entrySize / 4) / 2; i++) {
		//std::cout << "Position: " << position << std::endl;
		checkSize(rp + 7); // assertion which checks if the filedata has rp bytes

		int x = getCoordinate(data[rp], data[rp + 1], data[rp + 2], data[rp + 3]);
		int y = getCoordinate(data[rp + 4], data[rp + 5], data[rp + 6], data[rp + 7]);

		//std::cout << "xy: (" << x << ", " << y << ")" << std::endl;

		std::pair<int, int> xy = std::make_pair(x, y);
		coordinates.push_back(xy);
		rp += 8;
	}

	return coordinates;
}

StructRef Gds2Import::getStructRef(uint32_t& rp) {
	bool isStructRef = true;
	std::string strRefName = "";
	std::pair<int, int> coordinates = std::make_pair(0, 0);

	
	while (isStructRef && rp < filesize) {
		int elementSize = getWordInt(data[rp - 2], data[rp - 1]);
		int tag = getWordInt(data[rp], data[rp + 1]);

		if (tag == SNAME) {
			strRefName = getStructName(rp);
			std::cout << "Structure Reference: " << strRefName << std::endl;
		}
		else if (tag == XY) {
			coordinates = getXY(rp)[0]; // strRef only contain one set of coordinates at which a copy of the referenced structure is placed
			isStructRef = false;
		}

		rp += elementSize;
	}

	StructRef strRef = StructRef(strRefName, coordinates);
	std::cout << strRef.getCoordinates().first << " " << strRef.getCoordinates().second << std::endl;


	return StructRef(strRefName, coordinates);

}

// This method generates an polygon object which is based on a gds2 boundary
MyPolygon Gds2Import::getPolygon(uint32_t& rp) {
	unsigned int layer = 0;
	bool isBoundary = true;
	bool containsLayer = false;
	MyPolygon poly = MyPolygon();


	while (isBoundary) {
		int elementSize = getWordInt(data[rp - 2], data[rp - 1]);
		int tag = getWordInt(data[rp], data[rp + 1]);

		if (tag == LAYER) { // 0D02 denotes a layer 
			checkSize(rp + 3); // assertion which checks if the filedata has i+3 bytes
			layer = (unsigned int)data[rp + 3];
			//std::cout << "Layer: " << (unsigned int)data[rp + 3] << std::endl;
		}
		else if(tag == XY){
			std::vector<std::pair<int, int>> coordinates = getXY(rp);

			isBoundary = false;

			poly.setCoordinates(coordinates);
			poly.setLayer(layer);

		}

		rp += elementSize;

	}

	return poly;
}

Gds2Path Gds2Import::getPath(uint32_t& rp) {
	bool isPath = true;
	Gds2Path newPath = Gds2Path();

	while (isPath && rp < filesize - 1) {

		checkSize(rp + 1); // assertion which checks if the filedata has i+1 bytes

		int elementSize = getWordInt(data[rp - 2], data[rp - 1]);
		int tag = getWordInt(data[rp], data[rp + 1]);

		// rp iterates through a boundary
		if (tag == LAYER) { // 0D02 denotes a layer 
			checkSize(rp + 3); // assertion which checks if the filedata has i+3 bytes
			newPath.setLayer((unsigned int)data[rp + 3]);
			//std::cout << "Layer: " << (unsigned int)data[rp + 3] << std::endl;
		}

		else if (tag == PATHTYPE) {
			newPath.setPathType((unsigned int)data[rp + 3]);
		}

		else if (tag == WIDTH) {
			newPath.setWidth(getCoordinate(data[rp + 2], data[rp + 3], data[rp + 4], data[rp + 5]));
		}

		else if (tag == XY) { // 1003 denotes the xy coordinates of the path
			newPath.setCoordinates(getXY(rp));

			isPath = false;
		}

		rp += elementSize;
	}

	return newPath;
}

Gds2Structure Gds2Import::getStruct(uint32_t& rp){
	bool isStruct = true;
	int strNameSize = getWordInt(data[rp - 2], data[rp - 1]);
	std::string structName = getStructName(rp);
	Gds2Structure newStruct(structName); // create empty structure
	std::vector<MyPolygon> structPolys = {};
	rp += strNameSize;
	std::vector<Gds2Path> paths = {};

	while (rp < filesize - 2 && isStruct) {
		int elementSize = getWordInt(data[rp - 2], data[rp - 1]);
		int tag = getWordInt(data[rp], data[rp + 1]);

		switch (tag) {
		case BOX:
		case BOUNDARY: {
			MyPolygon newPol = getPolygon(rp);
			structPolys.push_back(newPol);
			break;
		}
		case SREF: {
			StructRef newStrRef = getStructRef(rp);
			newStruct.addStuctRef(newStrRef);
			break;
		}	
		case PATH: {
			Gds2Path newPath = getPath(rp);
			paths.push_back(newPath);
			break;
		}
		case ENDSTR: {
			isStruct = false;
			rp += elementSize;
			break;
		}
		default:
			rp += elementSize;
			break;
		}
		
	}


	newStruct.setPaths(paths);
	newStruct.setPolygons(structPolys);

	return newStruct;
}



std::vector<MyPolygon> Gds2Import::getStrRefPolygons(Gds2Structure& structure, std::map<std::string, Gds2Structure>& structMap, std::vector<MyPolygon>& polygons) {


	for (auto& strRef : structure.getStructRef()) {
		std::pair<int, int> placement = strRef.getCoordinates();
		Gds2Structure referencedStruct = structMap[strRef.getName()];
		std::vector<MyPolygon> refPoly = referencedStruct.getPolygons();

		if (referencedStruct.getStructRef().size() > 0) {
			getStrRefPolygons(referencedStruct, structMap, polygons);
			//! @todo Process return value
		}
		for (auto& polygon : refPoly) {
			std::vector<std::pair<int, int>> coords = polygon.getCoordinates();
			std::vector<std::pair<int, int>> newCoords = {};

			for (auto& xy : coords) {
				xy = std::make_pair(xy.first + placement.first, xy.second + placement.second);
				newCoords.push_back(xy);
			}
			polygon.setCoordinates(newCoords);
			polygons.push_back(polygon);

		}
	}

	return polygons;
}

std::pair<std::vector<MyPolygon>, std::vector<Gds2Path>> Gds2Import::getStrRefData(Gds2Structure& structure, std::map<std::string, Gds2Structure>& structMap, std::pair<std::vector<MyPolygon>, std::vector<Gds2Path>>& strRefData) {


	for (auto& strRef : structure.getStructRef()) {
		std::pair<int, int> placement = strRef.getCoordinates();
		Gds2Structure referencedStruct = structMap[strRef.getName()];
		std::vector<MyPolygon> refPoly = referencedStruct.getPolygons();
		std::vector<Gds2Path> refPaths = referencedStruct.getPaths();

		if (referencedStruct.getStructRef().size() > 0) {
			getStrRefData(referencedStruct, structMap, strRefData);
			//! @todo Process return value
		}
		for (auto& polygon : refPoly) {
			std::vector<std::pair<int, int>> coords = polygon.getCoordinates();
			std::vector<std::pair<int, int>> newCoords = {};

			for (auto& xy : coords) {
				xy = std::make_pair(xy.first + placement.first, xy.second + placement.second);
				newCoords.push_back(xy);
			}
			polygon.setCoordinates(newCoords);
			strRefData.first.push_back(polygon);

		}

		for (auto& path : refPaths) {
			std::vector<std::pair<int, int>> coords = path.getCoordinates();
			std::vector<std::pair<int, int>> newCoords = {};

			for (auto& xy : coords) {
				xy = std::make_pair(xy.first + placement.first, xy.second + placement.second);
				newCoords.push_back(xy);
			}

			path.setCoordinates(newCoords);
			strRefData.second.push_back(path);
		}


	}

	return strRefData;
}



std::vector<MyPolygon> Gds2Import::getPolygons() {
	std::vector<Gds2Structure> structures = getModelData();
	std::vector<MyPolygon> polygons = {};
	std::map<std::string, Gds2Structure> structMap = {};

	for (auto& structure : structures) {
		structMap[structure.getName()] = structure;
	}

	for (auto& structure : structures) {
		std::cout << "Structure: " << structure.getName() << " Polygons: " << structure.getPolygons().size() << " StrRef: " << structure.getStructRef().size() << std::endl;
		std::vector<MyPolygon> structPolys = structure.getPolygons();
		std::vector<MyPolygon> emptyVec = {};

		if (structure.getStructRef().size() > 0) {
			std::vector<MyPolygon> strRefPolys = getStrRefPolygons(structure, structMap, emptyVec);
			structPolys.insert(structPolys.end(), strRefPolys.begin(), strRefPolys.end());
		}

		std::cout << "Structure: " << structure.getName() << " Polygons: " << structPolys.size() << " StrRef: " << structure.getStructRef().size() << std::endl;

		if (structure.getPaths().size() > 0) {
			std::vector<MyPolygon> paths = {};

			for (auto& path : structure.getPaths()) {
				paths.push_back(path.pathToPolygon());
			}

			structPolys.insert(structPolys.end(), paths.begin(), paths.end());
		}


		std::cout << "Structure: " << structure.getName() << " Polygons: " << structPolys.size() << " StrRef: " << structure.getStructRef().size() <<std::endl;

		polygons.insert(polygons.end(), structPolys.begin(), structPolys.end());
		
	}

	return polygons;
}

std::pair<std::vector<MyPolygon>, std::vector<Gds2Path>> Gds2Import::getStructData(std::vector<Gds2Structure>& structures){
	
	std::vector<MyPolygon> polygons = {};
	std::map<std::string, Gds2Structure> structMap = {};
	std::vector<Gds2Path> paths = {};

	for (auto& structure : structures) {
		structMap[structure.getName()] = structure;
	}

	for (auto& structure : structures) {
		
		std::vector<MyPolygon> structPolys = structure.getPolygons();
		std::pair<std::vector<MyPolygon>,std::vector<Gds2Path>> emptyVec = {};

		if (structure.getStructRef().size() > 0) {
			std::pair<std::vector<MyPolygon>, std::vector<Gds2Path>> strRefData = getStrRefData(structure, structMap, emptyVec);
			structPolys.insert(structPolys.end(), strRefData.first.begin(), strRefData.first.end());
			paths.insert(paths.end(), strRefData.second.begin(), strRefData.second.end());
		}
		

		if (structure.getPaths().size() > 0) {
			std::vector<Gds2Path> structPaths = structure.getPaths();

			paths.insert(paths.end(), structPaths.begin(), structPaths.end());
		}


		std::cout << "Structure: " << structure.getName() << " Polygons: " << structPolys.size() << " StrRef: " << structure.getStructRef().size() << " Paths: " << paths.size() << std::endl;

		polygons.insert(polygons.end(), structPolys.begin(), structPolys.end());

	}

	std::pair<std::vector<MyPolygon>, std::vector<Gds2Path>> structData = std::make_pair(polygons, paths);

	return structData;
}

// This methods parses the file by starting at the first tag and than skips the given tag size. When the tag is included in the tag map the according data is extracted from the gds2 file
std::vector<Gds2Structure> Gds2Import::getModelData() {
	std::vector<Gds2Structure> structures = {};
	std::vector<MyPolygon> polygons = {};
	std::vector<Gds2Path> paths = {};
	std::vector<StructRef> structRefs = {};
	std::map<int, int> tags;
	tags[STRNAME] = 0; 
	tags[BOUNDARY] = 0;
	tags[SREF] = 0;
	tags[PATH] = 0;

	uint32_t i = 2; // i is 2 because the first two bytes of a gds2 file denote the size of the first tag (header-tag)

	while (i < filesize - 2) {
		int tag = getWordInt(data[i], data[i + 1]);
		int elementSize = getWordInt(data[i - 2], data[i - 1]);

		// Checks if the current tag contains important data. 
		// If it is the case it will read the data else it will jump to the next tag, based on the given element Size
		if (tags.count(tag) == 1) {
			

			switch (tag) {
			
			case STRNAME: {
				Gds2Structure newStruct = getStruct(i);
				structures.push_back(newStruct);
				break;
			}

			case BOUNDARY: {
				MyPolygon newPol = getPolygon(i);
				polygons.push_back(newPol);
				break;
			}

			case SREF: {
				StructRef newStrRef = getStructRef(i);
				structRefs.push_back(newStrRef);
				break;
			}


			case PATH: {
				Gds2Path newPath = getPath(i);
				paths.push_back(newPath);
				break;
			}
			}
		}
		else {
			if (elementSize == 0) {
				i++;
			}

			i += elementSize;
		}
	}

	// Print which elemnets the file contains
	std::cout << "Structures: " << structures.size() << std::endl;
	std::cout << "Polygons: " << polygons.size() << std::endl;
	std::cout << "SREF: " << structRefs.size() << std::endl;
	std::cout << "Paths: " << paths.size() << std::endl;

	return structures;
}

std::pair<std::vector<MyPolygon>, std::vector<Gds2Path>> Gds2Import::modelData() {
	std::vector<Gds2Structure> structures = {};
	std::vector<MyPolygon> polygons = {};
	std::vector<Gds2Path> paths = {};
	std::vector<StructRef> structRefs = {};
	std::map<std::string, Gds2Structure> structMap = {}; // map to track existing structures, important to read StrRefs
	std::map<int, int> tags;
	tags[STRNAME] = 0;
	tags[BOUNDARY] = 0;
	tags[SREF] = 0;
	tags[PATH] = 0;

	uint32_t i = 2; // i is 2 because the first two bytes of a gds2 file denote the size of the first tag (header-tag)

	while (i < filesize - 2) {
		int tag = getWordInt(data[i], data[i + 1]);
		int elementSize = getWordInt(data[i - 2], data[i - 1]);

		// Checks if the current tag contains important data. 
		// If it is the case it will read the data else it will jump to the next tag, based on the given element Size
		if (tags.count(tag) == 1) {


			switch (tag) {

			case STRNAME: {
				Gds2Structure newStruct = getStruct(i);
				structMap[newStruct.getName()] = newStruct;
				structures.push_back(newStruct);
				break;
			}

			case BOUNDARY: {
				MyPolygon newPol = getPolygon(i);
				polygons.push_back(newPol);
				break;
			}

			case SREF: {
				StructRef newStrRef = getStructRef(i);
				structRefs.push_back(newStrRef);
				break;
			}


			case PATH: {
				Gds2Path newPath = getPath(i);
				paths.push_back(newPath);
				break;
			}
			}
		}
		else {
			if (elementSize == 0) {
				i++;
			}

			i += elementSize;
		}
	}

	// Print which elemnets the file contains
	std::cout << "Structures: " << structures.size() << std::endl;
	std::cout << "Polygons: " << polygons.size() << std::endl;
	std::cout << "SREF: " << structRefs.size() << std::endl;
	std::cout << "Paths: " << paths.size() << std::endl;


	//
	std::pair<std::vector<MyPolygon>, std::vector<Gds2Path>> structData = getStructData(structures);

	//add BOUNDARYS to polygon data
	structData.first.insert(structData.first.end(), polygons.begin(), polygons.end());
	/*std::vector<MyPolygon> emptyVec = {};

	for(auto& strRef : structRefs){
		std::vector<MyPolygon> strRefPolys = getStrRefPolygons(structMap[strRef.getName()] , structMap, emptyVec);
		structData.first.insert(structData.first.end(), strRefPolys.begin(), strRefPolys.end());
		}*/




	//
	return structData;
}


std::vector<std::byte> Gds2Import::readFileData(const std::string& name) {
	std::filesystem::path inputFilePath{ name };
	uint32_t length = std::filesystem::file_size(inputFilePath);
	std::cout << "Filelength[Byte]: " << length << std::endl;
	if (length == 0) {
		return {};  // empty vector
	}
	std::vector<std::byte> buffer(length);
	std::ifstream inputFile(name, std::ios_base::binary);
	inputFile.read(reinterpret_cast<char*>(buffer.data()), length);
	inputFile.close();
	return buffer;
}


std::vector<std::pair<int, std::vector<int>>> Gds2Import::readStackUp(std::string path){
	std::fstream fIn;
	std::vector<std::pair<int, std::vector<int>>> stackUpInfo = {};
	std::vector<int> row = {};
	std::string line, word = "";
	int stackUp = 0;

	fIn.open(path, std::ios::in);
	
	if (!fIn.is_open()) {
		std::cerr << "File does not exist or couldn't be opened." << std::endl;
		return stackUpInfo;
	}


	
	while (getline(fIn, line)) {
		row.clear();
		std::stringstream s(line);
		int i = 0;

		
		while (getline(s, word, ';')) {
			if (i == 0) {
				stackUp = std::stoi(word);
			}
			else {
				row.push_back(std::stoi(word));
			}
			i++;
		}

		stackUpInfo.push_back(std::make_pair(stackUp, row));
	}
	fIn.close();

	std::sort(stackUpInfo.begin(), stackUpInfo.end());

	/*for (auto& info : stackUpInfo) {
		std::cout << info.second[1] << std::endl;
	}*/
	return stackUpInfo;
}
