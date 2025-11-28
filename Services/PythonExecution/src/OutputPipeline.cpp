#include <windows.h>

#include "OutputPipeline.h"
#include <fcntl.h>
#include "Application.h"
#include <io.h>
#include <OTCore/LogDispatcher.h>
#undef slots
#include "Python.h"

bool OutputPipeline::m_redirectOutput = false;

OutputPipeline::~OutputPipeline()
{
	if (m_outputWorkerThread)
	{
		m_redirectOutput = false;
		m_outputWorkerThread->join();

		delete m_outputWorkerThread;
		m_outputWorkerThread = nullptr;
	}
}

void OutputPipeline::setupOutputPipeline()
{
	if (m_redirectOutput)
	{
		// Create a pipe for getting the standard output
		if (_pipe(m_pipe_fds, 4096, _O_TEXT) == -1) {
			OT_LOG_EA("Creating pipe for capturing Python output failed.");
		}
	}
}

void OutputPipeline::initiateRedirect()
{
	if (m_redirectOutput)
	{
		// Redirect output to pipe
		std::string command =
			"import sys\n"
			"import os\n"
			"import logging\n"
			"sys.stdout = os.fdopen(" + std::to_string(m_pipe_fds[1]) + ", 'w')\n"
			"sys.stdout.reconfigure(line_buffering = True)\n"
			"logging.getLogger().addHandler(logging.StreamHandler(sys.stdout))\n";

		PyRun_SimpleString(command.c_str());

		m_outputWorkerThread = new std::thread(&OutputPipeline::readOutput, this);
	}
}

void OutputPipeline::flushOutput()
{
	if (m_redirectOutput && m_outputWorkerThread != nullptr)
	{
		long long currentOutputProcessingCount = m_outputProcessingCount;

		// Wait until all messages have been sent
		while (m_outputProcessingCount < currentOutputProcessingCount + 2)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
	}
}

void OutputPipeline::readOutput()
{
	char buffer[256];
	while (m_redirectOutput)
	{
		DWORD bytes_available = 0;
		if (PeekNamedPipe((HANDLE)_get_osfhandle(m_pipe_fds[0]), NULL, 0, NULL, &bytes_available, NULL)) {
			if (bytes_available > 0) {
				int count = _read(m_pipe_fds[0], buffer, sizeof(buffer) - 1);
				if (count > 0) {
					buffer[count] = '\0';
					//Application::instance().getCommunicationHandler().writeToServer("OUTPUT:" + std::to_string(strlen(buffer)) + ":" + std::string(buffer));
					Application::instance().getCommunicationHandler().writeToServer("OUTPUT:" + std::string(buffer));
				}
			}
		}

		m_outputProcessingCount++;

		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
}
