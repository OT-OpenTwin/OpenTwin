//! @file Randomizer.h
//! @author Alexander Kuester (alexk95)
//! @date December 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Toolkit API header
#include "OTCore/OTClassHelper.h"
#include "OToolkitAPI/Tool.h"

// OT header
#include <OTWidgets/Dialog.h>

// Qt header
#include <QtCore/qobject.h>

// std header
#include <map>

class QLabel;
class QWidget;
class QSpinBox;
class QGroupBox;
class QCheckBox;
class QLineEdit;
class QTabWidget;
class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;
class QRadioButton;
class QPlainTextEdit;

class Randomizer : public QObject, public otoolkit::Tool {
	Q_OBJECT
public:
	Randomizer();
	virtual ~Randomizer();

	// ###########################################################################################################################################################################################################################################################################################################################

	// API base functions

	//! @brief Return the unique tool name
	//! The name will be used to create all required menu entries
	virtual QString toolName(void) const override { return QString("Randomizer"); };

	//! @brief Create the central widget that will be displayed to the user in the main tab view
	virtual QWidget* runTool(QMenu* _rootMenu, std::list<QWidget*>& _statusWidgets) override;

	virtual void restoreToolSettings(QSettings& _settings) override;

	//! @brief Stop all the logic of this tool
	virtual bool prepareToolShutdown(QSettings& _settings) override;

	const std::map<QString, QPlainTextEdit*>& listEntries(void) const { return m_lists; };

private Q_SLOTS:
	void slotRunBool(void);
	void slotRunText(void);
	void slotRunList(void);
	void slotAddList(void);

private:
	
	QWidget* m_root;
	QVBoxLayout* m_rootLayout;
	QLabel* m_leftTitle;

	QGroupBox* m_boolGroup;
	QVBoxLayout* m_boolLayout;
	QHBoxLayout* m_boolTypeLayout;
	QRadioButton* m_zeroOneRB;
	QRadioButton* m_boolRB;
	QRadioButton* m_yesNoRB;
	QLabel* m_boolResult;
	QPushButton* m_boolRun;

	QGroupBox* m_textGroup;
	QVBoxLayout* m_textLayout;
	QHBoxLayout* m_textTopLayout;
	QCheckBox* m_upperCaseCB;
	QCheckBox* m_lowerCaseCB;
	QCheckBox* m_numbersCB;
	QCheckBox* m_symbolsCB;
	QLineEdit* m_symbolsEdit;
	QHBoxLayout* m_textBottomLayout;
	QLabel* m_textLengthL;
	QSpinBox* m_textLength;
	QPushButton* m_textRun;
	QLineEdit* m_textResult;

	QGroupBox* m_listGroup;
	QVBoxLayout* m_listLayout;
	QTabWidget* m_listTabs;
	std::map<QString, QPlainTextEdit*> m_lists;
	QHBoxLayout* m_addListLayout;
	QPushButton* m_addListBtn;
	QPushButton* m_runList;
	QLineEdit* m_listResult;

};

class RandomizerDialog : public ot::Dialog {
	Q_OBJECT
public:
	RandomizerDialog(Randomizer * _owner, QWidget* _widget);
	virtual ~RandomizerDialog();

	QString currentName(void) const;

private Q_SLOTS:
	void slotConfirm(void);
	void slotCancel(void);

private:
	QVBoxLayout* m_centralLayout;
	QHBoxLayout* m_nameLayout;
	QLabel* m_nameLabel;
	QLineEdit* m_nameEdit;
	QHBoxLayout* m_buttonLayout;
	QPushButton* m_confirmBtn;
	QPushButton* m_cancelBtn;

	Randomizer* m_owner;

	RandomizerDialog() = delete;
};
