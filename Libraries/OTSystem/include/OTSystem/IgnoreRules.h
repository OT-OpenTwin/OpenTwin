// @otlicense
// File: IgnoreRules.h
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
#include "OTSystem/SystemTypes.h"

// std header
#include <list>
#include <vector>
#include <filesystem>
#include <string_view>

namespace ot {

    class OT_SYS_API_EXPORT IgnoreRules {
    public:
        struct Rule {
            enum Kind : uint8_t {
                Literal,
                Glob,
                RecursiveGlob  // **
            };

            Rule() = default;
            Rule(const Rule&) = default;
            Rule(Rule&&) noexcept = default;
            ~Rule() = default;

            Rule& operator=(const Rule&) = default;
            Rule& operator=(Rule&&) noexcept = default;

            inline Rule createNext(size_t _segmentIndex) const;

            Kind kind = Literal;
            bool negated = false;
            bool directoryOnly = false;
            bool anchored = false;
            std::vector<std::string> segments;
        };

        static IgnoreRules parseFromFile(const std::filesystem::path& _filePath);
        static IgnoreRules parseFromText(const std::string_view& _fileContent);
		static void parseRuleFromLine(IgnoreRules& _rules, std::string_view _line);

        IgnoreRules() = default;
        IgnoreRules(const IgnoreRules&) = default;
        IgnoreRules(IgnoreRules&&) noexcept = default;
        ~IgnoreRules() = default;

        IgnoreRules& operator=(const IgnoreRules&) = default;
        IgnoreRules& operator=(IgnoreRules&&) noexcept = default;

		bool isEmpty() const { return m_rules.empty(); };

        bool matches(const std::filesystem::path& _path, bool _isDir) const;
        bool matchRule(const Rule& _rule, const std::vector<std::string_view>& _pathSegments) const;
        bool matchFrom(size_t _pathIndex, const Rule& _rule, const std::vector<std::string_view>& _pathSegments) const;
        bool matchGlobSegment(const std::string_view& _pattern, const std::string_view& _text) const;

        void addRule(const std::string& _rule);
        void addRule(Rule&& _rule) { m_rules.push_back(std::move(_rule)); };
        const std::vector<Rule>& getRules() const { return m_rules; };

    private:
        static inline std::list<std::string_view> splitLines(std::string_view _text);
        static inline void tidy(std::string_view& _text);
        static inline std::vector<std::string> splitPath(const std::string_view& _path);
        static inline std::vector<std::string_view> splitPathCached(const std::filesystem::path& _path);
        static inline Rule::Kind classify(const std::vector<std::string>& segments);

        std::vector<Rule> m_rules;
    };
}

#include "OTSystem/IgnoreRules.hpp"
