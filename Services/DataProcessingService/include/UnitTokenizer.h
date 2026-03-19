#pragma once
#include <vector>
#include <string>

#include "UnitToken.h"

class UnitTokenizer
{
public:
    // Parses a unit expression string into a flat list of UnitTokens.
    //
    // Supported grammar:
    //   expr       := term { ('*' | '.') term } [ '/' denominator ]
    //   denominator:= term | '(' term { ('*' | '.') term } ')'
    //   term       := symbol [ ('^' | implicit) exponent ]
    //   exponent   := signed_integer
    //
    // Examples:
    //   "kg"           → [{kg,  +1}]
    //   "km/h"         → [{km,  +1}, {h,  -1}]
    //   "kg*m/s2"      → [{kg,  +1}, {m,  +1}, {s,  -2}]
    //   "kg*m/(s2*K)"  → [{kg,  +1}, {m,  +1}, {s,  -2}, {K, -1}]
    //   "W/m2"         → [{W,   +1}, {m,  -2}]
    //   "m^-1"         → [{m,   -1}]
    //   "kg.m.s^-2"    → [{kg,  +1}, {m,  +1}, {s,  -2}]
    std::vector<UnitToken> tokenize(const std::string& _expr);

private: 
    size_t m_pos;
    size_t m_length;
    std::string m_expression;
    std::vector < UnitToken> m_tokens;
    // Attempts to parse a signed or unsigned integer at s[pos].
    // Advances pos past the consumed digits. Returns true on success.
    bool tryParseInt(const std::string& _string, std::size_t& _pos, int& _out);

    // Consumes an alphanumeric+underscore symbol token (e.g. "km", "degC", "BTU").
    // A symbol must start with a letter.
    bool tryParseSymbol(const std::string& _string, std::size_t& _pos, std::string& _out);
    
    // Parses one or more '*' / '.' separated terms in the same role context.
    // Stops at '/', ')' or end-of-string.
    void parseTermList(int _roleSign);

    // Parses one "term": a symbol followed by an optional exponent.
    // roleSign is +1 (numerator) or -1 (denominator) — it flips the
    // final exponent sign so callers only need to track context once.
    UnitToken parseTerm(int _roleSign);

};
