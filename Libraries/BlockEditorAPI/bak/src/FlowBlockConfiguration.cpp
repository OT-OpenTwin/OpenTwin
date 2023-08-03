// OpenTwin header
#include "OpenTwinCore/rJSONHelper.h"
#include "OTBlockEditorAPI/FlowBlockConfiguration.h"
#include "OTBlockEditorAPI/BlockConnectorConfiguration.h"
#include "OTGui/Painter2D.h"
#include "OpenTwinCore/otAssert.h"

#define JSON_MEMBER_BackgroundIcon "BackgroundIcon"
#define JSON_MEMBER_BackgroundPainter "BackgroundPainter"
#define JSON_MEMBER_TitleIcon "TitleIcon"
#define JSON_MEMBER_TitleColorLeft "TitleColorLeft"
#define JSON_MEMBER_TitleColorRight "TitleColorRight"
#define JSON_MEMBER_TitleTextColor "TitleTextColor"
#define JSON_MEMBER_Inputs "Inputs"
#define JSON_MEMBER_Outputs "Outputs"
#define JSON_MEMBER_IconBottomLeft "IconBottomLeft"
#define JSON_MEMBER_IconBottomRight "IconBottomRight"

ot::FlowBlockConfiguration::FlowBlockConfiguration(const std::string& _name)
	: BlockConfiguration(_name), m_backgroundPainter(nullptr)
{

}

ot::FlowBlockConfiguration::~FlowBlockConfiguration() {
	if (m_backgroundPainter) delete m_backgroundPainter;
}

void ot::FlowBlockConfiguration::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	otAssert(!m_inputs.empty() || !m_outputs.empty(), "Exporting invalid flow block configuration:\n\tNo inputs or outputs provided");

	BlockConfiguration::addToJsonObject(_document, _object);

	ot::rJSON::add(_document, _object, JSON_MEMBER_BackgroundIcon, m_backgroundIconSubPath);

	OT_rJSON_createValueObject(backgroundPainterObj);
	m_backgroundPainter->addToJsonObject(_document, backgroundPainterObj);
	ot::rJSON::add(_document, _object, JSON_MEMBER_BackgroundPainter, backgroundPainterObj);

	ot::rJSON::add(_document, _object, JSON_MEMBER_TitleIcon, m_titleIconSubPath);
	
	OT_rJSON_createValueObject(titleColorLeftObj);
	m_titleColorLeft.addToJsonObject(_document, titleColorLeftObj);
	ot::rJSON::add(_document, _object, JSON_MEMBER_TitleColorLeft, titleColorLeftObj);

	OT_rJSON_createValueObject(titleColorRightObj);
	m_titleColorRight.addToJsonObject(_document, titleColorRightObj);
	ot::rJSON::add(_document, _object, JSON_MEMBER_TitleColorRight, titleColorRightObj);

	OT_rJSON_createValueObject(titleTextColorObj);
	m_titleTextColor.addToJsonObject(_document, titleTextColorObj);
	ot::rJSON::add(_document, _object, JSON_MEMBER_TitleTextColor, titleTextColorObj);
	
	OT_rJSON_createValueArray(inputsArr);
	for (auto i : m_inputs) {
		OT_rJSON_createValueObject(inputObj);
		i->addToJsonObject(_document, inputObj);
		inputsArr.PushBack(inputObj, _document.GetAllocator());
	}
	ot::rJSON::add(_document, _object, JSON_MEMBER_Inputs, inputsArr);

	OT_rJSON_createValueArray(outputsArr);
	for (auto o : m_outputs) {
		OT_rJSON_createValueObject(outputObj);
		o->addToJsonObject(_document, outputObj);
		outputsArr.PushBack(outputObj, _document.GetAllocator());
	}
	ot::rJSON::add(_document, _object, JSON_MEMBER_Outputs, outputsArr);

	ot::rJSON::add(_document, _object, JSON_MEMBER_IconBottomLeft, m_bottomLeftIconSubPath);
	ot::rJSON::add(_document, _object, JSON_MEMBER_IconBottomRight, m_bottomRightIconSubPath);
}

