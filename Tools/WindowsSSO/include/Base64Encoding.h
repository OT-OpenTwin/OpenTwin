#pragma once
#include <string>
#include <vector>

std::string encode(std::vector<unsigned char> _token);

std::vector<unsigned char> decode(const std::string& _token);