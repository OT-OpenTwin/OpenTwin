// Project header
#include "StudioSuiteConnector/StudioConnector.h"

// OpenTwin header
#include "OTSystem/OperatingSystem.h"
#include "OTCore/LogDispatcher.h"
#include "OTCommunication/ActionTypes.h"
#include "OTFrontendConnectorAPI/WindowAPI.h"

// Qt header
#include <QtCore/quuid.h>
#include <QtCore/qsettings.h>

// std header
#include <thread>
#include <cassert>
#include <sstream>
#include <filesystem>

//#define DEBUG_PYTHON_SERVER

std::string StudioConnector::searchProjectAndExtractData(const std::string& fileName, const std::string& projectRoot, bool includeResults, bool includeParametricResults)
{
	ot::WindowAPI::setProgressBarVisibility("Waiting for CST Studio Suite", true, true);

	// First, we need to ensure that the python subservice is running
	startSubprocess();

	// Get the latest version of Studio Suite and the installpath
	int version = 0;
	std::string studioPath;
	determineStudioSuiteInstallation(version, studioPath);

	// Now we need to get a list of all process ids of running CST STUDIO SUITE DESIGN ENVIRONMENTS
	std::list<long long> studioPidList = getRunningDesignEnvironmentProcesses();

	// Now we execute a python script which performs the following tasks:
	// 1. Iterate through all pids and try connecting the DE. 
	// 2. If a DE was found, check whether the file is opened there. If so, use this DE for further processing
	// 3. If no DE with the given project was found, launch another DE instance and open the project there. Continue with this DE
	// 4. Save the project
	// 5. Execute a VBA script to extract the necessary information

	std::string exportFolder = projectRoot + "/Temp/Upload";
	std::filesystem::remove_all(exportFolder);

	std::string script = generateExtractScript(studioPath, fileName, exportFolder, studioPidList, includeResults, includeParametricResults);

	ot::ReturnMessage returnMessage = executeCommand(script);

	if (returnMessage.getStatus() == ot::ReturnMessage::Ok)
	{
		return "";
	}
	else
	{
		return returnMessage.getWhat();
	}
}

std::string StudioConnector::closeProject(const std::string& fileName)
{
	ot::WindowAPI::setProgressBarVisibility("Waiting for CST Studio Suite", true, true);

	// First, we need to ensure that the python subservice is running
	startSubprocess();

	// Get the latest version of Studio Suite and the installpath
	int version = 0;
	std::string studioPath;
	determineStudioSuiteInstallation(version, studioPath);

	// Now we need to get a list of all process ids of running CST STUDIO SUITE DESIGN ENVIRONMENTS
	std::list<long long> studioPidList = getRunningDesignEnvironmentProcesses();

	// Now we execute a python script which performs the following tasks:
	// 1. Iterate through all pids and try connecting the DE. 
	// 2. If a DE was found, check whether the file is opened there. If so, close this file

	std::string script = generateCloseScript(studioPath, fileName, studioPidList);

	ot::ReturnMessage returnMessage = executeCommand(script);

	if (returnMessage.getStatus() == ot::ReturnMessage::Ok)
	{
		return "";
	}
	else
	{
		return returnMessage.getWhat();
	}
}

std::string StudioConnector::openProject(const std::string& fileName)
{
	ot::WindowAPI::setProgressBarVisibility("Waiting for CST Studio Suite", true, true);

	// First, we need to ensure that the python subservice is running
	startSubprocess();

	// Get the latest version of Studio Suite and the installpath
	int version = 0;
	std::string studioPath;
	determineStudioSuiteInstallation(version, studioPath);

	// Now we execute a python script which performs the following tasks:
	// 1. Instantiate a new DE and open the selected project 

	std::string script = generateOpenScript(studioPath, fileName);

	ot::ReturnMessage returnMessage = executeCommand(script);

	if (returnMessage.getStatus() == ot::ReturnMessage::Ok)
	{
		return "";
	}
	else
	{
		return returnMessage.getWhat();
	}
}

StudioConnector::~StudioConnector()
{
	if (m_subProcessRunning)
	{
		closeSubprocess();
	}
}

