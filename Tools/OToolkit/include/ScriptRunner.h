// @otlicense

#pragma once

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qprocess.h>

class ScriptRunner : public QObject {
	Q_OBJECT
public:
	static ScriptRunner* runScript(const QString& _name, const QString& _scriptPath, const QStringList& _arguments, const QString& _workingDir, bool _defaultHandling = true);

	ScriptRunner() = default;

	bool run(const QString& _scriptPath, const QStringList& _arguments, const QString& _workingDir);

Q_SIGNALS:
    void stdOut(const QString& _output);
    void stdErr(const QString& _error);
	void finished(int _exitCode, QProcess::ExitStatus _status);

private Q_SLOTS:
	void slotStdOut();
    void slotStdErr();
    void slotFinished(int _exitCode, QProcess::ExitStatus _status);

private:
	QProcess* m_process = nullptr;
	QString m_name;
	bool m_defaultHandling = true;
};