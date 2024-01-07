#include "test.h"

std::mt19937_64& get_mt_engine()
{
    static thread_local std::mt19937_64 e{Catch::getSeed()};
    return e;
}