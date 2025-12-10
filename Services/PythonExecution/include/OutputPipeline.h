// @otlicense
// File: OutputPipeline.h
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

// std header
#include <thread>
#include <atomic>
#include <string>
#include <mutex>

class OutputPipeline
{
public:
	static OutputPipeline& instance()
	{
		static OutputPipeline g_instance;
		return g_instance;
	}

	enum class RedirectionMode
	{
		off,
		sendToServer,
		applicationRead
	};

	~OutputPipeline();
	void setRedirectOutputMode(RedirectionMode _mode);
	void setupOutputPipeline();
	void initiateRedirect();
	std::string flushOutput();
	
private:
	OutputPipeline() = default;
	RedirectionMode m_redirectionMode = RedirectionMode::off;

	std::thread* m_outputWorkerThread = nullptr;
	std::atomic<long long> m_outputProcessingCount = 0;
	int m_pipe_fds[2] = { 0,0 };
	std::string m_outputBuffer;
	void addToOutputBuffer(const std::string& _addedText)
	{
		std::lock_guard<std::mutex> guard(m_bufferMutex);
		m_outputBuffer += _addedText;
	}

	std::string flushOutputBuffer()
	{
		std::lock_guard<std::mutex> guard(m_bufferMutex);
		std::string output = m_outputBuffer;
		m_outputBuffer = "";
		return output;
	}
	void checkForEndToken()
	{
		std::lock_guard<std::mutex> guard(m_bufferMutex);
		if (m_outputBuffer.find("<END>") != std::string::npos)
		{
			m_endTokenDetected.notify_one();
		}
	}


	std::condition_variable m_endTokenDetected;
	std::mutex m_endTokenMutex, m_bufferMutex;


	void readOutput();
	void bufferOutput();
};
