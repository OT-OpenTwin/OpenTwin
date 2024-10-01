#include "Gds2Path.h"
#include <cmath>

#include <iostream>
#include <vector>
#include <cmath>
#include <utility>

// Utility function to add a perpendicular offset to a point
std::pair<int, int> offsetPoint(const std::pair<int, int>& p, const std::pair<int, int>& direction, double offset) {
    std::pair<int, int> result;
    result.first = p.first + static_cast<int>(offset * (-direction.second));
    result.second = p.second + static_cast<int>(offset * direction.first);
    return result;
}

// Normalize a vector
std::pair<int, int> normalize(const std::pair<int, int>& p) {
    double length = std::sqrt(p.first * p.first + p.second * p.second);
    return { static_cast<int>(p.first / length), static_cast<int>(p.second / length) };
}

// Convert path to polygon
std::vector<std::pair<int, int>> toPolygon(const std::vector<std::pair<int, int>>& path, double width) {

    std::vector<std::pair<int, int>> topPoints;
    std::vector<std::pair<int, int>> bottomPoints;
    int halfWidth = width / 2;

    for (int i = 0; i < path.size() - 1; i++) {
        if (path[i].first == path[i + 1].first) {
            topPoints.push_back(std::make_pair(path[i].first + halfWidth, path[i].second));
            bottomPoints.push_back(std::make_pair(path[i].first - halfWidth, path[i].second));
        }
        else {
            topPoints.push_back(std::make_pair(path[i].first, path[i].second + halfWidth));
            bottomPoints.push_back(std::make_pair(path[i].first, path[i].second - halfWidth));
        }
    }

    if (path[path.size() - 1].first == path[path.size() - 2].first) {
        topPoints.push_back(std::make_pair(path[path.size() - 1].first + halfWidth, path[path.size() - 1].second));
        bottomPoints.push_back(std::make_pair(path[path.size() - 1].first - halfWidth, path[path.size() - 1].second));
    }
    else {
        topPoints.push_back(std::make_pair(path[path.size() - 1].first, path[path.size() - 1].second + halfWidth));
        bottomPoints.push_back(std::make_pair(path[path.size() - 1].first, path[path.size() - 1].second - halfWidth));
    }

    for (int i = bottomPoints.size() - 1; i >= 0; i--) {
        topPoints.push_back(bottomPoints[i]);
    }

    topPoints.push_back(topPoints[0]);


    //std::vector<std::pair<int, int>> polygon;
    //std::vector<std::pair<int, int>> topPoints;
    //std::vector<std::pair<int, int>> bottomPoints;
    //double halfWidth = width / 2;

    //if (path.size() < 2) {
    //    return polygon;  // Not enough points to form a path
    //}

    //// Calculate the first segment direction and offset points
    //std::pair<int, int> direction = { path[1].first - path[0].first, path[1].second - path[0].second };
    //direction = normalize(direction);
    //std::pair<int, int> offset1 = offsetPoint(path[0], direction, halfWidth);
    //std::pair<int, int> offset2 = offsetPoint(path[0], direction, -halfWidth);
    //topPoints.push_back(offset1);
    //bottomPoints.push_back(offset2);

    //// Process intermediate points
    //for (size_t i = 1; i < path.size() - 1; ++i) {
    //    std::pair<int, int> prevDirection = { path[i].first - path[i - 1].first, path[i].second - path[i - 1].second };
    //    std::pair<int, int> nextDirection = { path[i + 1].first - path[i].first, path[i + 1].second - path[i].second };
    //    prevDirection = normalize(prevDirection);
    //    nextDirection = normalize(nextDirection);

    //    // Average direction for the miter join
    //    /*std::pair<int, int> avgDirection = { (prevDirection.first + nextDirection.first) / 2, (prevDirection.second + nextDirection.second) / 2 };
    //    avgDirection = normalize(avgDirection);

    //    std::pair<int, int> offset1 = offsetPoint(path[i], avgDirection, halfWidth);
    //    std::pair<int, int> offset2 = offsetPoint(path[i], avgDirection, -halfWidth);*/
    //    topPoints.push_back(offset1);
    //    bottomPoints.push_back(offset2);
    //}

    //// Calculate the last segment direction and offset points
    //direction = { path[path.size() - 1].first - path[path.size() - 2].first, path[path.size() - 1].second - path[path.size() - 2].second };
    //direction = normalize(direction);
    //offset1 = offsetPoint(path.back(), direction, halfWidth);
    //offset2 = offsetPoint(path.back(), direction, -halfWidth);
    //topPoints.push_back(offset1);
    //bottomPoints.push_back(offset2);

    //// Add topPoints to polygon
    //for (const auto& point : topPoints) {
    //    polygon.push_back(point);
    //}

    //// Add bottomPoints to polygon in reverse order
    //for (auto it = bottomPoints.rbegin(); it != bottomPoints.rend(); ++it) {
    //    polygon.push_back(*it);
    //}

    //// Add the first point again to close the polygon
    //polygon.push_back(polygon[0]);

    //return polygon;
    return topPoints;
}





