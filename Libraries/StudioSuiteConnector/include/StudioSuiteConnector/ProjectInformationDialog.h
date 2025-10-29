// @otlicense

#pragma once

// Qt header
#include <qobject.h>
#include <qicon.h>
#include <qstring.h>
#include <qcolor.h>
#include <qlineedit.h>
#include <qtablewidget.h>
#include <qdialog.h>

class QLayout;
class QLabel;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;
class QWidget;
class QMouseEvent;
class QVBoxLayout;
class QHBoxLayout;

class ProjectInformationDialog : public QDialog {
	Q_OBJECT
public:
	ProjectInformationDialog(const std::string &localFileName, const std::string& serverVersion, const std::string& localVersion);
	virtual ~ProjectInformationDialog();

Q_SIGNALS:
	void isClosing(void);

private Q_SLOTS:
	void slotButtonClosePressed();

private:

	void Close(void);

	QPushButton* my_buttonClose;
	QLineEdit* my_fileName;
	QLineEdit* my_serverVersion;
	QLineEdit* my_localVersion;
	QLabel* my_lFileName;
	QLabel* my_lServerVersion;
	QLabel* my_lLocalVersion;
	QVBoxLayout* my_layout;

	QHBoxLayout* my_layoutButtons;
	QWidget* my_widgetButtons;

	QHBoxLayout* my_layoutFileName;
	QWidget* my_widgetFileName;
	QHBoxLayout* my_layoutServerVersion;
	QWidget* my_widgetServerVersion;
	QHBoxLayout* my_layoutLocalVersion;
	QWidget* my_widgetLocalVersion;

	ProjectInformationDialog(const ProjectInformationDialog&) = delete;
	ProjectInformationDialog& operator = (ProjectInformationDialog&) = delete;
};
