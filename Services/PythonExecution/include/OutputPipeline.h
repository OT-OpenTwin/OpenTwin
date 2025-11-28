#pragma once
#include <thread>
#include <atomic>

class OutputPipeline
{
public:
	~OutputPipeline();
	static void setRedirectOutput(bool _redirectOutput) { m_redirectOutput = _redirectOutput; }
	void setupOutputPipeline();
	void initiateRedirect();
	void flushOutput();
private:
	static bool m_redirectOutput;
	std::thread* m_outputWorkerThread = nullptr;
	std::atomic<long long> m_outputProcessingCount = 0;
	int m_pipe_fds[2];

	void readOutput();
};
