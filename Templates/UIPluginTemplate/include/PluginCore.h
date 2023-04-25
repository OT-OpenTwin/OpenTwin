#pragma once

#include <openTwin/AbstractPluginCore.h>
#include <qstring.h>

namespace ot { class AbstractUIWidgetInterface; }
namespace ak { class aTextEditWidget; }

class PluginCore : public ot::AbstractPluginCore {
public:
	static PluginCore * instance(void);
	static void deleteInstance(void);

	// ##############################################################################################

	// Required functions

	//! @brief Will be called after the plugin was loaded
	//! In this function the plugin should create its required UI controls and initialize the data
	virtual bool initialize(void) override;

	//! @brief Will be called if a message for this plugin was received
	//! @param _action The action to be performed
	//! @param _message Message containing information
	virtual bool dispatchAction(const std::string& _action, const std::string& _message) override;

	//! @brief Will be called when the appearance of the UI frontend has changed
	//! In this function all UI controls should apply the colorstyle to match the UI frontend
	virtual void setColorstye(ak::aColorStyle * _colorStyle) override;

	// ##############################################################################################
	
	

	// Private functions
private:
	

	PluginCore();
	virtual ~PluginCore();

	ak::aTextEditWidget *		m_textEdit;

	PluginCore(PluginCore&) = delete;
	PluginCore& operator = (PluginCore&) = delete;
};