std::list<long long> StudioConnector::getRunningDesignEnvironmentProcesses()
{
	QProcess tasklist;
	tasklist.start(
		"tasklist",
		QStringList()
		<< "/FO" << "LIST"
		<< "/FI" << QString("IMAGENAME eq  CST DESIGN ENVIRONMENT_AMD64.exe"));
	tasklist.waitForFinished();
	std::stringstream output(tasklist.readAllStandardOutput().toStdString());

	std::list<long long> processIDs;

	while (!output.eof())
	{
		std::string line;
		std::getline(output, line);

		if (line.substr(0, 4) == "PID:")
		{
			long long pid = atoll(line.substr(5).c_str());
			processIDs.push_back(pid);
		}
	}

	return processIDs;
}

std::string StudioConnector::generateExtractScript(const std::string &studioPath, std::string fileName, std::string exportFolder, std::list<long long> studioPidList, bool includeResults, bool includeParametricResults)
{

	std::replace(fileName.begin(), fileName.end(), '/', '\\');

	std::stringstream script;

	script << "fileName = r'" << fileName << "'\n";
	script << "exportFolder = r'" << exportFolder << "'\n";
	script << "includeResults = " << (includeResults ? "True" : "False") << "\n";
	script << "includeParametricResults = " << (includeParametricResults ? "True" : "False") << "\n";
	script << "idlist = [";
	while (!studioPidList.empty())
	{
		script << studioPidList.front();
		studioPidList.pop_front();

		if (!studioPidList.empty()) script << ",";
	}
	script << "]\n";

	script << "import sys\n";
	script << "sys.path.append(r'" << studioPath << "\\AMD64\\python_cst_libraries')\n";

	script << "from cst.interface import DesignEnvironment\n";
	script << "import cst.results, os\n";

	script << "de = None\n";
	script << "prj = None\n";

	script << "for id in idlist:\n";
	script << "    try:\n";
	script << "       print('trying to connect...')\n";
	script << "       de = DesignEnvironment.connect(id)\n";
	script << "       if de.is_connected():\n";
	script << "          print('connected.')\n";
	script << "          prj = de.get_open_project(fileName)\n";
	script << "          break\n";
	script << "    except:\n";
	script << "       de = None\n";

	script << "if (prj == None):\n";
	script << "    print('project not found, creating new instance')\n";
	script << "    de = DesignEnvironment.new()\n";
	script << "    try:\n";
	script << "       prj = de.open_project(fileName)\n";
	script << "       print('project found')\n";
	script << "    except:\n";
	script << "       de.close();\n";
	script << "       raise Exception('Unable to open project')\n";

	script << "print('saving project')\n";
	script << "prj.save()\n";
	script << "print('saving done')\n";

	script << "vbaCode = (";

	// Here comes the VBA code. Every " in the code needs to be masked as \\\".
	// Every line is preceeded by "\" and appended by "\n"
	// For a line break, \\n is appended to the line.

	script << 
		"\"'#Language \\\"WWB-COM\\\" \\n\"\n"
		"\"\\n\"\n"
		"\"Option Explicit\\n\"\n"
		"\"\\n\"\n"
		"\"Dim count As Long\\n\"\n"
		"\"Dim baseFolder As String\\n\"\n"
		"\"\\n\"\n"
		"\"\\n\"\n"
		"\"Sub ExportSolidChildItem(parent$)\\n\"\n"
		"\"\\n\"\n"
		"\"	Dim item As String\\n\"\n"
		"\"	item = Resulttree.GetFirstChildName(parent$)\\n\"\n"
		"\"\\n\"\n"
		"\"	If item = \\\"\\\" Then\\n\"\n"
		"\"		Dim itemName As String\\n\"\n"
		"\"		itemName = Right(parent, Len(parent)-11)\\n\"\n"
		"\"		Dim index As Long\\n\"\n"
		"\"		index = InStrRev(itemName, \\\"\\\\\\\")\\n\"\n"
		"\"		Dim solidName As String\\n\"\n"
		"\"		Dim componentName As String\\n\"\n"
		"\"		solidName = Right(itemName, Len(itemName)-index)\\n\"\n"
		"\"		componentName = Left(itemName, index-1)\\n\"\n"
		"\"\\n\"\n"
		"\"		componentName = Replace(componentName, \\\"\\\\\\\", \\\"/\\\")\\n\"\n"
		"\"\\n\"\n"
		"\"		With STL\\n\"\n"
		"\"			.Reset\\n\"\n"
		"\"			.FileName (baseFolder + \\\"/stl\\\" + CStr(count) + \\\".stl\\\")\\n\"\n"
		"\"		    .Name (solidName)\\n\"\n"
		"\"	   		.Component (componentName)\\n\"\n"
		"\"    		.ExportFromActiveCoordinateSystem (False)\\n\"\n"
		"\"    		.ExportFileUnits(Units.GetUnit(\\\"Length\\\"))\\n\"\n"
		"\"    		.Write\\n\"\n"
		"\"		End With\\n\"\n"
		"\"\\n\"\n"
		"\"		Print #1, itemName\\n\"\n"
		"\"		Print #1, Solid.GetMaterialNameForShape(componentName + \\\":\\\" + solidName)\\n\"\n"
		"\"\\n\"\n"
		"\"		count = count + 1\\n\"\n"
		"\"	End If\\n\"\n"
		"\"\\n\"\n"
		"\"	While item <> \\\"\\\"\\n\"\n"
		"\"		ExportSolidChildItem(item)\\n\"\n"
		"\"		item = Resulttree.GetNextItemName(item)\\n\"\n"
		"\"	Wend\\n\"\n"
		"\"\\n\"\n"
		"\"End Sub\\n\"\n"
		"\"\\n\"\n"
		"\"Sub ExportMaterials()\\n\"\n"
		"\"\\n\"\n"
		"\"	Dim index As Long\\n\"\n"
		"\"	For index = 0 To Material.GetNumberOFMaterials()-1\\n\"\n"
		"\"\\n\"\n"
		"\"		Dim item As String\\n\"\n"
		"\"		item = Material.GetNameOfMaterialFromIndex(index)\\n\"\n"
		"\"\\n\"\n"
		"\"		Dim materialType As String\\n\"\n"
		"\"		materialType = Material.GetTypeOfMaterial(item)\\n\"\n"
		"\"\\n\"\n"
		"\"		Dim r As Double\\n\"\n"
		"\"		Dim g As Double\\n\"\n"
		"\"		Dim b As Double\\n\"\n"
		"\"		Material.GetColour(item, r, g, b)\\n\"\n"
		"\"\\n\"\n"
		"\"		Print #1, item\\n\"\n"
		"\"		Print #1, r; \\\" \\\"; g; \\\" \\\"; b\\n\"\n"
		"\"		Print #1, materialType\\n\"\n"
		"\"\\n\"\n"
		"\"		If materialType = \\\"Normal\\\" Then\\n\"\n"
		"\"			Dim x As Double\\n\"\n"
		"\"			Dim y As Double\\n\"\n"
		"\"			Dim z As Double\\n\"\n"
		"\"			Material.GetEpsilon(item, x, y, z)\\n\"\n"
		"\"			Print #1, x; \\\" \\\"; y; \\\" \\\"; z\\n\"\n"
		"\"			Material.GetMu(item, x, y, z)\\n\"\n"
		"\"			Print #1, x; \\\" \\\"; y; \\\" \\\"; z\\n\"\n"
		"\"			Material.GetSigma(item, x, y, z)\\n\"\n"
		"\"			Print #1, x; \\\" \\\"; y; \\\" \\\"; z\\n\"\n"
		"\"		End If\\n\"\n"
		"\"	Next\\n\"\n"
		"\"End Sub\\n\"\n"
		"\"\\n\"\n"
		"\"\\n\"\n"
		"\"Sub ExportUnits()\\n\"\n"
		"\"     Print #1, Units.GetUnit(\\\"Length\\\")\\n\"\n"
		"\"     Print #1, Units.GetUnit(\\\"Temperature\\\")\\n\"\n"
		"\"     Print #1, Units.GetUnit(\\\"Voltage\\\")\\n\"\n"
		"\"     Print #1, Units.GetUnit(\\\"Current\\\")\\n\"\n"
		"\"     Print #1, Units.GetUnit(\\\"Resistance\\\")\\n\"\n"
		"\"     Print #1, Units.GetUnit(\\\"Conductance\\\")\\n\"\n"
		"\"     Print #1, Units.GetUnit(\\\"Capacitance\\\")\\n\"\n"
		"\"     Print #1, Units.GetUnit(\\\"Inductance\\\")\\n\"\n"
		"\"     Print #1, Units.GetUnit(\\\"Frequency\\\")\\n\"\n"
		"\"     Print #1, Units.GetUnit(\\\"Time\\\")\\n\"\n"
		"\"End Sub\\n\"\n"
		"\"\\n\"\n"
		"\"\\n\"\n"
		"\"Sub ExportParameters()\\n\"\n"
		"\"		Print #1, CStr(GetNumberOfParameters())\\n\"\n"
		"\"		Dim index As Long\\n\"\n"
		"\"		For index = 0 To GetNumberOfParameters()-1\\n\"\n"
		"\"			Print #1, GetParameterName(index)\\n\"\n"
		"\"			Print #1, CStr(GetParameterNValue(index))\\n\"\n"
		"\"			Print #1, GetParameterDescription(GetParameterName(index))\\n\"\n"
		"\"		Next\\n\"\n"
		"\"End Sub\\n\"\n"
		"\"\\n\"\n"
		"\"Sub Main\\n\"\n"
		"\"\\n\"\n"
		"\"	SetLock(True)\\n\"\n"
		"\"\\n\"\n"
		"\"	baseFolder = GetProjectPath(\\\"Temp\\\") + \\\"/Upload\\\"\\n\"\n"
		"\"	On Error Resume Next\\n\"\n"
		"\"	MkDir baseFolder\\n\"\n"
		"\"	count = 0\\n\"\n"
		"\"\\n\"\n"
		"\"	ScreenUpdating(False)\\n\"\n"
		"\"	Dim currentItem As String\\n\"\n"
		"\"	currentItem = GetSelectedTreeItem()\\n\"\n"
		"\"\\n\"\n"
		"\"	Open baseFolder + \\\"/shape.info\\\" For Output As #1\\n\"\n"
		"\"	ExportSolidChildItem(\\\"Components\\\")\\n\"\n"
		"\"	Close #1\\n\"\n"
		"\"\\n\"\n"
		"\"	SelectTreeItem(currentItem)\\n\"\n"
		"\"	ScreenUpdating(True)\\n\"\n"
		"\"\\n\"\n"
		"\"	Open baseFolder + \\\"/material.info\\\" For Output As #1\\n\"\n"
		"\"	ExportMaterials()\\n\"\n"
		"\"	Close #1\\n\"\n"
		"\"\\n\"\n"
		"\"	Open baseFolder + \\\"/units.info\\\" For Output As #1\\n\"\n"
		"\"	ExportUnits()\\n\"\n"
		"\"	Close #1\\n\"\n"
		"\"\\n\"\n"
		"\"	Open baseFolder + \\\"/parameters.info\\\" For Output As #1\\n\"\n"
		"\"	ExportParameters()\\n\"\n"
		"\"	Close #1\\n\"\n"
		"\"\\n\"\n"
		"\"	SetLock(False)\\n\"\n"
		"\"End Sub\\n\"\n";
		
	script << ")\n";

	script << "print('execute VBA code')\n";
	script << "prj.schematic.execute_vba_code(vbaCode)\n";

	script << "if includeResults:\n";
	script << "    project = cst.results.ProjectFile(fileName, True)\n";
	script << "    results = project.get_3d().get_tree_items()\n";

	script << "    if includeParametricResults:\n";
	script << "        runIds = project.get_3d().get_all_run_ids()\n";
	script << "    else:\n";
	script << "        runIds = [0]\n";

	script << "    for id in runIds :\n";
	script << "        dict = project.get_3d().get_parameter_combination(id)\n";
	script << "        exportFileName = exportFolder + '\\\\' + str(id) + '\\\\parameters.info'\n";
	script << "        os.makedirs(os.path.dirname(exportFileName), exist_ok = True)\n";
	script << "        f = open(exportFileName, 'w')\n";
	script << "        for key, value in dict.items() :\n";
	script << "            f.write(str(key) + '\\n' + str(value) + '\\n')\n";
	script << "        f.close()\n";

	script << "    for item in results :\n";
	script << "        try:\n";
	script << "            if includeParametricResults:\n";
	script << "                runIds = project.get_3d().get_run_ids(item)\n";
	script << "            else:\n";
	script << "                runIds = [0]\n";
	script << "            for id in runIds : \n";
	script << "                data = project.get_3d().get_result_item(item, id, False)\n";
	script << "                exportFileName = exportFolder + '\\\\' + str(id) + '\\\\' + item\n";
	script << "                os.makedirs(os.path.dirname(exportFileName), exist_ok = True)\n";
	script << "                f = open(exportFileName, 'w')\n";
	script << "                f.write(data.title + '\\n')\n";
	script << "                f.write(data.xlabel + '\\n')\n";
	script << "                f.write(data.ylabel + '\\n')\n";
	script << "                f.write(str(len(data.get_data())) + '\\n')\n";
	script << "                for value in data.get_data() :\n";
	script << "                    f.write(str(value) + '\\n')\n";
	script << "                f.close()\n";
	script << "        except:\n";
	script << "            pass\n";

	script << "de.set_quiet_mode(False)\n";

	return script.str();
}

