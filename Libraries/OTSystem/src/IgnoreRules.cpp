// @otlicense
// File: IgnoreRules.cpp
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
#include "OTSystem/Exception.h"
#include "OTSystem/IgnoreRules.h"

// std header
#include <fstream>

ot::IgnoreRules ot::IgnoreRules::parseFromFile(const std::filesystem::path& _filePath) {
    std::ifstream file(_filePath, std::ios::binary | std::ios::ate);
    if (!file) {
        throw Exception::FileRead("Failed to open file for reading. File: \"" + _filePath.generic_string() + "\"");
    }

    std::string content;
    content.resize(file.tellg());

    file.seekg(0);
    file.read(content.data(), content.size());

	return parseFromText(content);
}

ot::IgnoreRules ot::IgnoreRules::parseFromText(const std::string_view& _fileContent) {
    IgnoreRules rules;

    for (std::string_view line : splitLines(_fileContent)) {
		parseRuleFromLine(rules, line);
    }

    return rules;
}

void ot::IgnoreRules::parseRuleFromLine(IgnoreRules& _rules, std::string_view _line) {
    tidy(_line);

    if (_line.empty() || _line[0] == '#') {
        return;
    }

    Rule rule;
    rule.negated = (_line[0] == '!');
    if (rule.negated) {
        _line.remove_prefix(1);
    }

    rule.anchored = (_line.find('/') == 0);
    if (rule.anchored) {
        _line.remove_prefix(1);
    }

    rule.directoryOnly = (_line.rfind('/') == (_line.length() - 1));
    if (rule.directoryOnly) {
        _line.remove_suffix(1);
    }

    rule.segments = splitPath(_line);

    rule.kind = classify(rule.segments);

	_rules.addRule(std::move(rule));
}

bool ot::IgnoreRules::matches(const std::filesystem::path& _path, bool _isDir) const {
    bool ignored = false;

    std::vector<std::string_view> segments = splitPathCached(_path);

    for (const Rule& rule : m_rules) {
        if (rule.directoryOnly && !_isDir) {
            continue;
        }
        if (!matchRule(rule, segments)) {
            continue;
        }

        ignored = !rule.negated;
    }

    return ignored;
}

bool ot::IgnoreRules::matchRule(const Rule& _rule, const std::vector<std::string_view>& _pathSegments) const {
    if (_rule.anchored) {
        return matchFrom(0, _rule, _pathSegments);
    }

    // unanchored -> try any depth
    for (size_t i = 0; i < _pathSegments.size(); ++i) {
        if (matchFrom(i, _rule, _pathSegments)) {
            return true;
        }
    }

    return false;
}

bool ot::IgnoreRules::matchFrom(size_t _pathIndex, const Rule& _rule, const std::vector<std::string_view>& _pathSegments) const {
    size_t p = _pathIndex;       // index in pathSegments
    size_t r = 0;                // index in rule.segments

    const size_t pCount = _pathSegments.size();
    const size_t rCount = _rule.segments.size();

    // Fast literal-only path
    if (_rule.kind == Rule::Kind::Literal) {
        if (p + rCount > pCount)
            return false;

        for (size_t i = 0; i < rCount; ++i) {
            if (_rule.segments[i] != _pathSegments[p + i])
                return false;
        }
        return true;
    }

    while (r < rCount && p <= pCount) {
        const std::string& seg = _rule.segments[r];

        // Recursive glob **
        if (seg == "**") {
            // ** at end always matches
            if (r + 1 == rCount)
                return true;

            // Try to match remaining rule segments at all depths
            r++;
            while (p < pCount) {
                if (matchFrom(p, _rule.createNext(r), _pathSegments)) {
                    return true;
                }
                p++;
            }
            return false;
        }

        // Path exhausted but rule remains
        if (p >= pCount)
            return false;

        // Segment match
        if (!matchGlobSegment(seg, _pathSegments[p])) {
            return false;
        }

        ++r;
        ++p;
    }

    return r == rCount && p <= pCount;
}

bool ot::IgnoreRules::matchGlobSegment(const std::string_view& _pattern, const std::string_view& _text) const {
    size_t p = 0, t = 0;
    size_t star = std::string_view::npos;
    size_t match = 0;

    while (t < _text.size()) {
        if (p < _pattern.size() && (_pattern[p] == '?' || _pattern[p] == _text[t])) {
            p++;
            t++;
        }
        else if (p < _pattern.size() && _pattern[p] == '*') {
            star = p++;
            match = t;
        }
        else if (star != std::string_view::npos) {
            p = star + 1;
            t = ++match;
        }
        else {
            return false;
        }
    }

    while (p < _pattern.size() && _pattern[p] == '*') {
        p++;
    }

    return p == _pattern.size();
}

void ot::IgnoreRules::addRule(const std::string& _rule) {
	parseRuleFromLine(*this, _rule);
}
