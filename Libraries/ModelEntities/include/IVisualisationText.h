#pragma once
#include <string>
#include "OTGui/TextEditorCfg.h"
#include "ContentChangedHandling.h"

class __declspec(dllexport) IVisualisationText
{
public:
	virtual ~IVisualisationText() {}
	virtual std::string getText() = 0;
	virtual void setText(const std::string& _text) = 0;
	virtual bool visualiseText() = 0;
	virtual ot::TextEditorCfg createConfig(bool _includeData) = 0;
	virtual ot::ContentChangedHandling getTextContentChangedHandling() = 0;
};
