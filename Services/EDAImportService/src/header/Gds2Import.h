#ifndef GDS2IMPORT_H_
#define GDS2IMPORT_H_

#include <vector>
#include <string>
#include <map>
#include "Polygon.h"
#include "Gds2Path.h"
#include "StructRef.h"
#include "Gds2Structure.h"
class Gds2Import {
private:
	std::vector<std::byte> data;
	uint32_t filesize;

	inline void checkSize(unsigned int x);
	int getWordInt(std::byte a, std::byte b);
	int getCoordinate(std::byte a, std::byte b, std::byte c, std::byte d);
	int getElemSize(std::byte a, std::byte b);
	std::string getStructName(uint32_t readPosition);
	std::vector<std::pair<int, int>> getXY(uint32_t readPosition);
	std::vector<MyPolygon> getStrRefPolygons(Gds2Structure& str, std::map<std::string, Gds2Structure>& structMap, std::vector<MyPolygon>& polygons);
	StructRef getStructRef(uint32_t& readPosition);
	MyPolygon getPolygon(uint32_t& readPosition);
	Gds2Path getPath(uint32_t& readPosition);
	Gds2Structure getStruct(uint32_t& readPosition);
	std::pair<std::vector<MyPolygon>, std::vector<Gds2Path>> getStrRefData(Gds2Structure& structure, std::map<std::string, Gds2Structure>& structMap, std::pair<std::vector<MyPolygon>, std::vector<Gds2Path>>& strReFData);
public:
	Gds2Import();
	Gds2Import(std::string filePath);
	std::vector<std::byte> readFileData(const std::string& name);
	std::vector<MyPolygon> getPolygons();
	std::vector<Gds2Structure> getModelData();
	std::pair<std::vector<MyPolygon>,std::vector<Gds2Path>> modelData();
	std::pair<std::vector<MyPolygon>, std::vector<Gds2Path>> getStructData(std::vector<Gds2Structure>& structures);
	std::vector<std::pair<int, std::vector<int>>> readStackUp(std::string path);
};
#endif