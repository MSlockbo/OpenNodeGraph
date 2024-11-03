// =====================================================================================================================
//  open-cpp-utils, an open-source cpp library with data structures that extend the STL.
//  Copyright (C) 2024  Medusa Slockbower
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License
//  along with this program.  If not, see <https://www.gnu.org/licenses/>.
// =====================================================================================================================

#ifndef OPEN_CPP_UTILS_STARTUP_H
#define OPEN_CPP_UTILS_STARTUP_H

#ifdef __cplusplus
#define STARTUP(f) \
        inline static void f(void); \
        struct f##_t_ { inline f##_t_(void) { f(); } }; inline static f##_t_ f##_; \
        inline static void f(void)
#elif defined(_MSC_VER)
#pragma section(".CRT$XCU",read)
    #define INITIALIZER2_(f,p) \
        static void f(void); \
        __declspec(allocate(".CRT$XCU")) void (*f##_)(void) = f; \
        __pragma(comment(linker,"/include:" p #f "_")) \
        static void f(void)
    #ifdef _WIN64
        #define STARTUP(f) INITIALIZER2_(f,"")
    #else
        #define STARTUP(f) INITIALIZER2_(f,"_")
    #endif
#else
    #define STARTUP(f) \
        static void f(void) __attribute__((constructor)); \
        static void f(void)
#endif

#endif // OPEN_CPP_UTILS_STARTUP_H
