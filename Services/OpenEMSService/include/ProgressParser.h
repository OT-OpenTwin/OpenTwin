// @otlicense
// File: ProgressParser.h
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

#include <optional>
#include <regex>
#include <string>
#include <string_view>
#include <algorithm>

#undef min
#undef max

class ProgressParser
{
public:
    void append(std::string_view text)
    {
        m_parseBuffer.append(text.data(), text.size());

        static const std::regex energyPattern(
            R"(Energy:\s*~?[+-]?(?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?\s*\(\s*([+-]?)\s*((?:\d+(?:\.\d*)?|\.\d+)(?:[eE][+-]?\d+)?)\s*dB\s*\))"
        );

        std::smatch match;
        auto searchBegin = m_parseBuffer.cbegin();

        while (std::regex_search(
            searchBegin, m_parseBuffer.cend(), match, energyPattern))
        {
            m_lastEnergyDb = std::stod(match[1].str() + match[2].str());
            searchBegin = match.suffix().first;
        }

        // Keep an incomplete Energy entry for the next segment
        const std::size_t energyPos = m_parseBuffer.rfind("Energy:");

        if (energyPos != std::string::npos &&
            m_parseBuffer.find(')', energyPos) == std::string::npos)
        {
            m_parseBuffer.erase(0, energyPos);
        }
        else
        {
            // Keep enough characters to detect a split "Energy:" marker
            constexpr std::size_t keepCharacters =
                std::string_view("Energy:").size() - 1;

            if (m_parseBuffer.size() > keepCharacters)
            {
                m_parseBuffer.erase(
                    0, m_parseBuffer.size() - keepCharacters);
            }
        }
    }
    
    void clear()
    {
        m_parseBuffer.clear();
        m_lastEnergyDb.reset();
        m_currentProgress = 0;
    }

    bool getCurrentProgress(int &progressNow) 
    {
        progressNow = m_currentProgress;

        if (const auto energyDb = m_lastEnergyDb)
        {
            int progress = std::min(100, 100 - (int)(100.0 * (m_stopCondition - *energyDb) / m_stopCondition));
            if (progress > m_currentProgress)
            {
                m_currentProgress = progress;
                progressNow = m_currentProgress;
                return true;
            }
        }

        return false;
    }

    void setStopCondition(double dBlevel)
    {
        m_stopCondition = dBlevel;
    }

private:
    std::string           m_parseBuffer;
    std::optional<double> m_lastEnergyDb;
    int                   m_currentProgress = 0;
    double                m_stopCondition = -50;
};