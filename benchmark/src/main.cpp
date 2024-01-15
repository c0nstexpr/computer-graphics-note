#include <fstream>
#include <filesystem>
#include <nanobench.h>
#include <print>

#include <cxxopts.hpp>
#include <stdsharp/random/random.h>

#include "rasterization/raster_triangle.h"

using namespace std;
using namespace filesystem;
using namespace stdsharp;
using namespace glm;
using namespace Catch;
using namespace ankerl::nanobench;
using namespace templates;
using namespace cxxopts;

namespace
{
    void output_to_file(const ParseResult& result, const Bench& b)
    {
        const auto& output = result["output"].as<path>();
        const auto& html_plot = csv();
        constexpr auto file_mode = ofstream::out | ofstream::trunc;
        constexpr auto fs_exception_bits = ofstream::failbit | ofstream::badbit;
        const auto& render_html_path = output / (b.name() + ".csv");

        ofstream render_fs{render_html_path, file_mode};

        render_fs.exceptions(fs_exception_bits);
        println("output benchmark rendered result to path {}", render_html_path.c_str());
        render(html_plot, b, render_fs);
    }
}

int main(int argc, char* const argv[])
{
    constexpr auto invalid_seed{static_cast<u64>(-1)};
    Options options{"Graphics Library Benchmark", "Run benchmark for graphics library"};

    {
        options.add_options() //
            ("o,output",
             "Report file directory for benchmark results",
             value<path>()->default_value("./")) //
            ("s,seed",
             "Set seed for benchmark results",
             value<u64>()->default_value(std::to_string(invalid_seed))) //
            ("h,help", "Print usage");
    }

    const auto& result = options.parse(argc, argv);

    if(result.count("help") != 0)
    {
        std::println("{}", options.help());
        return 0;
    }

    auto seed = get_random_device()();

    if(const auto seed_arg = result["seed"].as<u64>(); seed_arg != invalid_seed) seed = seed_arg;

    println("Current random seed: {}", seed);

    output_to_file(result, raster_triangle_benchmark(seed));

    return 0;
}