void ot::FlowBlockConfiguration::setFromJsonObject(OT_rJSON_val& _object) {
	BlockConfiguration::setFromJsonObject(_object);

	OT_rJSON_checkMember(_object, JSON_MEMBER_BackgroundIcon, String);
	OT_rJSON_checkMember(_object, JSON_MEMBER_BackgroundPainter, Object);
	OT_rJSON_checkMember(_object, JSON_MEMBER_TitleIcon, String);
	OT_rJSON_checkMember(_object, JSON_MEMBER_TitleColorLeft, Object);
	OT_rJSON_checkMember(_object, JSON_MEMBER_TitleColorRight, Object);
	OT_rJSON_checkMember(_object, JSON_MEMBER_TitleTextColor, Object);
	OT_rJSON_checkMember(_object, JSON_MEMBER_Inputs, Array);
	OT_rJSON_checkMember(_object, JSON_MEMBER_Outputs, Array);
	OT_rJSON_checkMember(_object, JSON_MEMBER_IconBottomLeft, String);
	OT_rJSON_checkMember(_object, JSON_MEMBER_IconBottomRight, String);

	m_backgroundIconSubPath = _object[JSON_MEMBER_BackgroundIcon].GetString();
	m_titleIconSubPath = _object[JSON_MEMBER_TitleIcon].GetString();
	m_bottomLeftIconSubPath = _object[JSON_MEMBER_IconBottomLeft].GetString();
	m_bottomRightIconSubPath = _object[JSON_MEMBER_IconBottomRight].GetString();

	OT_rJSON_val backgroundPainterObj = _object[JSON_MEMBER_BackgroundPainter].GetObject();
	m_backgroundPainter->setFromJsonObject(backgroundPainterObj);

	OT_rJSON_val titleColorLeftObj = _object[JSON_MEMBER_TitleColorLeft].GetObject();
	m_titleColorLeft.setFromJsonObject(titleColorLeftObj);

	OT_rJSON_val titleColorRightObj = _object[JSON_MEMBER_TitleColorRight].GetObject();
	m_titleColorRight.setFromJsonObject(titleColorRightObj);

	OT_rJSON_val titleTextColorObj = _object[JSON_MEMBER_TitleTextColor].GetObject();
	m_titleTextColor.setFromJsonObject(titleTextColorObj);

	OT_rJSON_val inputsArr = _object[JSON_MEMBER_Inputs].GetArray();
	for (rapidjson::SizeType i = 0; i < inputsArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(inputsArr, i, Object);
		OT_rJSON_val inputObj = inputsArr[i].GetObject();
		BlockConnectorConfiguration* newConnector = new BlockConnectorConfiguration;
		try {
			newConnector->setFromJsonObject(inputObj);
			addInput(newConnector);
		}
		catch (const std::exception& _e) {
			delete newConnector;
			throw _e;
		}
		catch (...) {
			delete newConnector;
			throw std::exception("Fatal: Unknown error");
		}
	}

	OT_rJSON_val outputsArr = _object[JSON_MEMBER_Outputs].GetArray();
	for (rapidjson::SizeType i = 0; i < outputsArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(outputsArr, i, Object);
		OT_rJSON_val outputObj = outputsArr[i].GetObject();
		BlockConnectorConfiguration* newConnector = new BlockConnectorConfiguration;
		try {
			newConnector->setFromJsonObject(outputObj);
			addOutput(newConnector);
		}
		catch (const std::exception& _e) {
			delete newConnector;
			throw _e;
		}
		catch (...) {
			delete newConnector;
			throw std::exception("Fatal: Unknown error");
		}
	}
}

void ot::FlowBlockConfiguration::setBackgroundPainter(Painter2D*& _painter) {
	if (m_backgroundPainter == _painter) return;
	if (m_backgroundPainter) delete m_backgroundPainter;
	m_backgroundPainter = _painter;
}

void ot::FlowBlockConfiguration::addInput(BlockConnectorConfiguration* _block) {
	m_inputs.push_back(_block);
}

void ot::FlowBlockConfiguration::addOutput(BlockConnectorConfiguration* _block) {
	m_outputs.push_back(_block);
}
