#include "ProgressLogger.h"

#include "Application.h"

#include "OTServiceFoundation/UiComponent.h"

#include <gmsh.h_cwrap>

#include <algorithm>
#include <sstream>
#include <chrono>

ProgressLogger::ProgressLogger(Application *app) : 
	application(app) ,
	loggerRunning(false),
	workerThread(nullptr),
	verboseFlag(false),
	showLogMessagesFlag(true)
{
}

ProgressLogger::~ProgressLogger()
{
	// Terminate the logging if it is still running
	if (workerThread != nullptr)
	{
		loggerRunning = false;
		workerThread->join();

		delete workerThread;
		workerThread = nullptr;

		gmsh::logger::stop();
	}
}

void ProgressLogger::startLogger2D(bool verbose)
{
	assert(workerThread == nullptr);

	verboseFlag         = verbose;
	showLogMessagesFlag = true;

	gmsh::logger::start();

	gmsh::vectorpair entityTags;
	gmsh::model::getEntities(entityTags, 1);

	size_t numberOfEdges = entityTags.size();

	entityTags.clear();
	gmsh::model::getEntities(entityTags, 2);

	size_t numberOfFaces = entityTags.size();
	entityTags.clear();

	loggerRunning = true;
	workerThread = new std::thread(&ProgressLogger::readLogInformation2D, this, verbose, numberOfEdges, numberOfFaces);
}

void ProgressLogger::stopLogger2D(void)
{
	loggerRunning = false;
	workerThread->join();

	delete workerThread;
	workerThread = nullptr;

	std::vector<std::string> messages2D;
	gmsh::logger::get(messages2D);

	gmsh::logger::stop();

	// Now we process the logged messages
	std::string message;

	for (auto msg : messages2D)
	{
		bool skipMessage = false;

		if (msg.length() > 10 && msg.substr(0, 10) == "Progress: ")          skipMessage = true;
		if (msg.length() > 14 && msg.substr(0, 14) == "Info: Meshing ")      skipMessage = true;
		if (msg.length() > 19 && msg.substr(0, 19) == "Info: Done meshing ") skipMessage = true;
		if (!verboseFlag && msg.length() > 5 && msg.substr(0, 5) == "Info:") skipMessage = true;

		if (msg.length() > 9 && msg.substr(0, 9) == "Warning: ")
		{
			if (msg.find("remain invalid in surface") != std::string::npos)
			{
				// Search for surface id
				size_t pos = msg.rfind(' ');
				std::string surfaceString = msg.substr(pos);
				int surfaceId = std::stoi(surfaceString);

				invalidSurfaces[surfaceId] = true;
			}
		}

		if (!skipMessage) message += "\t" + msg + "\n";
	}

	if (!message.empty() && showLogMessagesFlag)
	{
		application->getUiComponent()->displayMessage(message);
	}
}

void ProgressLogger::startLogger3D(bool verbose)
{
	assert(workerThread == nullptr);

	verboseFlag         = verbose;
	showLogMessagesFlag = true;

	gmsh::logger::start();

	loggerRunning = true;
	workerThread = new std::thread(&ProgressLogger::readLogInformation3D, this, verbose);
}

void ProgressLogger::stopLogger3D(void)
{
	loggerRunning = false;
	workerThread->join();

	delete workerThread;
	workerThread = nullptr;

	std::vector<std::string> messages3D;
	gmsh::logger::get(messages3D);

	gmsh::logger::stop();

	std::string message = process3DErrorMessages(verboseFlag, messages3D);

	if (!message.empty() && showLogMessagesFlag)
	{
		application->getUiComponent()->displayMessage(message);
	}
}

void ProgressLogger::readLogInformation2D(bool verbose, size_t edges, size_t faces)
{
	size_t messageCount = 0;

	int meshingDimension = 1;

	int lastPercent = 0;

	while (loggerRunning)
	{
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1000ms);

		std::vector<std::string> messages;
		gmsh::logger::get(messages);

		double percent = 0.0;

		if (messages.size() > messageCount)
		{
			std::string messageText;
			for (size_t i = messageCount; i < messages.size(); i++)
			{
				if (verbose)
				{
					if (messages[i].substr(0, 9) != "Progress:")
					{
						if (!messageText.empty())
						{
							messageText.append("\n");
						}
						messageText.append("\t");
						messageText.append(messages[i]);
					}
				}

				if (meshingDimension == 1 && messages[i].substr(0, 20) == "Info: Meshing curve ")
				{
					long long curveNum = atoll(messages[i].substr(20).c_str());

					percent = std::max(percent, 40.0 * curveNum / edges);

				}
				else if (messages[i].substr(0, 22) == "Info: Meshing surface ")
				{
					meshingDimension = 2;

					long long faceNum = atoll(messages[i].substr(22).c_str());

					percent = std::max(percent, 40.0 + 60.0 * faceNum / faces);
				}
			}

			if (verbose)
			{
				application->getUiComponent()->displayMessage(messageText + "\n");
			}

			messageCount = messages.size();

			int nowPercent = (int)(percent + 0.5);

			if (nowPercent > lastPercent)
			{
				lastPercent = nowPercent;
				application->getUiComponent()->setProgress(nowPercent);
			}
		}
	}
}

