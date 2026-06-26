#pragma once
namespace ot
{
    static size_t stringHeapSize(const std::string& s)
    {
        // If the string is using the SSO buffer, no heap allocation exists.
        // The threshold is implementation-defined but capacity() == sizeof(s)-1
        // is a reliable indicator on most platforms (MSVC: 15, libstdc++: 15).
        // A portable approach: only count heap bytes beyond what sizeof already covers.
        constexpr size_t ssoCapacity = sizeof(std::string) - 1;
        if (s.capacity() <= ssoCapacity)
            return 0;          // data lives inside the object, already counted
        return s.capacity();   // heap buffer, not yet counted
    }
}

