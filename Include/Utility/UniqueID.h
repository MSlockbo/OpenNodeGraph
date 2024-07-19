// =====================================================================================================================
// Copyright 2024 Medusa Slockbower
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// 	http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// =====================================================================================================================

#ifndef ENGINE_UNIQUEID_H
#define ENGINE_UNIQUEID_H

#include <cstdint>

#ifdef _MSC_VER
#define COUNTER __declspec(selectany)
#endif

namespace OpenShaderDesigner
{
    template<typename Base> uint64_t _Increment()
    {
        static uint64_t current = 0;
        return current++;
    }

    // Unfortunately adds a little bit of overhead at runtime
    template<typename Base, typename Type> uint64_t UniqueID()
    {
        static bool initialized = false;
        static uint64_t id = 0;

        if(initialized) return id;
        initialized = true;
        return id = _Increment<Base>();
    }
}

#endif //ENGINE_UNIQUEID_H
