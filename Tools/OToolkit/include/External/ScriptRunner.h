// @otlicense
// File: ScriptRunner.h
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

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qprocess.h>

class ScriptRunner : public QObject {
	Q_OBJECT
public:
	static ScriptRunner* runScript(const QString& _name, const QString& _scriptPath, const QStringList& _arguments, const QString& _workingDir, bool _defaultHandling = true);
	static bool runDetached(const QString& _name, const QString& _scriptPath, const QStringList& _arguments, const QString& _workingDir);

	ScriptRunner() = default;

	bool run(const QString& _scriptPath, const QStringList& _arguments, const QString& _workingDir);
	void stop();

Q_SIGNALS:
    void stdOut(const QString& _output);
    void stdErr(const QString& _error);
	void finished(int _exitCode, QProcess::ExitStatus _status);

private Q_SLOTS:
	void slotStdOut();
    void slotStdErr();
    void slotFinished(int _exitCode, QProcess::ExitStatus _status);

private:
	QStringList tidyLog(const QString& _msg);
	
	QProcess* m_process = nullptr;
	QString m_name;
	bool m_defaultHandling = true;
};