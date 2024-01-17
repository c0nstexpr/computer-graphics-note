#include <fstream>
#include <filesystem>
#include <nanobench.h>
#include <print>

#include <cxxopts.hpp>
#include <stdsharp/algorithm/algorithm.h>
#include <stdsharp/random/random.h>

#include "rasterization/raster_line.h"
#include "rasterization/raster_triangle.h"

using namespace std;
using namespace filesystem;
using namespace stdsharp;
using namespace glm;
using namespace Catch;
using namespace ankerl::nanobench;
using namespace templates;
using namespace cxxopts;
using namespace graphics::benchmark;

using bench_config = ankerl::nanobench::Config;

namespace
{

    constexpr auto html_template =
#pragma region
        R"(
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

        {
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
        }

        {
            var tableData = [
                {
                    type: 'table',
                    header: {
                        values: [
                            ["Name"],
                            ["Average"],
                            ["Median Absolute Percent Error"]
                        ],
                        align: "center"
                    },
                    cells: {
                        format: [null, "~f", ".2%"],
                        values: [
                            [{{#result}}'{{name}}',{{/result}}],
                            [{{#result}}{{average(elapsed)}},{{/result}}],
                            [{{#result}}{{medianAbsolutePercentError(elapsed)}},{{/result}}],
                            [{{#result}}{{context(relative)}},{{/result}}]
                        ]
                    }
                }
            ];

            var tableDiv = document.createElement("div");

            if({{relative}}) {
                var tb = tableData[0];
                tb.header.values.push(["Relative"]);
                tb.cells.format.push([".2%"]);
            }

            tableDiv.id = "tableDiv" ;
            document.body.appendChild(tableDiv);

            Plotly.newPlot(tableDiv.id, tableData, { title : '{{title}} Elapsed Time' });
        }

    </script>
</body>
</html>
)";
#pragma endregion

    void output_to_file(const ParseResult& result, decay_same_as<Bench> auto&& b)
    {
        const auto& render_html_path =
            result["output"].as<path>() / (b.title() + " benchmark.html");
        ofstream render_fs{render_html_path, ofstream::out | ofstream::trunc};
        vector<Result> results = b.results();

        if(b.relative())
        {
            constexpr auto elapsed_enum = Result::Measure::elapsed;

            for(const auto baseline = results[0].average(elapsed_enum);
                auto& r : results) // NOLINTNEXTLINE(*-const-cast)
                const_cast<unordered_map<string, string>&>(r.config().mContext)["relative"] =
                    to_string(baseline / r.average(elapsed_enum));
        }

        render_fs.exceptions(ofstream::failbit | ofstream::badbit);
        println("output benchmark rendered result to path {}", render_html_path.c_str());
        render(html_template, results, render_fs);
    }
}

int main(int argc, char* const argv[])
{
    constexpr auto draw_line_iterations_arg = "draw-line-iterations";
    constexpr auto raster_triangle_iterations_arg = "raster-triangle-iterations";
    constexpr auto invalid_seed{static_cast<u64>(-1)};
    Options options{"Graphics Library Benchmark", "Run benchmark for graphics library"};

    options.add_options() //
        ("o,output",
         "Report file directory for benchmark results",
         value<path>()->default_value("./")) //
        ("s,seed",
         "Set seed for benchmark results",
         value<u64>()->default_value(std::to_string(invalid_seed))) //
        (raster_triangle_iterations_arg,
         "Set iterations for raster triangle benchmark",
         value<unsigned>()->default_value("10000")) //
        (draw_line_iterations_arg,
         "Set iterations for raster triangle benchmark",
         value<unsigned>()->default_value("10000")) //
        ("h,help", "Print usage");


    const auto& result = options.parse(argc, argv);

    if(result.count("help") != 0)
    {
        std::println("{}", options.help());
        return 0;
    }

    auto seed = result["seed"].as<u64>();

    set_if_less(seed, get_random_device()());

    println("Current random seed: {}", seed);

    output_to_file(
        result,
        raster_triangle(result[raster_triangle_iterations_arg].as<unsigned>(), seed)
    );

    output_to_file(result, raster_line(result[draw_line_iterations_arg].as<unsigned>(), seed));

    return 0;
}