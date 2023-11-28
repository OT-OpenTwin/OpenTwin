#include "PluginCore.h"

#include <openTwin/AbstractUIWidgetInterface.h>

#include <akGui/aColorStyle.h>
#include <akWidgets/aTextEditWidget.h>

#include <qtextedit.h>

#include <iostream>

static PluginCore * g_instance{ nullptr };

// ##############################################################################################

PluginCore * PluginCore::instance(void) {
	if (g_instance == nullptr) g_instance = new PluginCore;
	return g_instance;
}

void PluginCore::deleteInstance(void) {
	if (g_instance) delete g_instance;
	g_instance = nullptr;
}

// ##############################################################################################

// Required functions

bool PluginCore::initialize(void) {
	// Display info message

	m_uiInterface->appenInfoMessage("[PluginCore] Initializing UI plugin");

	// Create a text edit widget
	m_textEdit = new ak::aTextEditWidget;

	// Add the widget to the tab view in the UI frontend
	m_uiInterface->addNewTab("Test", m_textEdit);

	// Return true: Initialization successful
	return true;
}

bool PluginCore::dispatchAction(const std::string& _action, const std::string& _message) {
	// Display received message in the text edit
	m_textEdit->append("Action received: " + QString::fromStdString(_action));

	// Return true: Initialization successful
	return true;
}

void PluginCore::setColorstye(ak::aColorStyle * _colorStyle) {
	m_textEdit->setColorStyle(_colorStyle);
}

// ##############################################################################################

// Private functions

PluginCore::PluginCore() 
	: m_textEdit(nullptr)
{
}

PluginCore::~PluginCore() {

}