std::string StudioConnector::generateCloseScript(const std::string& studioPath, std::string fileName, std::list<long long> studioPidList)
{
	std::replace(fileName.begin(), fileName.end(), '/', '\\');

	std::stringstream script;

	script << "fileName = r'" << fileName << "'\n";
	script << "idlist = [";
	while (!studioPidList.empty())
	{
		script << studioPidList.front();
		studioPidList.pop_front();

		if (!studioPidList.empty()) script << ",";
	}
	script << "]\n";

	script << "import sys\n";
	script << "sys.path.append(r'" << studioPath << "\\AMD64\\python_cst_libraries')\n";

	script << "from cst.interface import DesignEnvironment\n";

	script << "de = None\n";
	script << "prj = None\n";

	script << "for id in idlist:\n";
	script << "    try:\n";
	script << "       print('trying to connect...')\n";
	script << "       de = DesignEnvironment.connect(id)\n";
	script << "       if de.is_connected():\n";
	script << "          print('connected.')\n";
	script << "          prj = de.get_open_project(fileName)\n";
	script << "          prj.close()\n";
	script << "          if len(de.list_open_projects()) == 0:\n";
	script << "             de.close()\n";
	script << "          break\n";
	script << "    except:\n";
	script << "       de = None\n";

	return script.str();
}

