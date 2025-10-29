// @otlicense

#pragma once

class Application;
class SelfIntersectionCheck;
class ProgressLogger;
class StepWidthManager;

class EntityAnnotation;

#include <list>
#include <vector>
#include <tuple>
#include <string>

class ProximityMeshing
{
public:
	ProximityMeshing(Application *app);
	~ProximityMeshing() {};

	bool fixMeshIntersections(const std::string &meshName, int proximityMeshing, ProgressLogger *logger, StepWidthManager *stepWidthManager, bool verbose);
	void storeErrorAnnotations(void);

private:
	void loadSelfIntersectionObject(SelfIntersectionCheck &intersectionChecker);
	bool checkSelfIntersection(SelfIntersectionCheck &checker);
	void visualiseSelfIntersections(const std::string &meshName, std::vector < std::tuple<double, double, double, uint32_t, uint32_t> > IntersectingInfo, std::list<EntityAnnotation *> &errorAnnotation);
	std::string quadraticExpression(double localSize, double Sdelta, std::vector<double> partnerFaces);
	std::string linearExpression(double localSize, double Sdelta, std::vector<double> partnerFaces);

	Application *application;
	std::list<EntityAnnotation *> errorAnnotations;
};
