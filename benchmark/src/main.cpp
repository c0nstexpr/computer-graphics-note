#include <chrono>
#include <fstream>
#include <filesystem>
#include <print>

#include <cxxopts.hpp>

#include "rasterization/raster_line.h"
#include "rasterization/raster_triangle.h"

using namespace std;
using namespace filesystem;
using namespace stdsharp;
using namespace ankerl::nanobench;
using namespace templates;
using namespace cxxopts;
using namespace graphics::benchmark;

using bench_config = ankerl::nanobench::Config;

namespace
{
    constexpr auto html_template =
#pragma region
        R"template(
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
                            ["Total(s)"],
                            ["Average(ms)"],
                            ["Median Absolute Percent Error"]
                        ],
                        align: "center"
                    },
                    cells: {
                        format: [null, ".2f", ".2f", ".2%"],
                        values: [
                            [{{#result}}'{{name}}',{{/result}}],
                            [{{#result}}{{sumProduct(iterations, elapsed)}},{{/result}}],
                            [{{#result}}{{average(elapsed)}} * 1000,{{/result}}],
                            [{{#result}}{{medianAbsolutePercentError(elapsed)}},{{/result}}],
                            [{{#result}}{{context(relative)}},{{/result}}]
                        ]
                    }
                }
            ];

            var tableDiv = document.createElement("div");
            var minTime = {{epochs}} * {{minEpochTime}};

            if({{relative}}) {
                var tb = tableData[0];
                tb.header.values.push(["Relative"]);
                tb.cells.format.push([".2%"]);
            }

            tableDiv.id = "tableDiv" ;
            document.body.appendChild(tableDiv);

            Plotly.newPlot(tableDiv.id, tableData, { title : `{{title}} Elapsed (${minTime}s Minimum)` });
        }

    </script>
</body>
</html>
)template";
#pragma endregion

    void output_to_file(const path& dir, decay_same_as<Bench> auto&& b)
    {
        const auto& render_html_path = dir / (b.title() + " benchmark.html");
        ofstream render_fs{render_html_path, ofstream::out | ofstream::trunc};
        vector<Result> results = b.results();

        if(constexpr auto elapsed_enum = Result::Measure::elapsed; b.relative())
            for(const auto baseline = results[0].average(elapsed_enum); auto& r : results)
            { // NOLINTNEXTLINE(*-const-cast)
                auto& context = const_cast<unordered_map<string, string>&>(r.config().mContext);
                context["relative"] = to_string(baseline / r.average(elapsed_enum));
            }

        render_fs.exceptions(ofstream::failbit | ofstream::badbit);
        println("output benchmark rendered result to path {}", render_html_path.c_str());
        render(html_template, results, render_fs);
    }
}

int main(int argc, char* const argv[])
{
    constexpr auto epoch_arg = "epoch";
    constexpr auto min_epoch_time_arg = "min-epoch-time";
    Options options{"Graphics Library Benchmark", "Run benchmark for graphics library"};

    options.add_options() //
        ("o,output",
         "Report file directory for results",
         value<path>()->default_value("./")) //
        ("e,"s + epoch_arg,
         "Set epoch times",
         value<unsigned>()->default_value("10")) //
        ("m,"s + min_epoch_time_arg,
         "Set minimum time each epoch should take(ms)",
         value<unsigned>()->default_value("100")) //
        ("h,help", "Print usage");

    try
    {
        const auto& result = options.parse(argc, argv);
        const auto bench_fn =
            [epoch = result[epoch_arg].as<unsigned>(),
             min_epoch_time =
                 chrono::milliseconds{result[min_epoch_time_arg].as<unsigned>()}](Bench& b)
        {
            b.epochs(epoch).minEpochTime(min_epoch_time).warmup(10); //
        };

        if(result.count("help") != 0)
        {
            std::println("{}", options.help());
            return 0;
        }

        for( //
            const auto& out_dir = result["output"].as<path>(); //
            const auto& b :
            {
                raster_line(bench_fn),
                raster_triangle(bench_fn) //
            } //
        )
            output_to_file(out_dir, b);
    }
    catch(const exception& e)
    {
        println("{}", e.what());
        return 1;
    }

    return 0;
}