#pragma once
#include <string>

struct UnitToken 
{
    std::string m_symbol;   // raw symbol string, e.g. "km", "mol", "degC"
    int         m_exponent; // signed, e.g. -2 for s in "kg*m/s2"
};