// @otlicense
// File: TextEditorCfg.cpp
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

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTCore/RuntimeTests.h"
#include "OTGui/TextEditorCfg.h"

#if OT_TESTING_GLOBAL_AllTestsEnabled==true
#define OT_TESTING_LOCAL_TEXTEDITORCFG_PERFORMANCETEST_ENABLED OT_TESTING_GLOBAL_AllTestsEnabled
#elif OT_TESTING_GLOBAL_RuntimeTestingEnabled==true
#define OT_TESTING_LOCAL_TEXTEDITORCFG_PERFORMANCETEST_ENABLED OT_TESTING_GLOBAL_RuntimeTestingEnabled
#else
#define OT_TESTING_LOCAL_TEXTEDITORCFG_PERFORMANCETEST_ENABLED false
#endif 

#if OT_TESTING_LOCAL_TEXTEDITORCFG_PERFORMANCETEST_ENABLED==true
#define OT_TEST_TEXTEDITORCFG_Interval(___testText) OT_TEST_Interval(ot_intern_texteditorcfg_lcl_performancetest, "TextEditorCfg", ___testText)
#else
#define OT_TEST_TEXTEDITORCFG_Interval(___testText)
#endif

ot::TextEditorCfg::TextEditorCfg() :
	WidgetViewBase(WidgetViewBase::ViewText, WidgetViewBase::ViewIsCentral | WidgetViewBase::ViewIsCloseable | WidgetViewBase::ViewIsPinnable | WidgetViewBase::ViewNameAsTitle | WidgetViewBase::ViewCloseOnEmptySelection),
	m_syntax(DocumentSyntax::PlainText), m_readOnly(false), m_fileExtensionFilter(FileExtension::toFilterString(FileExtension::AllFiles)),
	m_hasMore(false), m_nextChunkStartIx(0), m_isChunk(false), m_remainingSize(0)
{}

ot::TextEditorCfg::TextEditorCfg(const ConstJsonObject & _jsonObject) : TextEditorCfg() {
	setFromJsonObject(_jsonObject);
}

ot::TextEditorCfg::~TextEditorCfg() {}

void ot::TextEditorCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	OT_TEST_TEXTEDITORCFG_Interval("Export");

	WidgetViewBase::addToJsonObject(_object, _allocator);

	_object.AddMember("Text", JsonString(m_text, _allocator), _allocator);
	_object.AddMember("IsChunk", m_isChunk, _allocator);
	_object.AddMember("HasMore", m_hasMore, _allocator);
	_object.AddMember("NextChunkStart", m_nextChunkStartIx, _allocator);
	_object.AddMember("RemainingSize", m_remainingSize, _allocator);
	_object.AddMember("ReadOnly", m_readOnly, _allocator);
	_object.AddMember("Syntax", JsonString(ot::toString(m_syntax), _allocator), _allocator);
	_object.AddMember("FileExtensionsFilter", JsonString(m_fileExtensionFilter, _allocator), _allocator);
}

void ot::TextEditorCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	OT_TEST_TEXTEDITORCFG_Interval("Import");

	WidgetViewBase::setFromJsonObject(_object);
	
	m_text = json::getString(_object, "Text");
	m_isChunk = json::getBool(_object, "IsChunk");
	m_hasMore = json::getBool(_object, "HasMore");
	m_nextChunkStartIx = json::getUInt64(_object, "NextChunkStart");
	m_remainingSize = json::getUInt64(_object, "RemainingSize");
	m_readOnly = json::getBool(_object, "ReadOnly");
	m_syntax = stringToDocumentSyntax(json::getString(_object, "Syntax"));
	m_fileExtensionFilter = json::getString(_object, "FileExtensionsFilter");
}

void ot::TextEditorCfg::setNextChunk(const std::string& _fullText, size_t _startIndex, size_t _chunkSize) {
	if (_fullText.empty()) {
		return;
	}

	size_t endIx = std::min(_fullText.size() - 1, (_startIndex + _chunkSize - 1));
	if (endIx < _startIndex) {
		OT_LOG_W("Start index is beyond the full text size.");
		return;
	}

	if (endIx < _fullText.size() - 1) {

		// Try to end at end of word
		for (; endIx > _startIndex; endIx--) {
			const char c = _fullText[endIx];
			if (isspace(_fullText[endIx])) {
				if (endIx > 0) {
					endIx--;
				}
				break;
			}
		}

		// If no end of word found in chunk, hard break
		if (endIx <= _startIndex) {
			endIx = std::min(_fullText.size() - 1, (_startIndex + _chunkSize - 1));
		}
	}

	m_text = _fullText.substr(_startIndex, (endIx - _startIndex) + 1);
	m_isChunk = true;
	m_hasMore = (endIx < _fullText.size() - 1);
	m_nextChunkStartIx = endIx + 1;
	m_remainingSize = (m_hasMore ? (_fullText.size() - m_nextChunkStartIx) : 0);
}

void ot::TextEditorCfg::setFileExtensionFilter(const std::initializer_list<FileExtension::DefaultFileExtension>& _extensions) {
	m_fileExtensionFilter = FileExtension::toFilterString(std::list<FileExtension::DefaultFileExtension>(_extensions));
}

void ot::TextEditorCfg::setFileExtensionFilter(const std::list<FileExtension::DefaultFileExtension>& _extensions) {
	m_fileExtensionFilter = FileExtension::toFilterString(_extensions);
}
