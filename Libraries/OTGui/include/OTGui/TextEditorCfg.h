// @otlicense
// File: TextEditorCfg.h
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

#pragma once

// OpenTwin header
#include "OTGui/GuiTypes.h"
#include "OTGui/FileExtension.h"
#include "OTGui/WidgetViewBase.h"

// std header
#include <list>
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_GUI_API_EXPORT TextEditorCfg : public WidgetViewBase {
		OT_DECL_DEFCOPY(TextEditorCfg)
		OT_DECL_DEFMOVE(TextEditorCfg)
	public:
		TextEditorCfg();
		TextEditorCfg(const ConstJsonObject& _jsonObject);
		virtual ~TextEditorCfg();

		//! @brief Add the object contents to the provided JSON object.
		//! @param _object Json object reference to write the data to.
		//! @param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		//! @brief Sets the next chunk of text from the full text.
		//! Will set all relevant members to represent the chunk.
		//! Will attemt to not break words when extracting the chunk.
		//! @param _fullText The full text to extract the chunk from.
		//! @param _startIndex The offset in the full text where the chunk starts.
		//! @param _chunkSize The size of the chunk to extract.
		void setNextChunk(const std::string& _fullText, size_t _startIndex, size_t _chunkSize);

		void setPlainText(const std::string& _text) { m_text = _text; };
		void setPlainText(std::string&& _text) { m_text = std::move(_text); };
		const std::string& getPlainText() const { return m_text; };

		void setIsChunk(bool _isChunk) { m_isChunk = _isChunk; };
		bool getIsChunk() const { return m_isChunk; };

		void setHasMore(bool _hasMore) { m_hasMore = _hasMore; };
		bool getHasMore() const { return m_hasMore; };

		void setNextChunkStartIndex(size_t _index) { m_nextChunkStartIx = _index; };
		size_t getNextChunkStartIndex() const { return m_nextChunkStartIx; };

		//! @brief Set read only enabled.
		//! @param _readOnly If true the text in the text editor can not be edited by the user.
		void setTextReadOnly(bool _readOnly) { m_readOnly = _readOnly; };
		bool getTextReadOnly() const { return m_readOnly; };

		void setDocumentSyntax(DocumentSyntax _syntax) { m_syntax = _syntax; };
		DocumentSyntax getDocumentSyntax() const { return m_syntax; };

		void setFileExtensionFilter(const std::initializer_list<FileExtension::DefaultFileExtension>& _extensions);
		void setFileExtensionFilter(const std::list<FileExtension::DefaultFileExtension>& _extensions);
		void setFileExtensionFilter(const std::string& _filter) { m_fileExtensionFilter = _filter; };
		const std::string& getFileExtensionFilters() const { return m_fileExtensionFilter; };

	private:
		std::string m_text;
		bool m_isChunk;
		bool m_hasMore;
		size_t m_nextChunkStartIx;
		bool m_readOnly;
		DocumentSyntax m_syntax;
		std::string m_fileExtensionFilter;
	};

}
