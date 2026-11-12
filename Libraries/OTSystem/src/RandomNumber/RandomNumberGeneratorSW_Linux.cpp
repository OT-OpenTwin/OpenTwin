// @otlicense
// File: RandomNumberGeneratorSW_Linux.cpp
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


// OpenTwin Header
#include "OTSystem/ArchitectureInfo.h"

#ifdef OT_OS_LINUX
#include "OTSystem/RandomNumber/RandomNumberGeneratorSW.h"
#include "OTSystem/Exception.h"

// Std header
#include <cstddef>
#include <cstdint>
#include <cerrno>

// Linux header
#include <sys/types.h>
#include <sys/random.h>

std::vector<uint8_t> ot::RandomNumberGeneratorSW::createRandomBytes(uint64_t _size)
{
    if (_size == 0)
    {
        throw Exception::OutOfBounds("Size must be > 0");
    }

    std::vector<uint8_t> randomBytes(_size);
    size_t filled = 0;

    while (filled < randomBytes.size())
    {
        ssize_t n = getrandom(randomBytes.data() + filled, randomBytes.size() - filled, 0);
        if (n < 0)
        {
            if (errno == EINTR) continue;
            throw Exception::Runtime("Random number generator failed");
        }
        filled += static_cast<size_t>(n);
    }

    return randomBytes;
}

#endif // OT_OS_LINUX