void ProgressLogger::readLogInformation3D(bool verbose)
{
	size_t messageCount = 0;

	double maxRadius = 1.0;
	double radius = 0.0;

	bool meshOptimization = false;

	int lastPercent = 0;

	while (loggerRunning)
	{
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(1000ms);

		std::vector<std::string> messages;
		gmsh::logger::get(messages);

		if (messages.size() > messageCount)
		{
			std::string messageText;
			for (size_t i = messageCount; i < messages.size(); i++)
			{
				if (verbose)
				{
					if (messages[i].substr(0, 9) != "Progress:")
					{
						if (!messageText.empty())
						{
							messageText.append("\n");
						}
						messageText.append("\t");
						messageText.append(messages[i]);
					}
				}

				if (!meshOptimization)
				{
					size_t found = messages[i].find("nodes created - worst tet radius");
					if (found != std::string::npos)
					{
						std::string value = messages[i].substr(found + 32);
						radius = atof(messages[i].substr(found + 32).c_str());

						maxRadius = std::max(radius, maxRadius);
					}
					else
					{
						if (messages[i] == "Info: Optimizing mesh..." || messages[i].find("Info: Improving") != std::string::npos)
						{
							application->getUiComponent()->setProgress(100);
							application->getUiComponent()->setProgressInformation("Optimizing mesh", true);
							meshOptimization = true;
						}
					}
				}
			}

			if (verbose)
			{
				application->getUiComponent()->displayMessage(messageText + "\n");
			}

			messageCount = messages.size();

			// Now calculate the percentage progress resulting from the current and max radii

			if (!meshOptimization && maxRadius > 1.0)
			{
				int nowPercent = (int)(100.0 - 100.0 * log(std::max(1.0, radius)) / log(maxRadius) + 0.5);

				if (nowPercent > lastPercent)
				{
					lastPercent = nowPercent;
					application->getUiComponent()->setProgress(nowPercent);
				}
			}
		}
	}
}

std::string ProgressLogger::process3DErrorMessages(bool verbose, std::vector<std::string> &messages3D)
{
	std::string message;

	for (auto msg : messages3D)
	{
		bool skipMessage = false;

		if (msg.length() > 10 && msg.substr(0, 10) == "Progress: ")      skipMessage = true;
		if (msg.find("< quality <") != std::string::npos)                skipMessage = true;
		if (!verbose && msg.length() > 5 && msg.substr(0, 5) == "Info:") skipMessage = true;

		if (msg.length() > 7 && msg.substr(0, 7) == "Error: ")
		{
			// Check for specific error messages
			std::vector<std::string> values;

			if (parseErrorString(msg, "Error: Invalid boundary mesh (segment-facet intersection) on surface %s intersection %s", values))
			{
				int surfaceId = getSurfaceId(values[0]);

				invalidSurfaces[surfaceId] = true;

				std::vector<std::string> pointList = invalidSurfacesPoints[surfaceId];
				pointList.push_back(values[1]);
				invalidSurfacesPoints[surfaceId] = pointList;
			}
			else
			{
				// If no specific message found, then try to read as much information from the error as possible
				size_t pos = msg.find("on surface");

				if (pos != std::string::npos)
				{
					// Search for surface id
					std::string surfaceString = msg.substr(pos + 10);
					int surfaceId = std::stoi(surfaceString);

					invalidSurfaces[surfaceId] = true;
				}
			}
		}

		if (!skipMessage) message += "\t" + msg + "\n";
	}

	return message;
}

bool ProgressLogger::parseErrorString(const std::string &message, std::string format, std::vector<std::string> &values)
{
	std::stringstream ssformat(format);
	std::stringstream ssmessage(message);

	std::string fsegment, msegment;

	ssformat >> fsegment;
	ssmessage >> msegment;

	while (true)
	{
		if (fsegment[0] == '%')
		{
			// We have a format specification to read the data segment
			values.push_back(msegment);
		}
		else
		{
			// We need to check whether the format is identical to the message. If not, we don't have a match
			if (msegment != fsegment) return false;
		}

		if (ssformat.eof() && ssmessage.eof()) break;

		if (ssformat.eof()) return false;
		if (ssmessage.eof()) return false;

		ssformat >> fsegment;
		ssmessage >> msegment;
	}

	return true;
}

int ProgressLogger::getSurfaceId(std::string &text)
{
	return std::atoi(text.c_str());
}
