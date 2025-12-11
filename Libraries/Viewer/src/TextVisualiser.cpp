// @otlicense
// File: TextVisualiser.cpp
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

#include <stdafx.h>
#include "TextVisualiser.h"
#include "OTCore/JSON.h"
#include "OTCore/String.h"
#include "OTCore/LogDispatcher.h"
#include "OTCore/ReturnMessage.h"
#include "OTCommunication/ActionTypes.h"
#include "OTWidgets/TextEditor.h"
#include "OTWidgets/TextEditorView.h"
#include "OTWidgets/GlobalWidgetViewManager.h"
#include "FrontendAPI.h"
#include "DocumentAPI.h"
#include "SceneNodeBase.h"
#include "GridFSFileInfo.h"

TextVisualiser::TextVisualiser(SceneNodeBase* _sceneNode)
	: Visualiser(_sceneNode, ot::WidgetViewBase::ViewText)
{
}

bool TextVisualiser::requestVisualization(const VisualiserState& _state)
{
	if (_state.m_selectionData.getSelectionOrigin() == ot::SelectionOrigin::User)
	{
		if(_state.m_singleSelection)
		{
			if(_state.m_selected)
			{
				FrontendAPI::instance()->messageModelService(createRequestDoc(_state, 0, true).toJson());
				return true;
			}
		}
		else
		{
			OT_LOG_I("Visualisation of a multiselection of texts is turned off for performance reasons.");
		}
	}
	return false;
}

bool TextVisualiser::requestNextDataChunk(size_t _nextChunkStartIndex) {
	OTAssertNullptr(this->getSceneNode());

	VisualiserState state;
	state.m_setFocus = false;
	FrontendAPI::instance()->messageModelService(createRequestDoc(state, _nextChunkStartIndex, true).toJson());
	return true;
}

bool TextVisualiser::requestRemainingData(size_t _nextChunkStartIndex) {
	OTAssertNullptr(this->getSceneNode());

	VisualiserState state;
	state.m_setFocus = false;
	FrontendAPI::instance()->messageModelService(createRequestDoc(state, _nextChunkStartIndex, false).toJson());
	return true;
}

void TextVisualiser::showVisualisation(const VisualiserState& _state) {

}

void TextVisualiser::hideVisualisation(const VisualiserState& _state) {

}

void TextVisualiser::slotRequestRemainingDataCompleted(uint8_t* _text) {
	QString text(reinterpret_cast<const char*>(_text));
	delete[]_text;

	ot::TextEditorView* view = dynamic_cast<ot::TextEditorView*>(ot::GlobalWidgetViewManager::instance().findView(getSceneNode()->getName(), ot::WidgetViewBase::ViewText));
	if (!view) {
		OT_LOG_E("Could not find text view for entity: " + getSceneNode()->getName());
	}
	else {
		view->getTextEditor()->setPlainText(text);
	}

	FrontendAPI::instance()->lockSelectionAndModification(false);
	FrontendAPI::instance()->setProgressBarVisibility("Export text", false, true);
}

void TextVisualiser::workerRequestRemainingData(size_t _nextChunkStartIndex) {
	OTAssertNullptr(this->getSceneNode());

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_UI_RequestTextData, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityName, ot::JsonString(getSceneNode()->getName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_Index, _nextChunkStartIndex, doc.GetAllocator());

	ot::ReturnMessage retMsg = ot::ReturnMessage::fromJson(FrontendAPI::instance()->messageModelService(doc.toJson()));
	if (!retMsg.isOk()) {
		OT_LOG_E("Could not get text data from model: " + retMsg.getWhat());
	}
	else if (!retMsg.getWhat().empty()) {
		ot::JsonDocument doc;

		// Parse the returned GridFS info
		if (!doc.fromJson(retMsg.getWhat())) {
			OT_LOG_E("Could not parse grid fs info");
		}
		else {
			ot::GridFSFileInfo fileInfo(doc.getConstObject());

			// Now get the actual data from GridFS
			DataStorageAPI::DocumentAPI api;
			uint8_t* dataBuffer = nullptr;
			size_t length = 0;

			bsoncxx::oid oid_obj{ fileInfo.getDocumentId() };
			bsoncxx::types::value id{ bsoncxx::types::b_oid{oid_obj} };

			api.GetDocumentUsingGridFs(id, dataBuffer, length, fileInfo.getFileName());
			api.DeleteGridFSData(id, fileInfo.getFileName());

			// Decompress if needed
			if (fileInfo.isFileCompressed()) {
				size_t decompressedSize = fileInfo.getUncompressedSize();
				uint8_t* decompr = ot::String::decompressBase64(reinterpret_cast<const char*>(dataBuffer), decompressedSize);
				delete[] dataBuffer;
				dataBuffer = decompr;
			}

			QMetaObject::invokeMethod(this, &TextVisualiser::slotRequestRemainingDataCompleted, Qt::QueuedConnection, dataBuffer);
			return;
		}
	}

	FrontendAPI::instance()->lockSelectionAndModification(false);
	FrontendAPI::instance()->setProgressBarVisibility("Export text", false, true);
}

ot::JsonDocument TextVisualiser::createRequestDoc(const VisualiserState& _state, size_t _nextChunkStartIndex, bool _nextChunkOnly) const {
	ot::VisualisationCfg visualisationCfg = createVisualiserConfig(_state);
	visualisationCfg.setNextChunkStartIndex(_nextChunkStartIndex);
	visualisationCfg.setLoadNextChunkOnly(_nextChunkOnly);
	visualisationCfg.setVisualisationType(OT_ACTION_CMD_UI_TEXTEDITOR_Setup);

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_MODEL_RequestVisualisationData, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getSceneNode()->getModelEntityID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_VisualisationConfig, ot::JsonObject(visualisationCfg, doc.GetAllocator()), doc.GetAllocator());

	return doc;
}