std::pair<int, int> Gds2Path::calcVector(std::pair<int, int>& p1, std::pair<int, int>& p2){
	return std::make_pair(p2.first - p1.first, p2.second - p1.second);
}

std::pair<int, int> Gds2Path::calcOffset(std::pair<int, int>& p1, std::pair<int, int>& p2){
	
	unsigned int width = getWidth();
	std::pair<int, int> vec = calcVector(p1, p2);
	int length = sqrt(pow(vec.first, 2) + pow(vec.second, 2));
	std::pair<int, int> offset = std::make_pair(-(vec.first / length) * width / 2, -(vec.second / length) * width / 2);

	return offset;
}

MyPolygon Gds2Path::pathToPolygon(){
    /*
	std::vector<std::pair<int, int>> upperPath = {};
	std::vector<std::pair<int, int>> lowerPath = {};
	std::vector<std::pair<int, int>> coordinates = getCoordinates();
	int pathSize = coordinates.size();

	for (int i = 0; i < pathSize - 1; i++) {
		std::pair<int, int> offset = calcOffset(coordinates[i], coordinates[i + 1]);
		upperPath.push_back(std::make_pair(coordinates[i].first + offset.first, coordinates[i].second + offset.second));
		lowerPath.push_back(std::make_pair(coordinates[i].first - offset.first, coordinates[i].second - offset.second));
	}

	//add last points
	std::pair<int, int> offset = calcOffset(coordinates[pathSize -2 ], coordinates[pathSize - 1]);
	upperPath.push_back(std::make_pair(coordinates.back().first + offset.first, coordinates.back().second + offset.second));
	lowerPath.push_back(std::make_pair(coordinates.back().first - offset.first, coordinates.back().second - offset.second));

	std::vector<std::pair<int, int>> newCoords = upperPath;

	//lower path is added to the coordinates with the reversed order
	for (int i = lowerPath.size() - 1; i >= 0; i--) {
		newCoords.push_back(lowerPath[i]);
	}

	newCoords.push_back(newCoords[0]); // The first coordinate is added to the end to close the polygon

	MyPolygon newPoly(getLayer(), newCoords);

	return newPoly;*/
    std::vector<std::pair<int,int>> coords = toPolygon(getCoordinates(), getWidth());
    MyPolygon newPoly(getLayer(), coords);

    return newPoly;

}

unsigned int Gds2Path::getWidth(){ return width; }

unsigned int Gds2Path::getLayer(){ return layer; }

unsigned int Gds2Path::getPathType(){ return pathType; }

std::vector<std::pair<int, int>> Gds2Path::getCoordinates(){ return coordinates; }

void Gds2Path::setWidth(int width) { this->width = width; }

void Gds2Path::setLayer(unsigned int layer) { this->layer = layer; }

void Gds2Path::setPathType(unsigned int pathType) { this->pathType = pathType; }

void Gds2Path::setCoordinates(std::vector<std::pair<int, int>> coordinates){ this->coordinates = coordinates; }
