//
// Created by Maddie on 6/18/2024.
//

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
