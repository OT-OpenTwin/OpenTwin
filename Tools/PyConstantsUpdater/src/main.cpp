// @otlicense

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>
#include <vector>

namespace
{
    using MacroMap = std::map<std::string, std::string>;

    // ############################################################################
    // Logging
    // ############################################################################

    void logInfo(const std::string& message)
    {
        std::cout << "[INFO] " << message << '\n';
    }

    void logWarning(const std::string& message)
    {
        std::cerr << "[WARNING] " << message << '\n';
    }

    void logError(const std::string& message)
    {
        std::cerr << "[ERROR] " << message << '\n';
    }

    // ############################################################################
    // String utilities
    // ############################################################################

    std::string trim(const std::string& value)
    {
        const auto first = value.find_first_not_of(" \t\r\n");

        if (first == std::string::npos)
        {
            return {};
        }

        const auto last = value.find_last_not_of(" \t\r\n");
        return value.substr(first, last - first + 1);
    }

    bool isIdentifierStart(char c)
    {
        return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
    }

    bool isIdentifierChar(char c)
    {
        return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
    }

    // Remove C++ comments while preserving string literals.
    std::string removeComments(const std::string& line, bool& insideBlockComment)
    {
        std::string result;
        bool insideString = false;
        bool escaped = false;

        for (size_t i = 0; i < line.size(); ++i)
        {
            const char c = line[i];
            const char next = (i + 1 < line.size()) ? line[i + 1] : '\0';

            if (insideBlockComment)
            {
                if (c == '*' && next == '/')
                {
                    insideBlockComment = false;
                    ++i;
                }
                continue;
            }

            if (insideString)
            {
                result += c;

                if (escaped)
                {
                    escaped = false;
                }
                else if (c == '\\')
                {
                    escaped = true;
                }
                else if (c == '"')
                {
                    insideString = false;
                }

                continue;
            }

            if (c == '"')
            {
                insideString = true;
                result += c;
            }
            else if (c == '/' && next == '/')
            {
                break;
            }
            else if (c == '/' && next == '*')
            {
                insideBlockComment = true;
                ++i;
            }
            else
            {
                result += c;
            }
        }

        return result;
    }

    // ############################################################################
    // Header parsing
    // ############################################################################

    bool readMacros(const std::filesystem::path& source, MacroMap& macros)
    {
        std::ifstream file(source);

        if (!file.is_open())
        {
            logError("Could not open source file: " + source.string());
            return false;
        }

        std::string line;
        std::string logicalLine;
        bool insideBlockComment = false;
        size_t lineNumber = 0;

        while (std::getline(file, line))
        {
            ++lineNumber;

            // Handle preprocessor line continuations.
            if (!line.empty() && line.back() == '\\')
            {
                logicalLine += line.substr(0, line.size() - 1);
                continue;
            }

            logicalLine += line;
            logicalLine = removeComments(logicalLine, insideBlockComment);

            const std::string cleaned = trim(logicalLine);
            logicalLine.clear();

            if (cleaned.empty() || cleaned[0] != '#')
            {
                continue;
            }

            std::istringstream stream(cleaned.substr(1));

            std::string directive;
            stream >> directive;

            if (directive != "define")
            {
                continue;
            }

            std::string name;
            stream >> name;

            if (name.empty())
            {
                logWarning("Invalid define at line " + std::to_string(lineNumber));
                continue;
            }

            // Function-like macros have no whitespace between their name
            // and opening parenthesis.
            const size_t namePosition = cleaned.find(name);
            const size_t valuePosition = namePosition + name.size();

            if (valuePosition < cleaned.size() &&
                cleaned[valuePosition] == '(')
            {
                logWarning("Skipping function-like macro: " + name);
                continue;
            }

            const std::string value = trim(cleaned.substr(valuePosition));

            if (macros.count(name))
            {
                logError("Duplicate macro definition: " + name);
                return false;
            }

            macros[name] = value;
        }

        if (!logicalLine.empty())
        {
            logError("Incomplete preprocessor line continuation at end of file.");
            return false;
        }

        logInfo("Read " + std::to_string(macros.size()) + " macros.");
        return true;
    }

    // ############################################################################
    // Macro resolution
    // ############################################################################

    bool resolveMacro(
        const std::string& name,
        const MacroMap& macros,
        std::set<std::string>& resolving,
        std::string& result,
        std::string& error)
    {
        const auto it = macros.find(name);

        if (it == macros.end())
        {
            error = "Undefined macro reference: " + name;
            return false;
        }

        if (!resolving.insert(name).second)
        {
            error = "Circular macro reference involving: " + name;
            return false;
        }

        const std::string& expression = it->second;

        result.clear();

        for (size_t i = 0; i < expression.size();)
        {
            const char c = expression[i];

            // Preserve string literals without expanding their contents.
            if (c == '"')
            {
                result += c;
                ++i;

                bool escaped = false;

                while (i < expression.size())
                {
                    const char current = expression[i++];
                    result += current;

                    if (escaped)
                    {
                        escaped = false;
                    }
                    else if (current == '\\')
                    {
                        escaped = true;
                    }
                    else if (current == '"')
                    {
                        break;
                    }
                }

                if (result.empty() || result.back() != '"')
                {
                    error = "Unterminated string literal in macro: " + name;
                    resolving.erase(name);
                    return false;
                }

                continue;
            }

            if (isIdentifierStart(c))
            {
                size_t end = i + 1;

                while (end < expression.size() &&
                    isIdentifierChar(expression[end]))
                {
                    ++end;
                }

                const std::string identifier = expression.substr(i, end - i);

                if (identifier.rfind("OT", 0) != 0)
                {
                    result += identifier;
                    i = end;
                    continue;
                }

                std::string replacement;

                if (!resolveMacro(identifier, macros, resolving, replacement, error))
                {
                    resolving.erase(name);
                    return false;
                }

                result += replacement;
                i = end;
                continue;
            }

            result += c;
            ++i;
        }

        resolving.erase(name);
        return true;
    }

