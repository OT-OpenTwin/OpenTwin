#pragma once
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
