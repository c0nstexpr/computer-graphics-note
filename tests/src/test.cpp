#include "test.h"

#include <catch2/catch_get_random_seed.hpp>
#include <stdsharp/random/random.h>
#include <print>

using namespace std;

mt19937_64& get_mt_engine()
{
    static thread_local mt19937_64 e{stdsharp::get_random_device()()};
    return e;
}