std::string StudioConnector::generateOpenScript(const std::string& studioPath, std::string fileName)
{
	std::replace(fileName.begin(), fileName.end(), '/', '\\');

	std::stringstream script;

	script << "fileName = r'" << fileName << "'\n";

	script << "import sys\n";
	script << "sys.path.append(r'" << studioPath << "\\AMD64\\python_cst_libraries')\n";

	script << "from cst.interface import DesignEnvironment\n";

	script << "de = DesignEnvironment.new()\n";
	script << "try:\n";
	script << "   prj = de.open_project(fileName)\n";
	script << "   prj.save();\n";
	script << "   print('project found')\n";
	script << "   de.set_quiet_mode(False)\n";
	script << "except:\n";
	script << "   de.close();\n";
	script << "   raise Exception('Unable to open project')\n";

	return script.str();
}

void StudioConnector::determineStudioSuiteInstallation(int &version, std::string &studioPath)
{
	// First, read the latest version of the installed CST Studio Suite Software

	QSettings registry("HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432node\\CST AG\\CST DESIGN ENVIRONMENT",	QSettings::NativeFormat);

	version = 0;
	for (auto key : registry.childGroups())
	{
		int thisVersion = atoi(key.toStdString().c_str());

		version = std::max(thisVersion, version);
	}

	if (version == 0) throw std::string("CST Studio Suite is not installed on this computer.");
	if (version < 2023) throw std::string("This version of CST Studio Suite is not supported: " + std::to_string(version));


	// Now read the installation path of this version
	QSettings studioKey(QString(("HKEY_LOCAL_MACHINE\\SOFTWARE\\WOW6432node\\CST AG\\CST DESIGN ENVIRONMENT\\" + std::to_string(version)).c_str()), QSettings::NativeFormat);

	studioPath = studioKey.value("INSTALLPATH").toString().toStdString();
}

