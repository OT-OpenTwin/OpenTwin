#pragma once

#pragma once

// Qt header
#include <qobject.h>
#include <qicon.h>
#include <qstring.h>
#include <qcolor.h>
#include <qlineedit.h>
#include <qtablewidget.h>
#include <qdialog.h>

// AK header
#include <akWidgets/aWidget.h>

class QLayout;
class QLabel;
class QPushButton;
class QTableWidget;
class QTableWidgetItem;
class QWidget;
class QMouseEvent;
class QVBoxLayout;
class QHBoxLayout;

class CommitMessageDialog : public QDialog {
	Q_OBJECT
public:
	CommitMessageDialog(const QIcon& windowIcon);
	virtual ~CommitMessageDialog();

	QString changeMessage(void);

	bool wasConfirmed(void) const { return my_confirmed; }

	void setNotConfirmed(void) { my_confirmed = false; }

	void cancelClose(void) { my_cancelClose = true; }

	void reset(const QString& _projectToCopy);

signals:
	void isClosing(void);

private slots:
	void slotButtonConfirmPressed();
	void slotButtonCancelPressed();
	void slotReturnPressed();

private:

	void Close(void);

	bool			my_confirmed;
	bool			my_cancelClose;

	QString			my_projectToCopy;

	QPushButton* my_buttonConfirm;
	QPushButton* my_buttonCancel;
	QLineEdit* my_input;
	QLabel* my_label;
	QVBoxLayout* my_layout;

	QHBoxLayout* my_layoutButtons;
	QWidget* my_widgetButtons;

	QHBoxLayout* my_layoutInput;
	QWidget* my_widgetInput;

	CommitMessageDialog(const CommitMessageDialog&) = delete;
	CommitMessageDialog& operator = (CommitMessageDialog&) = delete;
};
