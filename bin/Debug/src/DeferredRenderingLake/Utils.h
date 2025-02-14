# pragma once

#ifndef UTILS_H
#define UTILS_H

#include <cstdlib>
#include <ctime>


namespace Random
{
    /// Initializes the random number generator (with current time).
    inline static void InitRand(void)
    {
        srand(static_cast<unsigned int>(time(0)));
    }

    /// Generates a random float number between min and max.
    inline static float RandF(float min, float max)
    {
        return min + (rand() / static_cast<float>(RAND_MAX)) * (max - min);
    }

    /// Generates a random int number between min and max.
    inline static int RandI(int min, int max)
    {
        return min + (rand() % (max - min + 1));
    }
}

#endif // UTILS_H
