/*
 *	File:		aWindowManager.cpp
 *	Package:	akWidgets
 *
 *  Created on: January 30, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// AK header
#include <akCore/aException.h>
#include <akCore/aMessenger.h>
#include <akCore/aUidMangager.h>
#include <akCore/rJSON.h>

#include <akGui/aColorStyle.h>
#include <akGui/aIconManager.h>
#include <akGui/aObjectManager.h>
#include <akGui/aTtbContainer.h>

#include <akWidgets/aDockWidget.h>
#include <akWidgets/aTtbPage.h>
#include <akWidgets/aWindow.h>
#include <akWidgets/aWindowManager.h>
#include <akWidgets/aWindowManagerTimerSignalLinker.h>

// TabToolBar header
#include <TabToolbar/TabToolbar.h>				// tt::TabToolbar
#include <TabToolbar/Page.h>					// tt::Page

// Qt header
#include <qwidget.h>							// QWidget
#include <qprogressbar.h>						// QProgressBar
#include <qstatusbar.h>							// QStatusBar
#include <qlabel.h>								// QLabel
#include <qtimer.h>								// QTimer
#include <qmessagebox.h>						// QMessageBox
#include <qbytearray.h>							// QByteArray
#include <qmovie.h>
#include <qtabwidget.h>
#include <qtabbar.h>


// m_window->resizeDocks({ dock }, { 0 }, Qt::Horizontal); // This is the hack

ak::aWindowManager::aWindowManager(
	aMessenger *									_messenger,
	aUidManager *								_uidManager,
	aColorStyle *							_colorStyle
) : aPaintable(otMainWindow),
m_window(nullptr),
m_messenger(nullptr),
m_uidManager(nullptr),
m_tabToolBar(nullptr),
m_statusLabel(nullptr),
m_progressBar(nullptr),
m_timerLabelHide(nullptr),
m_timerLabelShow(nullptr),
m_timerProgressHide(nullptr),
m_timerProgressShow(nullptr),
m_timerSignalLinker(nullptr),
m_timerShowMainWindow(nullptr)
{
	// Check parameter
	assert(_messenger != nullptr); // Nullptr provided
	assert(_uidManager != nullptr); // Nullptr provided
	m_messenger = _messenger;
	m_uidManager = _uidManager;
	m_uid = m_uidManager->getId();
	m_colorStyle = _colorStyle;

	// Create main window
	m_window = new aWindow();
	m_window->setAutoFillBackground(true);
	m_window->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);

	// Create tab Toolbar
	m_tabToolBar = new tt::TabToolbar();
	m_tabToolBar->setVisible(false);
	m_tabToolBar->setContextMenuPolicy(Qt::ContextMenuPolicy::NoContextMenu);
	m_window->addToolBar(m_tabToolBar);

	// Connect tab Toolbar tab signals
	connect(m_tabToolBar, SIGNAL(tabClicked(int)), this, SLOT(slotTabToolbarTabClicked(int)));
	connect(m_tabToolBar, SIGNAL(currentTabChanged(int)), this, SLOT(slotTabToolbarTabCurrentTabChanged(int)));

	// Create progressbar
	m_progressBar = new QProgressBar;
	m_progressBar->setMinimumWidth(180);
	m_progressBar->setMaximumWidth(180);
	m_progressBar->setMinimum(0);
	m_progressBar->setValue(0);
	m_progressBar->setMaximum(100);
	m_progressBar->setVisible(false);

	// Create status label
	m_statusLabel = new QLabel;
	m_statusLabel->setMinimumWidth(250);
	m_statusLabel->setMaximumWidth(250);
	m_statusLabel->setText(QString());

	m_window->statusBar()->addPermanentWidget(m_progressBar, 180);
	m_window->statusBar()->addPermanentWidget(m_statusLabel, 250);
	m_window->statusBar()->setVisible(true);

	// Setup timer
	m_timerLabelHide = new QTimer;
	m_timerLabelShow = new QTimer;
	m_timerProgressHide = new QTimer;
	m_timerProgressShow = new QTimer;
	m_timerShowMainWindow = new QTimer;
	m_timerLabelHide->setInterval(1500);
	m_timerLabelShow->setInterval(3000);
	m_timerProgressHide->setInterval(1500);
	m_timerProgressShow->setInterval(3000);
	m_timerShowMainWindow->setInterval(1);
	m_timerShowMainWindow->setSingleShot(true);

	// Create timer signal linker
	m_timerSignalLinker = new aWindowManagerTimerSignalLinker(this);
	
	// Link timer to the corresponding functions (realised via the timer type) 
	m_timerSignalLinker->addLink(m_timerLabelHide, ak::aWindowManagerTimerSignalLinker::timerType::statusLabelHide);
	m_timerSignalLinker->addLink(m_timerLabelShow, ak::aWindowManagerTimerSignalLinker::timerType::statusLabelShow);
	m_timerSignalLinker->addLink(m_timerProgressHide, ak::aWindowManagerTimerSignalLinker::timerType::progressHide);
	m_timerSignalLinker->addLink(m_timerProgressShow, ak::aWindowManagerTimerSignalLinker::timerType::progressShow);
	m_timerSignalLinker->addLink(m_timerShowMainWindow, ak::aWindowManagerTimerSignalLinker::timerType::showWindow);

	if (m_colorStyle != nullptr) { setColorStyle(m_colorStyle); }

	// Show main window
	//m_timerShowMainWindow->start();
	m_window->resize(800, 600);
	//	m_window->showMaximized();
}

ak::aWindowManager::~aWindowManager() {
	A_OBJECT_DESTROYING
	// Delete the timer signal linker first, so all objects will be disconnected propertly
	if (m_timerSignalLinker != nullptr) { delete m_timerSignalLinker; m_timerSignalLinker = nullptr; }

}

void ak::aWindowManager::setColorStyle(
	aColorStyle *								_colorStyle
) {
	assert(_colorStyle != nullptr); // nullptr provided
	m_colorStyle = _colorStyle;

	m_window->setColorStyle(_colorStyle);

	m_progressBar->setStyleSheet(m_colorStyle->toStyleSheet(cafForegroundColorWindow |
		cafBackgroundColorWindow));
	m_statusLabel->setStyleSheet(m_colorStyle->toStyleSheet(cafForegroundColorWindow |
		cafBackgroundColorTransparent));

	// TTB
	QString sheet(m_colorStyle->toStyleSheet(cafForegroundColorWindow |
		cafBackgroundColorWindow, "QToolBar{border: 0px;", "}"));
	if (sheet.isEmpty()) {
		sheet = "QToolBar{border: 0px;}";
	}
	m_tabToolBar->SetStylesheet(sheet);
	
	sheet = m_colorStyle->toStyleSheet(cafForegroundColorWindow |
		cafBackgroundColorWindow, "QTabWidget{", "}");
	sheet.append(m_colorStyle->toStyleSheet(cafBorderColorHeader, "QTabWidget::pane{border: 0px solid; border-top-width: 1px; border-bottom-width: 1px;", "}\n"));
	sheet.append(m_colorStyle->toStyleSheet(cafBackgroundColorHeader | cafForegroundColorHeader,
		"QTabWidget::tab-bar{", "}\n"));
	sheet.append(m_colorStyle->toStyleSheet(cafBackgroundColorHeader | cafForegroundColorHeader,
		"QTabBar::tab{", "}\n"));
	sheet.append(m_colorStyle->toStyleSheet(cafBackgroundColorFocus | cafForegroundColorFocus,
		"QTabBar::tab:hover{", "}\n"));
	sheet.append(m_colorStyle->toStyleSheet(cafBackgroundColorSelected | cafForegroundColorSelected,
		"QTabBar::tab:selected{", "}"));
	
	if (sheet.length() == 0) {
		// Provide manual stylesheet to fix styling issue in toolBar
		sheet = "QTabWidget{}\n"
				"QTabWidget::pane{border: 0px solid #707070; border-top-width: 1px; border-bottom-width: 1px;}\n"
			;
	}

	m_tabToolBar->SetTabBarStylesheet(sheet);
	m_tabToolBar->SetHideButtonStylesheet(m_colorStyle->toStyleSheet(cafForegroundColorWindow |
		cafBackgroundColorWindow));
}

void ak::aWindowManager::removeChildObject(
	aObject *								_child
) {
	aObject::removeChildObject(_child);
	if (_child->type() == otTabToolbarPage) {
		aTtbPage * page = nullptr;
		page = dynamic_cast<aTtbPage *>(_child);
		assert(page != nullptr); // Cast failed
		m_tabToolBar->DestroyPage(page->index());
		for (int i = m_tabToolBarContainer.size() - 1; i >= 0; i--) {
			if (m_tabToolBarContainer[i] == page) {
				m_tabToolBarContainer.erase(m_tabToolBarContainer.begin() + i);
			}
		}
	}
	else {
		m_window->takeCentralWidget();
		m_window->SetCentralWidget(nullptr);
	}
}

void ak::aWindowManager::SetObjectName(
	const QString &							_alias
) {
	m_window->setObjectName(_alias);
	m_window->statusBar()->setObjectName(_alias + "__StatusBar");
	m_progressBar->setObjectName(_alias + "__ProgressBar");
	m_statusLabel->setObjectName(_alias + "__StatusLabel");
	m_tabToolBar->setObjectName(_alias + "__TTB");
}

void ak::aWindowManager::setCentralWidget(
	QWidget *														_centralWidget
) {
	assert(_centralWidget != nullptr); // nullptr provided
	m_window->takeCentralWidget();
	m_window->SetCentralWidget(_centralWidget);
}

// #############################################################################################################

// Dock management

void ak::aWindowManager::addDock(
	QDockWidget*										_dock,
	dockLocation								_dockLocation
) {
	try {
		if (_dock == nullptr) { throw aException("Is nullptr", "Check dock"); }
		switch (_dockLocation)
		{
		case dockLeft:
			m_window->addDockWidget(Qt::LeftDockWidgetArea, _dock);
			break;
		case dockRight:
			m_window->addDockWidget(Qt::RightDockWidgetArea, _dock);
			break;
		case dockBottom:
			m_window->addDockWidget(Qt::BottomDockWidgetArea, _dock);
			break;
		default:
			assert(0); // Not implemented yet
			break;
		}
		m_window->resizeDocks({ _dock }, { 0 }, Qt::Orientation::Vertical);
		m_window->resizeDocks({ _dock }, { 0 }, Qt::Orientation::Horizontal);
		//_dock->resize(200, 200);
	}
	catch (const aException & e) { throw aException(e, "ak::aWindowManager::addDock()"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::aWindowManager::addDock()"); }
	catch (...) { throw aException("Unknown error", "ak::aWindowManager::addDock()"); }
}

void ak::aWindowManager::tabifyDock(
	QDockWidget*							_mainDock,
	QDockWidget*							_subDock
) {
	assert(_mainDock != nullptr); // Cast failed
	assert(_subDock != nullptr); // Cast failed

	// tabify dock
	m_window->tabifyDockWidget(_mainDock, _subDock);
	//_subDock->resize(200, 200);
	m_window->resizeDocks({ _subDock }, { 0 }, Qt::Orientation::Vertical);
	m_window->resizeDocks({ _subDock }, { 0 }, Qt::Orientation::Horizontal);
	//_subDock->resize(200, 200);
	_mainDock->raise();
}

void ak::aWindowManager::setDockPriorityBottomLeft(
	dockLocation						_dockLocation
) {
	try {
		switch (_dockLocation)
		{
		case dockLeft:
			m_window->setCorner(Qt::Corner::BottomLeftCorner, Qt::DockWidgetArea::LeftDockWidgetArea); break;
		case dockBottom:
			m_window->setCorner(Qt::Corner::BottomLeftCorner, Qt::DockWidgetArea::BottomDockWidgetArea); break;
		default:
			throw aException("Invalid dock location", "Check dock location");
		}
	}
	catch (const aException & e) { throw aException(e, "ak::aWindowManager::setDockPriorityBottomLeft()"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::aWindowManager::setDockPriorityBottomLeft()"); }
	catch (...) { throw aException("Unknown error", "ak::aWindowManager::setDockPriorityBottomLeft()"); }
}

void ak::aWindowManager::setDockPriorityBottomRight(
	dockLocation						_dockLocation
) {
	try {
		switch (_dockLocation)
		{
		case dockRight:
			m_window->setCorner(Qt::Corner::BottomRightCorner, Qt::DockWidgetArea::RightDockWidgetArea); break;
		case dockBottom:
			m_window->setCorner(Qt::Corner::BottomRightCorner, Qt::DockWidgetArea::BottomDockWidgetArea); break;
		default:
			throw aException("Invalid dock location", "Check dock location");
		}
	}
	catch (const aException & e) { throw aException(e, "ak::aWindowManager::setDockPriorityBottomLeft()"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::aWindowManager::setDockPriorityBottomLeft()"); }
	catch (...) { throw aException("Unknown error", "ak::aWindowManager::setDockPriorityBottomLeft()"); }
}

// #############################################################################################################

// Status

void ak::aWindowManager::setStatusBarProgress(
	int											_progress
) {
	if (_progress < 0 || _progress > 100) { throw aException("Progress out of range", "ak::aWindowManager::setStatusProgress()"); }
	setStatusBarContinuous(false);
	m_progressBar->setValue(_progress);
}

void ak::aWindowManager::setStatusBarVisible(
	bool										_visible,
	bool										_showDelayed
) {
	if (_visible) {
		m_timerProgressShow->stop();
		if (m_timerProgressHide->isActive()) {
			m_timerProgressHide->stop();
			m_progressBar->setVisible(false);
		}
		if (_showDelayed) {
			m_timerProgressShow->start();
		}
		else { m_progressBar->setVisible(true); }
	}
	else {
		m_timerProgressHide->stop();
		if (m_timerProgressShow->isActive()) {
			m_timerProgressShow->stop();
			m_progressBar->setVisible(true);
		}
		if (_showDelayed) {
			m_timerProgressHide->start();
		}
		else { m_progressBar->setVisible(false); }
	}
}

void ak::aWindowManager::setStatusBarContinuous(
	bool										_continuos
) {
	m_progressBarContinuous = _continuos;
	if (m_progressBarContinuous) {
		m_progressBar->setValue(0);
		m_progressBar->setRange(0, 0);
	}
	else {
		m_progressBar->setRange(0, 100);
	}
}

bool ak::aWindowManager::getStatusBarVisible(void) const { return m_progressBar->isVisible(); }

bool ak::aWindowManager::getStatusLabelVisible(void) const { return m_statusLabel->isVisible(); }

QString ak::aWindowManager::getStatusLabelText(void) const { return m_statusLabel->text(); }

int ak::aWindowManager::getStatusBarProgress(void) const { return m_progressBar->value(); }

bool ak::aWindowManager::getStatusBarContinuous(void) const { return m_progressBarContinuous; }

void ak::aWindowManager::setStatusLabelText(
	const QString &														_status
) { m_statusLabel->setText(_status); }

void ak::aWindowManager::setStatusLabelVisible(
	bool																_visible,
	bool																_showDelayed
) {
	if (_visible) {
		m_timerLabelShow->stop();
		if (m_timerLabelHide->isActive()) {
			m_timerLabelHide->stop();
			m_statusLabel->setVisible(false);
		}
		if (_showDelayed) {
			m_timerLabelShow->start();
		}
		else { m_statusLabel->setVisible(true); }
	}
	else {
		m_timerLabelHide->stop();
		if (m_timerLabelShow->isActive()) {
			m_timerLabelShow->stop();
			m_statusLabel->setVisible(true);
		}
		if (_showDelayed) {
			m_timerLabelHide->start();
		}
		else { m_statusLabel->setVisible(false); }
	}
}

void ak::aWindowManager::showMaximized(void) { m_window->showMaximized(); }

void ak::aWindowManager::showMinimized(void) { m_window->showMinimized(); }

void ak::aWindowManager::showNormal(void) { m_window->showNormal(); }

void ak::aWindowManager::close(void) { m_window->close(); }

void ak::aWindowManager::resize(
	int		_width,
	int		_height
) { m_window->resize(_width, _height); }

std::string ak::aWindowManager::saveState(void) {
	AK_rJSON_createDOC(doc);
	rJSON::add(doc, "ObjectName", m_window->objectName().toStdString());
	rJSON::add(doc, "IsMaximized", m_window->isMaximized());
	rJSON::add(doc, "IsMinimized", m_window->isMinimized());
	rJSON::add(doc, "SizeWidth", m_window->size().width());
	rJSON::add(doc, "SizeHeight", m_window->size().height());
	rJSON::add(doc, "PositionX", m_window->pos().x());
	rJSON::add(doc, "PositionY", m_window->pos().y());

	QByteArray lastConfig(m_window->saveState());
	AK_rJSON_createValueArray(stat);
	for (auto itm : lastConfig) {
		stat.PushBack((int)itm, doc.GetAllocator());
	}
	rJSON::add(doc, "State", stat);

	return rJSON::toJSON(doc);
}

bool ak::aWindowManager::restoreState(
	const std::string &									_state,
	bool												_setPositionAndSize
) {
	if (_state.length() == 0) { return false; }
	AK_rJSON_parseDOC(doc, _state.c_str());

	AK_rJSON_ifNoMember(doc, "ObjectName") { return false; }
	AK_rJSON_ifNoMember(doc, "IsMaximized") { return false; }
	AK_rJSON_ifNoMember(doc, "IsMinimized") { return false; }
	AK_rJSON_ifNoMember(doc, "State") { return false; }
	AK_rJSON_ifNoMember(doc, "SizeWidth") { return false; }
	AK_rJSON_ifNoMember(doc, "SizeHeight") { return false; }
	AK_rJSON_ifNoMember(doc, "PositionX") { return false; }
	AK_rJSON_ifNoMember(doc, "PositionY") { return false; }

	if (m_window->objectName() != rJSON::getString(doc, "ObjectName").c_str()) { assert(0); return false; }	// Invalid object name
	bool isMax = rJSON::getBool(doc, "IsMaximized");
	bool isMin = rJSON::getBool(doc, "IsMinimized");
	auto state = doc["State"].GetArray();

	// Create state string
	QByteArray actualState;
	for (auto itm = state.Begin(); itm != state.End(); itm++) {
		assert(itm->IsInt());	// Array contains a non integer value
		actualState.push_back(itm->GetInt());
	}

	if (_setPositionAndSize) {
		if (!isMax) {
			QSize size{ doc["SizeWidth"].GetInt(), doc["SizeHeight"].GetInt() };
			m_window->showNormal();
			m_window->resize(size);
		}
		else {
			m_window->showMaximized();
		}

		QPoint pos{ doc["PositionX"].GetInt(), doc["PositionY"].GetInt() };
		//m_window->move(pos);

		if (isMin) {
			m_window->showMinimized();
		}
	}

	slotRestoreSetting(actualState);

	return true;
}

void ak::aWindowManager::setShowStatusObjectDelayTimerInterval(int _interval) {
	bool labelShow = false;
	bool progressShow = false;
	// Check if timer is already running
	if (m_timerLabelShow->isActive()) { m_timerLabelShow->stop(); labelShow = true; }
	if (m_timerProgressShow->isActive()) { m_timerProgressShow->stop(); progressShow = true; }
	// Set new interval
	m_timerLabelShow->setInterval(_interval);
	m_timerProgressShow->setInterval(_interval);
	// Reastart timer if needed
	if (labelShow) { m_timerLabelShow->start(); }
	if (progressShow) { m_timerProgressShow->start(); }
}

void ak::aWindowManager::setHideStatusObjectDelayTimerInterval(int _interval) {
	bool labelHide = false;
	bool progressHide = false;
	// Check if timer is already running
	if (m_timerLabelHide->isActive()) { m_timerLabelHide->stop(); labelHide = true; }
	if (m_timerProgressHide->isActive()) { m_timerProgressHide->stop(); progressHide = true; }
	// Set new interval
	m_timerLabelHide->setInterval(_interval);
	m_timerProgressHide->setInterval(_interval);
	// Reastart timer if needed
	if (labelHide) { m_timerLabelHide->start(); }
	if (progressHide) { m_timerProgressHide->start(); }
}

int ak::aWindowManager::getShowStatusObjectDelayTimerInterval(void) const {
	return m_timerLabelShow->interval();
}

int ak::aWindowManager::getHideStatusObjectDelayTimerInterval(void) const {
	return m_timerLabelHide->interval();
}

void ak::aWindowManager::setWaitingAnimationVisible(
	bool									_visible
) { m_window->setWaitingAnimationVisible(_visible); }

void ak::aWindowManager::setWaitingAnimation(
	QMovie *							_movie
) { m_window->setWaitingAnimation(_movie); }

// #############################################################################################################

void ak::aWindowManager::setTabToolBarVisible(
	bool						_vis
) {
	m_tabToolBar->setVisible(_vis);
}

void ak::aWindowManager::setTabToolbarDoubleClickEnabled(
	bool						_isEnabled
) {
	m_tabToolBar->SetAllowDoubleClickOnTab(_isEnabled);
}

ak::aTtbPage * ak::aWindowManager::createTabToolbarSubContainer(
	const QString &				_text
) {
	tt::Page * page = m_tabToolBar->AddPage(_text);
	ak::aTtbPage * p = new ak::aTtbPage(m_messenger, page, _text);
	if (m_colorStyle != nullptr) { p->setColorStyle(m_colorStyle); setColorStyle(m_colorStyle); }
	m_tabToolBarContainer.push_back(p);
	resortTabToolBarTabOrder();
	return p;
}

ak::aTtbContainer * ak::aWindowManager::getTabToolBarSubContainer(
	const QString &				_text
) {
	for (auto itm : m_tabToolBarContainer) {
		if (itm->text() == _text) {
			return itm;
		}
	}
	return nullptr;
}

QString ak::aWindowManager::getTabToolBarTabText(ak::ID _tabId) {
	return m_tabToolBar->tabWidget()->tabText(_tabId);
}

void ak::aWindowManager::addTabToolbarWidget(
	ak::UID						_parentUid,
	ak::UID						_objectUid
) {
	assert(0); // Not implemented yet
}

ak::ID ak::aWindowManager::currentTabToolbarTab(void) const { return m_tabToolBar->CurrentTab(); }

int ak::aWindowManager::tabToolbarTabCount(void) const { return m_tabToolBar->TabCount(); }

void ak::aWindowManager::enableTabToolbar(bool flag) const { return m_tabToolBar->setEnabled(flag); }

void ak::aWindowManager::setCurrentTabToolBarTab(
	ak::ID						_tabID
) {
	assert(_tabID >= 0 && _tabID < m_tabToolBar->TabCount());	// Index out of range
	m_tabToolBar->SetCurrentTab(_tabID);
}

void ak::aWindowManager::setCurrentTabToolBarTab(
	const QString&				_tabName
) {
	QTabBar * tb = m_tabToolBar->tabWidget()->tabBar();
	int ct = tb->count();

	for (int index = 0; index < ct; index++) 
	{
		if (tb->tabText(index) == _tabName) 
		{
			m_tabToolBar->SetCurrentTab(index);
			break;
		}
	}
}

void ak::aWindowManager::setCentralWidgetMinimumSize(
	const QSize &				_size
) {
	QWidget * w = m_window->centralWidget();
	if (w != nullptr) {
		w->setMinimumSize(_size);
	}
}

void ak::aWindowManager::setTabToolBarTabOrder(const QStringList& _list) {
	m_tabToolBarTabOrder = _list; 
	resortTabToolBarTabOrder();
}

// #############################################################################################################

ak::aWindow * ak::aWindowManager::window(void) const { return m_window; }

void ak::aWindowManager::setWindowTitle(
	const QString &														_title
) { m_window->setWindowTitle(_title); }

void ak::aWindowManager::setWindowIcon(
	const QIcon &														_icon
) { m_window->setWindowIcon(_icon); }

QString ak::aWindowManager::windowTitle(void) const { return m_window->windowTitle(); }

void ak::aWindowManager::addEventHandler(
	aWindowEventHandler *					_eventHandler
) { m_window->addEventHandler(_eventHandler); }

void ak::aWindowManager::removeEventHandler(
	aWindowEventHandler *					_eventHandler
) { m_window->removeEventHandler(_eventHandler); }

// #############################################################################################################

// Slots

void ak::aWindowManager::slotRestoreSetting(
	const QByteArray &					_actualState
) {
	m_window->restoreState(_actualState);
}

void ak::aWindowManager::slotTabToolbarTabClicked(int _index) {
	m_messenger->sendMessage(m_uid, etTabToolbarClicked, _index);
}
void ak::aWindowManager::slotTabToolbarTabCurrentTabChanged(int _index) {
	m_messenger->sendMessage(m_uid, etTabToolbarChanged, _index);
	emit tabToolBarTabChanged(_index);
}

// #############################################################################################################

// Private functions

void ak::aWindowManager::resortTabToolBarTabOrder(void) {
	if (m_tabToolBarTabOrder.isEmpty()) { return; }

	QTabBar * tb = m_tabToolBar->tabWidget()->tabBar();
	int ct = tb->count();
	int currentTab{ 0 };

	for (int target = 0; target < m_tabToolBarTabOrder.count(); target++) {
		QString targetText = m_tabToolBarTabOrder[target];

		bool found{ false };
		for (int checkTab = currentTab; checkTab < ct; checkTab++) {
			if (tb->tabText(checkTab) == targetText) {
				if (checkTab != currentTab) {
					tb->moveTab(checkTab, currentTab);
				}
				found = true;
				break;
			}
		}
		currentTab++;
	}
}