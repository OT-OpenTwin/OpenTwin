// @otlicense
// File: OutputPipeline.cpp
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

#include <windows.h>

#include "OutputPipeline.h"
#include <fcntl.h>
#include "Application.h"
#include <io.h>
#include <OTCore/LogDispatcher.h>
#undef slots
#include "Python.h"

OutputPipeline::~OutputPipeline()
{
	if (m_outputWorkerThread)
	{
		m_redirectionMode = RedirectionMode::off;
		m_outputWorkerThread->join();

		delete m_outputWorkerThread;
		m_outputWorkerThread = nullptr;
	}
}

void OutputPipeline::setRedirectOutputMode(RedirectionMode _mode)
{
	if (_mode != m_redirectionMode)
	{
		m_redirectionMode = _mode;
		if (m_outputWorkerThread != nullptr)
		{
			m_outputWorkerThread->join();
			delete m_outputWorkerThread;
			m_outputWorkerThread = nullptr;
		}

		if (_mode == RedirectionMode::sendToServer)
		{
			m_outputWorkerThread = new std::thread(&OutputPipeline::readOutput, this);
		}
		else if(_mode == RedirectionMode::applicationRead)
		{
			m_outputWorkerThread = new std::thread(&OutputPipeline::bufferOutput, this);
		}
	}
}

void OutputPipeline::setupOutputPipeline()
{
	if (m_redirectionMode != RedirectionMode::off)
	{
		// Create a pipe for getting the standard output
		if (_pipe(m_pipe_fds, 4096, _O_TEXT) == -1) {
			OT_LOG_EA("Creating pipe for capturing Python output failed.");
		}
	}
}

void OutputPipeline::initiateRedirect()
{
	if (m_redirectionMode != RedirectionMode::off)
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

std::string OutputPipeline::flushOutput()
{
	if (m_redirectionMode == RedirectionMode::sendToServer && m_outputWorkerThread != nullptr)
	{
		long long currentOutputProcessingCount = m_outputProcessingCount;

		// Wait until all messages have been sent
		while (m_outputProcessingCount < currentOutputProcessingCount + 2)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10));
		}
		return "";
	}
	else if (m_redirectionMode == RedirectionMode::applicationRead && m_outputWorkerThread != nullptr)
	{
		std::string endToken= "<END>";
		int written = _write(m_pipe_fds[1], endToken.c_str(), static_cast<uint32_t>(endToken.size()));

		if (written < 0) {
			perror("_write failed");
		}
		std::unique_lock<std::mutex>lock(m_endTokenMutex);
		m_endTokenDetected.wait(lock);

		
		std::string output = flushOutputBuffer();
		auto pos = output.find(endToken);
		output = output.substr(0,pos);
		return output;
	}
	else
	{
		return "";
	}
}

void OutputPipeline::readOutput()
{
	char buffer[256];
	while (m_redirectionMode == RedirectionMode::sendToServer)
	{
		DWORD bytes_available = 0;
		if (PeekNamedPipe((HANDLE)_get_osfhandle(m_pipe_fds[0]), NULL, 0, NULL, &bytes_available, NULL)) {
			if (bytes_available > 0) {
				int count = _read(m_pipe_fds[0], buffer, sizeof(buffer) - 1);
				if (count > 0) {
					assert(count < 256);
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

void OutputPipeline::bufferOutput()
{
	char buffer[256];
	while (m_redirectionMode == RedirectionMode::applicationRead)
	{
		DWORD bytes_available = 0;
		if (PeekNamedPipe((HANDLE)_get_osfhandle(m_pipe_fds[0]), NULL, 0, NULL, &bytes_available, NULL)) {
			if (bytes_available > 0) {
				int count = _read(m_pipe_fds[0], buffer, sizeof(buffer) - 1);
				if (count > 0) {
					buffer[count] = '\0';
					addToOutputBuffer(buffer);
					checkForEndToken();
				}
			}
		}

		m_outputProcessingCount++;

		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
}
