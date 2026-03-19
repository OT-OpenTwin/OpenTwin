#include "UnitTokenizer.h"
#include "OTCore/String.h"

bool UnitTokenizer::tryParseInt(const std::string& _string, std::size_t& _pos, int& _out)
{
    if (_pos >= _string.size())
    {
        return false;
    }

    int sign = 1;
    std::size_t start = _pos;

    if (_string[_pos] == '-') 
    { 
        sign = -1; 
        ++_pos; 
    }
    else if (_string[_pos] == '+') 
    { 
        ++_pos; 
    }

    if (_pos >= _string.size() || !std::isdigit(_string[_pos])) 
    {
        _pos = start; // backtrack — no digits followed the sign
        return false;
    }

    int value = 0;
    while (_pos < _string.size() && std::isdigit(_string[_pos]))
    {
        value = value * 10 + (_string[_pos++] - '0');
    }
    _out = sign * value;
    return true;
}

bool UnitTokenizer::tryParseSymbol(const std::string& _string, std::size_t& _pos, std::string& _out)
{
    if (_pos >= _string.size() || !std::isalpha(_string[_pos]))
    {
        return false;
    }

    std::size_t start = _pos;
    while (_pos < _string.size() && (std::isalnum(_string[_pos]) || _string[_pos] == '_'))
    {
        ++_pos;
    }

    _out = _string.substr(start, _pos - start);
    return true;
}

void UnitTokenizer::parseTermList(int _roleSign)
{
    UnitToken token = parseTerm(_roleSign);
    m_tokens.push_back(token);
    while (m_pos < m_length && (m_expression[m_pos] == '*' || m_expression[m_pos] == '.'))
    {
        ++m_pos; // consume operator
        token = parseTerm(_roleSign);
        m_tokens.push_back(token);
    }
}

UnitToken UnitTokenizer::parseTerm(int _roleSign)
{
    std::string sym;
    if (!tryParseSymbol(m_expression, m_pos, sym))
    {
        throw std::exception(("Expected unit symbol at position " + std::to_string(m_pos) + " in \"" + m_expression + "\"").c_str());
    }

    int exp = 1;

    if (m_pos < m_length && m_expression[m_pos] == '^') {
        // Explicit exponent: "m^2", "s^-2"
        ++m_pos;
        if (!tryParseInt(m_expression, m_pos, exp))
        {
            throw std::exception(("Expected integer after '^' at position " + std::to_string(m_pos) + " in \"" + m_expression + "\"").c_str());
        }
    }
    else if (m_pos < m_length && (m_expression[m_pos] == '-' || std::isdigit(m_expression[m_pos]))) {
        // Implicit exponent: "s2", "s-2"  (no caret)
        if (!tryParseInt(m_expression, m_pos, exp))
        {
            throw std::exception(("Expected integer after symbol at position " + std::to_string(m_pos) + " in \"" + m_expression + "\"").c_str());

        }
    }
    // else: exponent defaults to 1
    return { sym, _roleSign * exp };
}

std::vector<UnitToken> UnitTokenizer::tokenize(const std::string& _expr)
{
    m_tokens.clear();
    m_expression = _expr;
    ot::String::removeControlCharacters(m_expression);
    ot::String::removeWhitespaces(m_expression);
    m_pos = 0;
    m_length = m_expression.size();

    if (m_pos >= m_length)
    {
        throw std::exception("Empty unit expression");
    }
    // An expression has the shape:
    //   numerator [ '/' denominator { '/' denominator } ]
    //
    // Each denominator can be:
    //   - a single term:              "m/s2"
    //   - a parenthesised term list:  "kg*m/(s2*K)"
    //   - another term list without
    //     parens (left-associative):  "m/s/K"  →  m * s^-1 * K^-1
    // -----------------------------------------------------------------------


    // Parse the numerator
    parseTermList(+1);

    // Parse zero or more '/ ...' sections
    while (m_pos < m_length && m_expression[m_pos] == '/')
    {
        ++m_pos; // consume '/'

        if (m_pos < m_length && m_expression[m_pos] == '(')
        {
            // Parenthesised denominator: "/(s2*K)"
            ++m_pos; // consume '('

            if (m_pos >= m_length || m_expression[m_pos] == ')')
            {
                throw std::exception(("Empty parentheses in denominator in \"" + m_expression + "\"").c_str());
            }

            parseTermList(-1);

            if (m_pos >= m_length || m_expression[m_pos] != ')')
            {
                throw std::exception(("Missing closing ')' in \"" + m_expression + "\"").c_str());
            }
            ++m_pos; // consume ')'
        }
        else 
        {
            // Bare denominator term (no parens): "/s2"
            // Only a SINGLE term is consumed here — matching conventional
            // notation where "kg*m/s2*K" would be ambiguous. If the caller
            // intends multiple denominator terms without parens they should
            // chain slashes: "kg*m/s2/K".
            parseTerm(-1);
        }
    }

    // Trailing garbage check
    if (m_pos < m_length)
    {
        throw std::exception(("Unexpected character '" + std::string(1, m_expression[m_pos]) + "' at position " + std::to_string(m_pos) + " in \"" + m_expression + "\"").c_str());
    }

    return m_tokens;
}