    // ############################################################################
    // C++ string literal parsing
    // ############################################################################

    bool parseStringLiterals(
        const std::string& expression,
        std::string& result)
    {
        result.clear();

        size_t i = 0;
        bool foundLiteral = false;

        while (i < expression.size())
        {
            while (i < expression.size() &&
                std::isspace(static_cast<unsigned char>(expression[i])))
            {
                ++i;
            }

            if (i == expression.size())
            {
                break;
            }

            if (expression[i] != '"')
            {
                return false;
            }

            foundLiteral = true;
            ++i;

            bool terminated = false;

            while (i < expression.size())
            {
                char c = expression[i++];

                if (c == '"')
                {
                    terminated = true;
                    break;
                }

                if (c != '\\')
                {
                    result += c;
                    continue;
                }

                if (i >= expression.size())
                {
                    return false;
                }

                const char escaped = expression[i++];

                switch (escaped)
                {
                case '\\': result += '\\'; break;
                case '"':  result += '"';  break;
                case '\'': result += '\''; break;
                case '?':  result += '?';  break;
                case 'a':  result += '\a'; break;
                case 'b':  result += '\b'; break;
                case 'f':  result += '\f'; break;
                case 'n':  result += '\n'; break;
                case 'r':  result += '\r'; break;
                case 't':  result += '\t'; break;
                case 'v':  result += '\v'; break;

                default:
                    logWarning("Unsupported escape sequence in string literal.");
                    return false;
                }
            }

            if (!terminated)
            {
                return false;
            }
        }

        return foundLiteral;
    }

    // ############################################################################
    // Python output formatting
    // ############################################################################

    std::string escapePythonString(const std::string& value)
    {
        std::string result = "\"";

        for (unsigned char c : value)
        {
            switch (c)
            {
            case '\\': result += "\\\\"; break;
            case '"':  result += "\\\""; break;
            case '\n': result += "\\n";  break;
            case '\r': result += "\\r";  break;
            case '\t': result += "\\t";  break;
            case '\0': result += "\\x00"; break;

            default:
                if (c < 0x20 || c == 0x7F)
                {
                    static const char hex[] = "0123456789abcdef";

                    result += "\\x";
                    result += hex[c >> 4];
                    result += hex[c & 0x0F];
                }
                else
                {
                    result += static_cast<char>(c);
                }
                break;
            }
        }

        result += '"';
        return result;
    }

    // ############################################################################
    // Conversion
    // ############################################################################

    bool convertMacros(
        const MacroMap& macros,
        std::map<std::string, std::string>& constants)
    {
        for (const auto& [name, expression] : macros)
        {
            std::set<std::string> resolving;
            std::string expanded;
            std::string error;

            if (!resolveMacro(name, macros, resolving, expanded, error))
            {
                logError(name + ": " + error);
                return false;
            }

            std::string value;

            if (!parseStringLiterals(expanded, value))
            {
                logWarning("Skipping unsupported macro: " + name);
                continue;
            }

            constants[name] = escapePythonString(value);
        }

        return true;
    }

    // ############################################################################
    // Python file generation
    // ############################################################################

    bool writePythonFile(
        const std::filesystem::path& source,
        const std::filesystem::path& destination,
        const std::map<std::string, std::string>& constants)
    {
        std::ofstream file(destination);

        if (!file.is_open())
        {
            logError("Could not open destination file: " + destination.string());
            return false;
        }

        file << "# ############################################################################\n";
        file << "# This file was auto generated from the following source header:\n";
        file << "#     " << source.filename().string() << "\n";
        file << "# ############################################################################\n\n";

        file << "from typing import Final\n\n";

        for (const auto& [name, value] : constants)
        {
            file << name << ": Final[str] = " << value << '\n';
        }

        if (!file.good())
        {
            logError("Failed while writing destination file.");
            return false;
        }

        logInfo("Generated " + std::to_string(constants.size()) +
            " Python constants.");

        return true;
    }
}

// ############################################################################
// Main
// ############################################################################

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cerr
            << "Usage: DefineToPython <source.h> <destination.py>\n";

        return 1;
    }

    const std::filesystem::path source = argv[1];
    const std::filesystem::path destination = argv[2];

    if (!std::filesystem::exists(source))
    {
        logError("Source file does not exist: " + source.string());
        return 1;
    }

    if (source == destination)
    {
        logError("Source and destination must be different files.");
        return 1;
    }

    MacroMap macros;
    std::map<std::string, std::string> constants;

    if (!readMacros(source, macros))
    {
        return 1;
    }

    if (!convertMacros(macros, constants))
    {
        return 1;
    }

    if (!writePythonFile(source, destination, constants))
    {
        return 1;
    }

    logInfo("Conversion completed successfully.");
    return 0;
}