void StudioConnector::startSubprocess() {
	if (m_subProcessRunning) return;

	// Determine new unique server name
	QUuid uidGenerator = QUuid::createUuid();
	m_serverName = uidGenerator.toString().toStdString();

	m_subprocessPath = findSubprocessPath() + m_executableName;
	initiateProcess();

#ifdef DEBUG_PYTHON_SERVER
	serverName = "TestServer";
#endif
	m_server.listen(m_serverName.c_str());
#ifndef DEBUG_PYTHON_SERVER
	//std::thread workerThread(&StudioConnector::runSubprocess, this);
	//workerThread.detach();

	runSubprocess();
#endif
	connectWithSubprocess();
	
	// Send ping
	ot::JsonDocument pingDoc;
	pingDoc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Ping, pingDoc.GetAllocator()), pingDoc.GetAllocator());
	send(pingDoc.toJson());
		
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Init, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_NAME, ot::JsonString(OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE, doc.GetAllocator()), doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_SESSION_COUNT, 0, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_SERVICE_ID, 0, doc.GetAllocator());   

	ot::JsonArray flagsArr;
	ot::addLogFlagsToJsonArray(ot::LogDispatcher::instance().getLogFlags(), flagsArr, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_LogFlags, flagsArr, doc.GetAllocator());

	ot::ReturnMessage returnMessage = send(doc.toJson());

	if (returnMessage.getStatus() == ot::ReturnMessage::ReturnMessageStatus::Failed) {
		OT_LOG_E("Failed to initialise Python subprocess");
		std::string errorMessage = "Failed to initialise Python subprocess, due to following error: " + returnMessage.getWhat();
		throw std::string(errorMessage);
	}

	m_subProcessRunning = true;
}

