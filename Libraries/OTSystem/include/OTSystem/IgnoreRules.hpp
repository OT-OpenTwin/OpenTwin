// @otlicense
// File: IgnoreRules.hpp
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
#include "OTSystem/IgnoreRules.h"

inline ot::IgnoreRules::Rule ot::IgnoreRules::Rule::createNext(size_t _segmentIndex) const {
	Rule r;
	r.kind = kind;
	r.negated = negated;
	r.directoryOnly = directoryOnly;
	r.anchored = anchored;
	r.segments = std::vector<std::string>(segments.begin() + _segmentIndex, segments.end());
	return r;
}

inline std::list<std::string_view> ot::IgnoreRules::splitLines(std::string_view _text) {
    std::list<std::string_view> lines;

    size_t pos = 0;
    const size_t len = _text.size();

    while (pos < len) {
        size_t end = _text.find('\n', pos);
        if (end == std::string_view::npos) {
            end = len;
        }

        size_t lineLen = end - pos;
        if (lineLen && _text[pos + lineLen - 1] == '\r') {
            lineLen--;
        }

        lines.push_back(std::string_view(_text.data() + pos, lineLen));
        pos = end + 1;
    }

    return lines;
}

inline void ot::IgnoreRules::tidy(std::string_view& _text) {
    size_t start = 0;
    size_t end = _text.size();

    while (start < end) {
        char c = _text[start];
        if (c != ' ' && c != '\t') {
            break;
        }

        start++;
    }

    while (end > start) {
        char c = _text[end - 1];
        if (c != ' ' && c != '\t') {
            break;
        }

        end--;
    }

    _text = _text.substr(start, end - start);
}

inline std::vector<std::string> ot::IgnoreRules::splitPath(const std::string_view& _path) {
    std::vector<std::string> segments;

    size_t pos = 0;
    while (pos < _path.size()) {
        size_t end = _path.find('/', pos);
        if (end == std::string_view::npos) {
            end = _path.size();
        }

        if (end > pos) {
            segments.emplace_back(_path.substr(pos, end - pos));
        }

        pos = end + 1;
    }

    return segments;
}

inline std::vector<std::string_view> ot::IgnoreRules::splitPathCached(const std::filesystem::path& _path) {
    // Thread-local reuse (safe, fast)
    thread_local std::string pathBuffer;
    thread_local std::vector<std::string_view> segments;

    pathBuffer.clear();
    segments.clear();

    // Use generic format to guarantee '/'
    pathBuffer = _path.generic_string();

    const char* data = pathBuffer.data();
    const size_t len = pathBuffer.size();

    size_t start = 0;

    // Skip leading '/'
    if (len && data[0] == '/') {
        start = 1;
    }

    for (size_t i = start; i <= len; ++i) {
        if (i == len || data[i] == '/') {
            if (i > start) {
                segments.emplace_back(data + start, i - start);
            }
            start = i + 1;
        }
    }

    return segments;
}

inline ot::IgnoreRules::Rule::Kind ot::IgnoreRules::classify(const std::vector<std::string>& segments) {
    bool hasWildcard = false;

    for (const auto& seg : segments) {
        if (seg == "**") {
            return Rule::Kind::RecursiveGlob;
        }

        for (char c : seg) {
            if (c == '*' || c == '?') {
                hasWildcard = true;
                break;
            }
        }
    }

    if (hasWildcard) {
        return Rule::Kind::Glob;
    }
    else {
        return Rule::Kind::Literal;
    }
}
