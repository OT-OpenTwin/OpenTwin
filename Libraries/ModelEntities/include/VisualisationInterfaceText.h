#pragma once
#include <string>
#include "OTGui/TextEditorCfg.h"

class __declspec(dllexport) VisualisationInterfaceText
{
public:
	virtual std::string getText() = 0;
	virtual void setText(const std::string& _text) = 0;
	virtual bool visualiseText() = 0;
	virtual ot::TextEditorCfg createConfig() = 0;
};
