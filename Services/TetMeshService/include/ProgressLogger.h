// @otlicense
// File: ProgressLogger.h
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
