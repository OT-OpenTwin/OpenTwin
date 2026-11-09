// @otlicense
// File: TestToolBar.cpp
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

// OpenTwin header
#include "Tools/WidgetTest/TestToolBar.h"
#include "OTWidgets/Widgets/MainWindow.h"
#include "OTWidgets/ToolBar/TabToolBar.h"

// Qt header
#include <QtCore/qfile.h>
#include <QtWidgets/qmessagebox.h>
#include <QtWidgets/qfiledialog.h>

TestToolBar::TestToolBar(WidgetTest* _owner) :
	m_owner(_owner), m_window(nullptr), m_centralWidgetManager(nullptr), m_toolBarManager(nullptr)
{

}

void TestToolBar::slotTest()
{
	using namespace ot;

	QString fileName = QFileDialog::getSaveFileName(m_window, "Select file to save", "", "All Files (*.*)");
	if (fileName.isEmpty()) return;

	QFile file(fileName);
	if (!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::critical(m_window, "Error", "Could not open file for writing: " + file.errorString());
		return;
	}

	for (int i = 0; i < 10000; i++)
	{
		QByteArray prefix = "Test_" + QByteArray::number(i) + "_";
		file.write(
			"// @seq dia name = \"" + prefix + "Test\"  func = \"" + prefix + "Func 1\"\n"
			"// @seq func life=\"" + prefix + "Life 1\" name=\"" + prefix + "Func 1\" default=\"Call\"\n"
			"// @seq call " + prefix + "Func 2\n"
			"// @seq func life=\"" + prefix + "Life 2\" name=\"" + prefix + "Func 2\" default=\"Call 2\"\n"
			"// @seq call func=\"" + prefix + "Func 3\" text=\"Calling Func 3\"\n"
			"// @seq return Nothing\n"
			"// @seq func life=\"" + prefix + "Life 2\" name=\"" + prefix + "Func 2.1\" default=\"Call 3\"\n"
			"// @seq call " + prefix + "Func 4\n"
			"// @seq func life=\"" + prefix + "Life 3\" name=\"" + prefix + "Func 3\"\n"
			"// @seq call func=\"" + prefix + "Func 2.1\" text=\"" + prefix + "Calling Func 2.1\"\n"
			"// @seq return Nothing\n"
			"// @seq func life=\"" + prefix + "Life 4\" name=\"" + prefix + "Func 4\" default=\"Call 4\"\n"
			"// @seq call " + prefix + "Func 5\n"
			"// @seq func life=\"" + prefix + "Life 4\" name=\"" + prefix + "Func 5\" default=\"Call 5\"\n"
			"// @seq func life=\"" + prefix + "Life X\" name=\"" + prefix + "Func 6\" default=\"Call Unexist\"\n"
			"// @seq call " + prefix + "Func Unexist\n"
		);
	}
	file.close();
}