std::string StudioConnector::findSubprocessPath()
{
#ifdef _DEBUG
	const std::string otRootFolder = ot::OperatingSystem::getEnvironmentVariable("OPENTWIN_DEV_ROOT");
	assert(otRootFolder != "");
	return otRootFolder + "\\Deployment\\";

#else
	return ".\\";

#endif // DEBUG

}

void StudioConnector::initiateProcess()
{
	QStringList arguments{ QString(m_serverName.c_str()) };
	m_subProcess.setArguments(arguments);

	//Turning off the default communication channels of the process. Communication will be handled via QLocalServer and QLocalSocket
	m_subProcess.setStandardOutputFile(QProcess::nullDevice());
	m_subProcess.setStandardErrorFile(QProcess::nullDevice());
	m_subProcess.setProgram(m_subprocessPath.c_str());
}

void StudioConnector::runSubprocess()
{
	try
	{
		const bool startSuccessfull = startProcess();
		if (!startSuccessfull)
		{
			std::string message = "Starting Python Subprocess timeout.";
			OT_LOG_E(message);
			throw std::string(message.c_str());
		}
		OT_LOG_D("Python Subprocess started");
	}
	catch (std::exception& e)
	{
		const std::string exceptionMessage("Starting the python subprocess failed due to: " + std::string(e.what()) + ". Shutting down.");
		OT_LOG_E(exceptionMessage);
		exit(0);
	}
}

bool StudioConnector::startProcess()
{
	bool processStarted = false;
	for (int i = 1; i <= m_numberOfRetries; i++)
	{
		OT_LOG_D("Starting the subprocess: " + m_subProcess.program().toStdString() + " trial: " + std::to_string(i) + "/" + std::to_string(m_numberOfRetries));
		m_subProcess.start();
		auto state = m_subProcess.state();
		if (state == QProcess::NotRunning)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(m_timeoutSubprocessStart));
		}
		else if (state == QProcess::Starting)
		{
			processStarted = m_subProcess.waitForStarted(m_timeoutSubprocessStart);
			if (processStarted)
			{
				return true;
			}
			else if (m_subProcess.error() != QProcess::Timedout)
			{
				std::string errorMessage;
				getProcessErrorOccured(errorMessage);
				errorMessage = "Error occured while starting Python Subservice: " + errorMessage;
				throw std::string(errorMessage.c_str());
			}
		}
		else
		{
			assert(state == QProcess::Running);
			return true;
		}

	}
	return processStarted;

}

