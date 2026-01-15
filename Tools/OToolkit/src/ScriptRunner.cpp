// @otlicense

// OpenTwin header
#include "ScriptRunner.h"
#include "OToolkitAPI/OToolkitAPI.h"

#define SCRIPT_LOG(___message) OTOOLKIT_LOG("Script", ___message)
#define SCRIPT_LOGW(___message) OTOOLKIT_LOGW("Script", ___message)
#define SCRIPT_LOGE(___message) OTOOLKIT_LOGE("Script", ___message)

ScriptRunner* ScriptRunner::runScript(const QString& _name, const QString& _scriptPath, const QStringList& _arguments, const QString& _workingDir, bool _defaultHandling) {
	SCRIPT_LOG("Starting script \"" + _name + "\"");
	ScriptRunner* runner = new ScriptRunner;
	runner->m_defaultHandling = _defaultHandling;
	runner->m_name = _name;
	if (!runner->run(_scriptPath, _arguments, _workingDir)) {
		delete runner;
		return nullptr;
	}
	return runner;
}

bool ScriptRunner::run(const QString& _scriptPath, const QStringList& _arguments, const QString& _workingDir) {
	if (m_process) {
		SCRIPT_LOGE("Script is already running.");
		return false;
	}

	QString cmd;
	QStringList args(_arguments);

#ifdef OT_OS_WINDOWS
	cmd = "cmd.exe";
	args.prepend(_scriptPath);
	args.prepend("/C");
#endif

#ifdef OT_OS_MAC
	cmd = "/bin/bash";
	args.prepend(_batchFilePath);
#endif

#ifdef OT_OS_LINUX
	cmd = "/bin/sh";
	args.prepend(_batchFilePath);
#endif

	m_process = new QProcess(this);
	if (!_workingDir.isEmpty()) {
		m_process->setWorkingDirectory(_workingDir);
	}
	connect(m_process, &QProcess::readyReadStandardOutput, this, &ScriptRunner::slotStdOut);
	connect(m_process, &QProcess::readyReadStandardError, this, &ScriptRunner::slotStdErr);
	connect(m_process, &QProcess::finished, this, &ScriptRunner::slotFinished);

	m_process->start(cmd, args);

	return true;
}

void ScriptRunner::slotStdOut() {
	QString msg = QString::fromUtf8(m_process->readAllStandardOutput());
	Q_EMIT stdOut(msg);
	if (m_defaultHandling) {
		while (msg.endsWith('\n') || msg.endsWith('\r')) {
			msg.chop(1);
		}
		SCRIPT_LOG("[" + m_name + "] " + msg);
	}	 
}

void ScriptRunner::slotStdErr() {
	QString msg = QString::fromUtf8(m_process->readAllStandardError());
	Q_EMIT stdErr(msg);
	if (m_defaultHandling) {
		while (msg.endsWith('\n') || msg.endsWith('\r')) {
			msg.chop(1);
		}
		SCRIPT_LOGE("[" + m_name + "] " + msg);
	}
}

void ScriptRunner::slotFinished(int _exitCode, QProcess::ExitStatus _status) {
	Q_EMIT finished(_exitCode, _status);
	if (m_defaultHandling) {
		SCRIPT_LOG("[" + m_name + "] Script finished with exit code " + QString::number(_exitCode) + ".");
		this->deleteLater();
	}
}
