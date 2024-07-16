//
// Created by Maddie on 6/20/2024.
//

#ifndef TIMER_H
#define TIMER_H

#include <chrono>

namespace OpenShaderDesigner
{
    class Timer
    {
    public:
        Timer() : Start(std::chrono::high_resolution_clock::now()) { }

        void Reset() { Start = std::chrono::high_resolution_clock::now(); }

        [[nodiscard]] double Poll() const
        { return std::chrono::duration<double>(std::chrono::high_resolution_clock::now() - Start).count(); }
    private:
        std::chrono::high_resolution_clock::time_point Start;
    };
}

#endif //TIMER_H