void StudioConnector::connectWithSubprocess()
{
	OT_LOG_D("Waiting for connection servername: " + m_serverName);
	bool connected = m_server.waitForNewConnection(m_timeoutServerConnect);
	if (connected)
	{
		m_socket = m_server.nextPendingConnection();
		m_socket->waitForConnected(m_timeoutServerConnect);
		connected = m_socket->state() == QLocalSocket::ConnectedState;
		if (connected)
		{
			OT_LOG_D("Connection with subservice established");
		}
		else
		{
			std::string errorMessage;
			getSocketErrorOccured(errorMessage);
			errorMessage = "Error occured while connecting with Python Subservice: " + errorMessage;
			throw std::string(errorMessage.c_str());
		}
	}
	else
	{
		throw std::string("Timout while waiting for subprocess to connect with server.");
	}
}

ot::ReturnMessage StudioConnector::executeCommand(const std::string& command)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_PYTHON_EXECUTE_Command, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_CMD_PYTHON_Command, ot::JsonString(command, doc.GetAllocator()), doc.GetAllocator());

	return send(doc.toJson());
}

ot::ReturnMessage StudioConnector::send(const std::string& message) {
	std::string errorMessage;
	bool readyToWrite = checkSubprocessResponsive(errorMessage);
	if (!readyToWrite)
	{
		OT_LOG_E("Python Subservice not responsive. Due to error: " + errorMessage);

		closeSubprocess();
		runSubprocess();
	}
	const std::string messageAsLine = message + "\n";
	m_socket->write(messageAsLine.c_str());
	m_socket->flush();
	bool allIsWritten = m_socket->waitForBytesWritten(m_timeoutSendingMessage);

	if (!waitForResponse())
	{
		return send(message);
	}

	const std::string returnString(m_socket->readLine().data());
	ot::ReturnMessage returnMessage = ot::ReturnMessage::fromJson(returnString);

	return returnMessage;
}

bool StudioConnector::checkSubprocessResponsive(std::string& errorMessage)
{
	bool subProcessResponsive = true;

#ifndef DEBUG_PYTHON_SERVER
	if (m_subProcess.state() != QProcess::Running)
	{
		OT_LOG_E("Sending message failed. Process state: " + m_subProcess.state());
		subProcessResponsive = false;
		getProcessErrorOccured(errorMessage);
	}
#endif // DEBUG_PYTHON_SERVER

	if (!m_socket->isValid())
	{
		OT_LOG_E("Sending message failed. Socket state: " + m_socket->state());
		subProcessResponsive = false;
		getSocketErrorOccured(errorMessage);
	}
	return subProcessResponsive;
}

void StudioConnector::closeSubprocess()
{
	if (m_socket != nullptr)
	{
		//subProcess.kill();
		//delete socket;
		//socket = nullptr;

		m_subProcessRunning = false;

		m_socket->close();
		m_socket->waitForDisconnected(m_timeoutServerConnect);
		m_subProcess.waitForFinished(m_timeoutSubprocessStart);
		OT_LOG_D("Closed Python Subprocess");

		delete m_socket;
		m_socket = nullptr;
	}
}

bool StudioConnector::waitForResponse()
{
	while (!m_socket->canReadLine())
	{
		bool receivedMessage = m_socket->waitForReadyRead(m_heartBeat);
		if (!receivedMessage)
		{
			std::string errorMessage;
			bool subprocessResponsive = checkSubprocessResponsive(errorMessage);
			if (!subprocessResponsive)
			{
				const std::string message = "Python Subservice crashed while waiting for response: " + errorMessage;
//				_uiComponent->displayMessage(message);
				return false;
			}
		}
	}
	return true;
}


void StudioConnector::getProcessErrorOccured(std::string& message)
{
	message = m_subProcess.errorString().toStdString();
}

void StudioConnector::getSocketErrorOccured(std::string& message)
{
	message = m_socket->errorString().toStdString();
}
