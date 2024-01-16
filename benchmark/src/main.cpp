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
    constexpr auto html_template = R"(
<html>

<head>
    <script src="https://cdn.plot.ly/plotly-latest.min.js"></script>
</head>

<body>
    <script>
        function drawPlot(name, yTitle, data){
            var div = document.createElement("div");

            data = data = data.map(
                obj => Object.assign(
                    obj,
                    { boxpoints: 'all', pointpos: 0, type: 'box', boxmean: 'sd' }
                )
            );

            var layout = {
                title: { text: '{{title}} ' + name },
                yaxis: { title: yTitle, autorange: true }
            };

            div.id = name + "Div" ;
            document.body.appendChild(div);

            Plotly.newPlot(div.id, data, layout, {responsive: true});
        }

        var elapsedData = [
            {{#result}}{
                name: '{{name}}',
                y: [{{#measurement}}{{elapsed}},{{/measurement}}]
            },
            {{/result}}
        ];
        var instructionsData = [
            {{#result}}{
                name: '{{name}}',
                y: [{{#measurement}}{{instructions}},{{/measurement}}]
            },
            {{/result}}
        ];
        var cpucyclesData = [
            {{#result}}{
                name: '{{name}}',
                y: [{{#measurement}}{{cpucycles}},{{/measurement}}]
            },
            {{/result}}
        ];
        var branchinstructionsData = [
            {{#result}}{
                name: '{{name}}',
                y: [{{#measurement}}{{branchinstructions}},{{/measurement}}]
            },
            {{/result}}
        ];
        var branchmissesData = [
            {{#result}}{
                name: '{{name}}',
                y: [{{#measurement}}{{branchmisses}},{{/measurement}}]
            },
            {{/result}}
        ];
        drawPlot('elapsed', 'seconds/iteration', elapsedData);
        drawPlot('instructions', 'count/iteration', instructionsData);
        drawPlot('cpucycles', 'count/iteration', cpucyclesData);
        drawPlot('branchinstructions', 'count/iteration', branchinstructionsData);
        drawPlot('branchmisses', 'count/iteration', branchmissesData);

        var tableData = [
            {
                type: 'table',
                header: {
                    values: [
                        ["Name"],
                        ["Average"],
                        ["Median Absolute Percent Error"],
                        ["Relative"]
                    ],
                    align: "center"
                },
                cells: {
                    values: [
                        [{{#result}}'{{name}}',{{/result}}],
                        [{{#result}}{{average(elapsed)}},{{/result}}],
                        [{{#result}}{{medianAbsolutePercentError(elapsed)}},{{/result}}],
                        [{{#result}}{{context(relative)}},{{/result}}]
                    ]
                }
            }
        ]

        var tableDiv = document.createElement("div");
        tableDiv.id = "tableDiv" ;
        document.body.appendChild(tableDiv);

        Plotly.newPlot(tableDiv.id, tableData);
    </script>
</body>
</html>
    )";

    template<typename T, auto P = &T::mResults>
    constexpr auto& get_results(T& b)
    {
        return b.*P;
    }

    template<typename Tag, typename Tag::type M>
    struct tag_accessor
    {
        friend Tag::type get(const Tag /*unused*/) { return M; }
    };

    struct bench_accessor
    {
        using type = std::vector<Result> Bench::*;
        friend type get(bench_accessor /*unused*/);
    };

    struct result_accessor
    {
        using type = ankerl::nanobench::Config Result::*;
        friend type get(result_accessor /*unused*/);
    };

    template struct tag_accessor<bench_accessor, &Bench::mResults>;
    template struct tag_accessor<result_accessor, &Result::mConfig>;

    void output_to_file(const ParseResult& result, decay_same_as<Bench> auto&& b)
    {
        const auto& output = result["output"].as<path>();
        constexpr auto file_mode = ofstream::out | ofstream::trunc;
        constexpr auto fs_exception_bits = ofstream::failbit | ofstream::badbit;
        const auto& render_html_path = output / (b.title() + " benchmark.html");

        ofstream render_fs{render_html_path, file_mode};

        if(b.relative())
        {
            auto& results = b.*get(bench_accessor{});

            for(const auto baseline = results.front().average(Result::Measure::elapsed);
                auto& r : results)
                (r.*get(result_accessor{})).mContext["relative"] =
                    to_string(baseline / r.average(Result::Measure::elapsed));
        }

        render_fs.exceptions(fs_exception_bits);
        println("output benchmark rendered result to path {}", render_html_path.c_str());
        render(html_template, b, render_fs);
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