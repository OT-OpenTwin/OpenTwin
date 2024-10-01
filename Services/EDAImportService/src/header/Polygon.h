#ifndef MYPOLYGON_H_
#define MYPOLYGON_H_

#include <vector>




class MyPolygon{
private: 
		unsigned int layer;
		std::vector<std::pair<int, int>> coordinates;

public:
	MyPolygon(unsigned int l, std::vector<std::pair<int, int>> c);
	MyPolygon();
	
	void setLayer(unsigned int layer);

	unsigned int getLayer();
	std::vector<std::pair<int, int>> getCoordinates();
	void setCoordinates(std::vector<std::pair<int, int>> coords);
};

#endif // !POLYGON_H_
