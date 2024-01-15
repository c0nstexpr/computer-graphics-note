#include <fstream>
#include <filesystem>
#include <nanobench.h>
#include <print>
#include <iostream>

#include <cxxopts.hpp>

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
    inline constexpr auto output_arg = "output"sv;

    void output_to_file(const ParseResult& result, Bench& b)
    {
        const auto& output = result[output_arg.data()].as<path>();
        const auto& html_plot = htmlBoxplot();
        constexpr auto file_mode = ofstream::out | ofstream::trunc;
        constexpr auto fs_exception_bits = ofstream::failbit | ofstream::badbit;
        const auto& render_html_path = output / "benchmark-render.html";
        ofstream render_fs{render_html_path, file_mode};

        render_fs.exceptions(fs_exception_bits);
        println("output benchmark rendered result to path {}", render_html_path.c_str());
        render(html_plot, b, render_fs);
    }
}

int main(int argc, char* const argv[])
{
    Options options{"Graphics Library Benchmark", "Run benchmark for graphics library"};

    options
        .add_options()("o,output", "Report file directory for benchmark results", value<path>()->default_value("./"))(
            "h,help",
            "Print usage"
        );

    const auto& result = options.parse(argc, argv);

    if(result.count("help") != 0)
    {
        std::println("{}", options.help());
        return 0;
    }

    Bench b;

    raster_triangle_benchmark(b);

    b.relative(false);
    b.minEpochIterations();

    output_to_file(result, b);

    return 0;
}