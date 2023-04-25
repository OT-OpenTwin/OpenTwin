#pragma once

class Application;

#include <vector>
#include <map>
#include <string>
#include <thread>

class ProgressLogger
{
public:
	ProgressLogger(Application *app);
	~ProgressLogger();

	void startLogger2D(bool verbose);
	void stopLogger2D(void);

	void startLogger3D(bool verbose);
	void stopLogger3D(void);

	bool isInvalidSurface(int faceTag) { return invalidSurfaces.find(faceTag) != invalidSurfaces.end(); }
	bool hasInvalidSurfacePoint(int faceTag) { return invalidSurfacesPoints.find(faceTag) != invalidSurfacesPoints.end(); }
	std::vector<std::string> getInvalidSurfacePoints(int faceTag) { return invalidSurfacesPoints[faceTag]; }

	void showLogMessages(bool flag) { showLogMessagesFlag = flag; }

private:
	void readLogInformation2D(bool verbose, size_t edges, size_t faces);
	void readLogInformation3D(bool verbose);
	std::string process3DErrorMessages(bool verbose, std::vector<std::string> &messages3D);
	bool parseErrorString(const std::string &message, std::string format, std::vector<std::string> &values);
	int getSurfaceId(std::string &text);

	Application *application;
	bool loggerRunning;
	std::thread *workerThread;
	bool verboseFlag;
	bool showLogMessagesFlag;

	std::map<int, bool>                     invalidSurfaces;
	std::map<int, std::vector<std::string>> invalidSurfacesPoints;
};
