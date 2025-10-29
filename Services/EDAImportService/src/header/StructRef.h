// @otlicense

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

