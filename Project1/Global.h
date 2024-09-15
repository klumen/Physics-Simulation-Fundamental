#ifndef GLOBAL_H
#define GLOBAL_H

#include <cstdio>
#include <string>
#include <cmath>
#include <cwchar>
#include <mutex>
#include <thread>
#include <random>

static unsigned int random_uint(unsigned int l, unsigned int r)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(l, r);

    return dis(gen);
}

static float random_float(float l, float r)
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(l, r);

    return dis(gen);
}

#endif // !GLOBAL_H